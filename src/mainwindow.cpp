#include "mainwindow.h"

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
            for (const auto& [_, movement] : movementGeos) {
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

        // if (intersection.controller().intergreenMatrix().has_value()) {
        //     const auto& im =
        //         intersection.controller().intergreenMatrix().value();

        //     qDebug() << "Intergreen matrix" << iId << ":";
        //     for (const auto& [clearingSgId, enteringIgMap] : im.matrix()) {
        //         auto clearingSg =
        //             intersection.controller().signalGroup(clearingSgId);
        //         int cSgL = clearingSg.label();
        //         char cSgT = tag(clearingSg.type());

        //         for (const auto& [enteringSgId, igTime] : enteringIgMap) {
        //             auto enteringSg =
        //                 intersection.controller().signalGroup(enteringSgId);
        //             int eSgL = enteringSg.label();
        //             char eSgT = tag(enteringSg.type());

        //             qDebug()
        //                 << "    " << QString::number(cSgL) + cSgT << "->"
        //                 << QString::number(eSgL) + eSgT << ":"
        //                 << igTime.minIntergreenTime()

        //                 << "s_c:" << QString::number(igTime.data().s_c, 'f',
        //                 2)
        //                 << "l_v:" << QString::number(igTime.data().l_v, 'f',
        //                 2)
        //                 << "v_c:" << QString::number(igTime.data().v_c, 'f',
        //                 2)
        //                 << "/"
        //                 << "t_c:"
        //                 << QString::number(
        //                        (igTime.data().s_c + igTime.data().l_v) /
        //                            igTime.data().v_c,
        //                        'f', 2)
        //                 << "///"
        //                 << "s_e:" << QString::number(igTime.data().s_e, 'f',
        //                 2)
        //                 << "v_e:" << QString::number(igTime.data().v_e, 'f',
        //                 2)
        //                 << "/"
        //                 << "t_e:"
        //                 << QString::number(
        //                        igTime.data().s_e / igTime.data().v_e, 'f', 2)
        //                 << "///"
        //                 << "t_y:" << QString::number(igTime.data().t_y, 'f',
        //                 2)
        //                 << "/ isSimultaneousPermitted:"
        //                 << igTime.isSimultaneousMovementPermitted();

        //             scene_->addEllipse(igTime.position().x - 0.2,
        //                                igTime.position().y - 0.2, 0.4, 0.4,
        //                                QPen(Qt::red, 0.15));
        //         }
        //     }
        // }
    }
}

MainWindow::~MainWindow() {}