#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include "../domain/Project.h"
#include <QAbstractItemModel>


class ProjectModel : public QAbstractItemModel {
  Q_OBJECT

public:
  explicit ProjectModel(Project *project, QObject *parent = nullptr);

  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
  Project *m_project;
};

#endif // PROJECTMODEL_H
