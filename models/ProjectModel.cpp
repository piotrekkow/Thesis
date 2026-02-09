#include "ProjectModel.h"
#include <QMetaProperty>

ProjectModel::ProjectModel(Project *project, QObject *parent)
    : QAbstractItemModel(parent), m_project(project) {}

QVariant ProjectModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  QObject *item = static_cast<QObject *>(index.internalPointer());

  if (role == Qt::DisplayRole) {
    // Try to find a "name" or "id" property, or use class name
    const QMetaObject *meta = item->metaObject();
    int idIndex = meta->indexOfProperty("id");
    if (idIndex != -1) {
      return meta->property(idIndex).read(item);
    }
    return meta->className();
  }

  return QVariant();
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  return QAbstractItemModel::flags(index);
}

QVariant ProjectModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return QStringLiteral("Hierarchy");

  return QVariant();
}

QModelIndex ProjectModel::index(int row, int column,
                                const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  QObject *parentItem;

  if (!parent.isValid())
    parentItem = m_project;
  else
    parentItem = static_cast<QObject *>(parent.internalPointer());

  const QObjectList &children = parentItem->children();
  if (row < children.count())
    return createIndex(row, column, children.at(row));

  return QModelIndex();
}

QModelIndex ProjectModel::parent(const QModelIndex &index) const {
  if (!index.isValid())
    return QModelIndex();

  QObject *childItem = static_cast<QObject *>(index.internalPointer());
  QObject *parentItem = childItem->parent();

  if (parentItem == m_project || parentItem == nullptr)
    return QModelIndex();

  QObject *grandparentItem = parentItem->parent();
  // Simplified: assuming hierarchy matches containment.
  // If grandparent is null (and parent is project), row is index in project
  // children. But here parentItem needs to be found in grandparent's children.

  QObject *root = m_project;
  if (parentItem == root)
    return QModelIndex();

  // Find parent's row in grandparent
  QObject *grandparent = parentItem->parent();
  if (!grandparent)
    return QModelIndex(); // Should not happen if strictly under project

  const QObjectList &children = grandparent->children();
  int row = children.indexOf(parentItem);
  return createIndex(row, 0, parentItem);
}

int ProjectModel::rowCount(const QModelIndex &parent) const {
  QObject *parentItem;
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    parentItem = m_project;
  else
    parentItem = static_cast<QObject *>(parent.internalPointer());

  return parentItem->children().count();
}

int ProjectModel::columnCount(const QModelIndex &parent) const { return 1; }
