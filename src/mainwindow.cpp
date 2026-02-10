#include "mainwindow.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainterPath>

#include "debug_sink.h"
#include "geometry/calculators/edge_calculator.h"
#include "geometry/calculators/movement_calculator.h"
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

        auto geo = geometry::EdgeCalculator::compute(*network_, id);
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

            for (const auto& fromEdgeId : node.incomingEdges()) {
                auto groups = geometry::MovementCalculator::compute(
                    *network_, fromEdgeId, id);

                for (const auto& [_, mov] : groups) {
                    for (const auto& path : mov.paths()) {
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
                        scene_->addPath(ppath, QPen(Qt::cyan, 0.1));
                    }
                }
            }
        }
    }
}

MainWindow::~MainWindow() {}
