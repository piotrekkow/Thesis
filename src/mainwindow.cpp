#include "mainwindow.h"

#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qnamespace.h>

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QPainterPath>

#include "conflict/map_factory.h"
#include "core/geometry/calculators/edge_factory.h"
#include "core/geometry/calculators/movement_factory.h"
#include "core/geometry/crossing.h"
#include "debug_sink.h"
#include "geometry/calculators/crossing_factory.h"
#include "geometry/calculators/edge_factory.h"
#include "geometry/calculators/movement_factory.h"
#include "id.h"
#include "intersection.h"
#include "network.h"
#include "network_view.h"
#include "overloaded.h"
#include "position.h"
#include "topology/node.h"
#include "vector2.h"

namespace {
void drawCross(QGraphicsScene* scene, utils::Position pos, double size,
               QPen pen) {
    utils::Vector2 vx(size / 2.0, size / 2.0);
    for (size_t i = 0; i < 2; ++i) {
        auto x = pos + vx.rotate(1.5707 * i);
        auto y = pos - vx.rotate(1.5707 * i);
        scene->addLine(x.x, x.y, y.x, y.y, pen);
    }
}
void drawCricle(QGraphicsScene* scene, utils::Position pos, double diameter,
                QPen pen) {
    scene->addEllipse(pos.x - diameter / 2, pos.y - diameter / 2, diameter,
                      diameter, pen);
}
}  // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    network_ = new Network();

    scene_ = new QGraphicsScene(this);
    DebugSink::setScene(scene_);

    view_ = new NetworkView(this);
    view_->setScene(scene_);
    view_->setRenderHint(QPainter::Antialiasing);
    setCentralWidget(view_);

    for (const auto& [id, edge] : network_->edges()) {
        QPointF startPos(edge.exit().position().x, edge.exit().position().y);
        QPointF endPos(edge.entry().position().x, edge.entry().position().y);
        scene_->addLine(QLineF(startPos, endPos), QPen(Qt::yellow, 0.2));

        auto geo = geometry::EdgeFactory::build(*network_, id);
        for (const auto& line : geo.entries()) {
            QPointF p1(line.p1().x, line.p1().y);
            QPointF p2(line.p2().x, line.p2().y);
            scene_->addLine(QLineF(p1, p2), QPen(Qt::green, 0.2));
            scene_->addEllipse(p1.x() - 0.2, p1.y() - 0.2, 0.4, 0.4,
                               QPen(Qt::green, 0.2));
        }
        for (const auto& line : geo.exits()) {
            QPointF p1(line.p1().x, line.p1().y);
            QPointF p2(line.p2().x, line.p2().y);
            scene_->addLine(QLineF(p1, p2), QPen(Qt::gray, 0.2));
            scene_->addEllipse(p1.x() - 0.2, p1.y() - 0.2, 0.4, 0.4,
                               QPen(Qt::gray, 0.2));
        }
    }

    for (const auto& [iId, intersection] : network_->intersections()) {
        for (const auto& [id, node] : intersection.nodes()) {
            // Only process nodes with valid movement structures
            if (!node.movementStructure().has_value()) {
                continue;  // Skip nodes without movement structure
            }

            auto movementGeos = geometry::MovementFactory::build(
                *network_, node.movementStructure().value());

            // draw movement paths
            for (const auto& [mId, movement] : movementGeos) {
                for (const auto& path : movement.paths()) {
                    QPainterPath ppath;
                    bool first = true;

                    for (const auto& p : path.positions()) {
                        if (first) {
                            ppath.moveTo(QPointF(p.x, p.y));
                            first = false;
                        } else {
                            ppath.lineTo(QPointF(p.x, p.y));
                        }
                    }
                    scene_->addPath(ppath, QPen(Qt::darkCyan, 0.1));
                    // double maxK = utils::maxCurvature(path);
                    // auto* textItem = scene_->addText(QString::number(maxK),
                    //                                  QFont("Arial", 1));
                    // textItem->setPos(path.positions().front().x,
                    //                  path.positions().front().y);
                }
            }

            // get all crossing geometry
            std::unordered_map<CrossingId, geometry::Crossing> cGeo;
            for (const auto& [crossingId, crossing] : node.crossings()) {
                cGeo.insert({crossingId, geometry::CrossingFactory::build(
                                             *network_, crossingId)});
            }

            for (const auto& [_, geo] : cGeo) {
                QPointF p1(geo.rims().first.p1().x, geo.rims().first.p1().y);
                QPointF p2(geo.rims().first.p2().x, geo.rims().first.p2().y);
                QPointF p3(geo.rims().second.p1().x, geo.rims().second.p1().y);
                QPointF p4(geo.rims().second.p2().x, geo.rims().second.p2().y);

                scene_->addEllipse(p1.x() - 0.1, p1.y() - 0.1, 0.2, 0.2,
                                   QPen(Qt::darkRed, 0.1));

                scene_->addLine(QLineF(p1, p2), QPen(Qt::darkRed, 0.1));
                scene_->addLine(QLineF(p3, p4), QPen(Qt::darkRed, 0.1));
            }

            auto conflictMap = conflict::MapFactory::compute(
                movementGeos, cGeo, node.movementStructure().value());
            // draw collision points
            for (const auto& [cSId, eConflicts] : conflictMap) {
                for (const auto& [_, cpoints] : eConflicts) {
                    for (const auto& cp : cpoints) {
                        std::visit(
                            overloaded{[&](MovementId) {
                                           drawCricle(scene_, cp.position(),
                                                      0.3,
                                                      QPen(Qt::white, 0.1));
                                       },
                                       [&](CrossingId) {
                                           drawCross(scene_, cp.position(), 0.3,
                                                     QPen(Qt::white, 0.1));
                                       }},
                            cSId);
                    }
                }
            }
        }

        if (const auto& im = intersection.controller().intergreenMatrix()) {
            const auto& controller = intersection.controller();
            const auto& sgs = controller.signalGroups();

            // Collect and sort IDs for stable ordering
            QList<SignalGroupId> ids;
            for (const auto& [id, _] : sgs) ids.append(id);
            std::sort(ids.begin(), ids.end(), [](const auto& a, const auto& b) {
                return a.value() < b.value();
            });

            auto sgTag = [](SignalGroup::Type t) -> char {
                switch (t) {
                    case SignalGroup::Type::TRAM_T:
                        return 'T';
                    case SignalGroup::Type::PROTECTED_T:
                        return 'T';
                    case SignalGroup::Type::BUS_B:
                        return 'B';
                    case SignalGroup::Type::GENERAL_K:
                        return 'K';
                    case SignalGroup::Type::PROTECTED_K:
                        return 'K';
                    case SignalGroup::Type::CONDITIONAL_ARROW_S:
                        return 'S';
                    case SignalGroup::Type::PEDESTRIAN_P:
                        return 'P';
                    case SignalGroup::Type::CYCLIST_R:
                        return 'R';
                }
                return '?';
            };

            auto label = [&](SignalGroupId id) {
                return QString("%1%2")
                    .arg(id.value())
                    .arg(sgTag(sgs.at(id).type()));
            };

            // Draw signal group labels at center of mass of their geometry
            {
                std::unordered_map<MovementId, std::vector<QPointF>>
                    movementFirstPts;
                std::unordered_map<CrossingId, std::vector<QPointF>>
                    crossingPts;

                for (const auto& [nId, node] : intersection.nodes()) {
                    if (!node.movementStructure().has_value()) continue;
                    auto movGeos = geometry::MovementFactory::build(
                        *network_, node.movementStructure().value());
                    for (const auto& [mId, movement] : movGeos) {
                        for (const auto& path : movement.paths()) {
                            if (!path.positions().empty())
                                movementFirstPts[mId].emplace_back(
                                    path.positions().front().x,
                                    path.positions().front().y);
                        }
                    }
                    for (const auto& [crossingId, _c] : node.crossings()) {
                        auto geo = geometry::CrossingFactory::build(*network_,
                                                                    crossingId);
                        crossingPts[crossingId].emplace_back(
                            geo.rims().first.p1().x, geo.rims().first.p1().y);
                        crossingPts[crossingId].emplace_back(
                            geo.rims().first.p2().x, geo.rims().first.p2().y);
                        crossingPts[crossingId].emplace_back(
                            geo.rims().second.p1().x, geo.rims().second.p1().y);
                        crossingPts[crossingId].emplace_back(
                            geo.rims().second.p2().x, geo.rims().second.p2().y);
                    }
                }

                for (const auto& [sgId, sg] : sgs) {
                    std::vector<QPointF> pts;
                    for (const auto& stream : sg.streams()) {
                        std::visit(
                            overloaded{[&](MovementId mId) {
                                           auto it = movementFirstPts.find(mId);
                                           if (it != movementFirstPts.end())
                                               for (const auto& p : it->second)
                                                   pts.push_back(p);
                                       },
                                       [&](CrossingId cId) {
                                           auto it = crossingPts.find(cId);
                                           if (it != crossingPts.end())
                                               for (const auto& p : it->second)
                                                   pts.push_back(p);
                                       }},
                            stream);
                    }
                    if (pts.empty()) continue;
                    QPointF center(0, 0);
                    for (const auto& p : pts) center += p;
                    center /= static_cast<qreal>(pts.size());

                    auto* textItem =
                        scene_->addText(label(sgId), QFont("Arial", 1));
                    textItem->setDefaultTextColor(Qt::yellow);
                    textItem->setPos(
                        center.x() - textItem->boundingRect().width() / 2,
                        center.y() - textItem->boundingRect().height() / 2);

                    QRectF bgRect(center.x() - textItem->textWidth(),
                                  center.y() - 1.25, textItem->textWidth() * 2,
                                  1.5);
                    scene_->addRect(bgRect, Qt::NoPen,
                                    QBrush(QColor(0, 0, 0, 188)));

                    textItem->setZValue(1);
                }
            }

            const int W = 5;  // column width

            // Header row: blank corner cell, then entering IDs
            QString header = QString("%1").arg("", W);
            for (const auto& eId : ids)
                header += QString("%1").arg(label(eId), W);

            // Data rows: clearing ID label, then entry times (or "-")
            QString matrixTable = header;
            for (const auto& cId : ids) {
                QString row = QString("%1").arg(label(cId), W);
                for (const auto& eId : ids) {
                    if (const auto* e = im->entry(cId, eId))
                        row += QString("%1").arg(e->time(), W);
                    else
                        row += QString("%1").arg("-", W);
                }
                matrixTable += "\n" + row;
            }
            qDebug().noquote() << "Intergreen matrix" << iId << ":\n"
                               << matrixTable << '\n';

            //
            // can proceed simultaneously matrix
            //

            QString psMatrixTable = header;
            for (const auto& cId : ids) {
                QString row = QString("%1").arg(label(cId), W);
                for (const auto& eId : ids) {
                    if (const auto* e = im->entry(cId, eId))
                        if (e->canProceedSimultaneously())
                            row += QString("%1").arg('y', W);
                        else
                            row += QString("%1").arg('n', W);

                    else
                        row += QString("%1").arg("-", W);
                }
                psMatrixTable += "\n" + row;
            }
            qDebug().noquote() << "Can proceed simultaneously" << iId << ":\n"
                               << psMatrixTable << '\n';

            //
            // calculation block
            //

            // Header
            const char* sep = "\t";
            QStringList cheader = {"cId", "eId", "v_c", "l_v", "s_c",
                                   "t_c", "v_e", "a_e", "s_e", "rest",
                                   "t_e", "t_y", "raw"};
            QString ctable = cheader.join(sep);

            auto fmtOpt = [](std::optional<double> v) {
                return v ? QString::number(*v, 'f', 2) : QString("-");
            };
            auto fmt = [](double v) { return QString::number(v, 'f', 2); };

            for (const auto& [cId, _c] : sgs) {
                for (const auto& [eId, _e] : sgs) {
                    const auto* e = im->entry(cId, eId);
                    if (!e) continue;

                    const auto d = e->data();
                    double t_c = (d.s_c + d.l_v) / d.v_c;
                    double t_e = 0;
                    if (d.v_e && d.a_e)
                        t_e = e->isEntryFromRest()
                                  ? (d.s_e / *d.v_e) + d.deltaRollingEntryTime
                                  : std::sqrt((2.0 * d.s_e + 1.5) / *d.a_e);

                    QStringList row = {
                        label(cId),    label(eId),
                        fmt(d.v_c),    fmt(d.l_v),
                        fmt(d.s_c),    fmt(t_c),
                        fmtOpt(d.v_e), fmtOpt(d.a_e),
                        fmt(d.s_e),    e->isEntryFromRest() ? "yes" : "no",
                        fmt(t_e),      QString::number(d.t_y),
                        fmt(e->raw()),
                    };
                    ctable += "\n" + row.join(sep);
                }
            }

            qDebug().noquote() << "Intergreen data" << iId << ":\n" << ctable;
        }
    }
}

MainWindow::~MainWindow() {}