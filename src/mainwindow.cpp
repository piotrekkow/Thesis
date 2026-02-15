#include "mainwindow.h"

#include <qnamespace.h>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainterPath>

#include "collision_map.h"
#include "core/geometry/calculators/edge_factory.h"
#include "core/geometry/calculators/movement_factory.h"
#include "core/geometry/crossing.h"
#include "core/geometry/movement_map.h"
#include "crossing_collision_map.h"
#include "debug_sink.h"
#include "geometry/calculators/crossing_factory.h"
#include "geometry/calculators/edge_factory.h"
#include "geometry/calculators/movement_factory.h"
#include "network.h"
#include "network_view.h"
#include "topology/intersection.h"
#include "topology/node.h"

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

    for (const auto& [_, intersection] : network_->intersections()) {
        for (const auto& [id, node] : intersection.nodes()) {
            // Only process nodes with valid movement structures
            if (!node.movementStructure().has_value()) {
                continue;  // Skip nodes without movement structure
            }

            auto movementMap = geometry::MovementFactory::build(
                *network_, node.movementStructure().value());

            // draw movement paths
            for (const auto& [_, movement] : movementMap.movements()) {
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
                }
            }

            auto collisionMap = CollisionMap::build(
                movementMap, node.movementStructure().value());
            // draw collision points
            for (const auto& [movementId, movCollisions] : collisionMap.map()) {
                for (const auto& cp : movCollisions.points()) {
                    scene_->addEllipse(cp.position().x - 0.1,
                                       cp.position().y - 0.1, 0.2, 0.2,
                                       QPen(Qt::white, 0.1));
                }
            }

            // get all crossing geometry
            std::unordered_map<CrossingId, geometry::Crossing> cGeo;
            for (const auto& [id, crossing] : node.crossings()) {
                cGeo.insert(
                    {id, geometry::CrossingFactory::build(*network_, id)});
            }

            for (const auto& [id, geo] : cGeo) {
                QPointF p1(geo.rims().first.p1().x, geo.rims().first.p1().y);
                QPointF p2(geo.rims().first.p2().x, geo.rims().first.p2().y);
                QPointF p3(geo.rims().second.p1().x, geo.rims().second.p1().y);
                QPointF p4(geo.rims().second.p2().x, geo.rims().second.p2().y);

                scene_->addEllipse(p1.x() - 0.1, p1.y() - 0.1, 0.2, 0.2,
                                   QPen(Qt::darkRed, 0.1));

                scene_->addLine(QLineF(p1, p2), QPen(Qt::darkRed, 0.1));
                scene_->addLine(QLineF(p3, p4), QPen(Qt::darkRed, 0.1));
            }

            auto crossingCollisionMap =
                CrossingCollisionMap::build(movementMap, cGeo);
            // draw collision points
            for (const auto& [crossingId, crossingCollisions] :
                 crossingCollisionMap.map()) {
                for (const auto& cp : crossingCollisions.points()) {
                    scene_->addEllipse(cp.first.position().x - 0.1,
                                       cp.first.position().y - 0.1, 0.2, 0.2,
                                       QPen(Qt::black, 0.1));
                    scene_->addEllipse(cp.second.position().x - 0.1,
                                       cp.second.position().y - 0.1, 0.2, 0.2,
                                       QPen(Qt::black, 0.1));
                }
            }
        }
    }
}

MainWindow::~MainWindow() {}
