#include "mainwindow.h"

#include <qnamespace.h>

#include <QGraphicsScene>
#include <QGraphicsView>

#include "intersection.h"
#include "network.h"
#include "network_view.h"
#include "node.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    network_ = new Network();

    scene_ = new QGraphicsScene(this);

    view_ = new NetworkView(this);
    view_->setScene(scene_);
    view_->setRenderHint(QPainter::Antialiasing);
    setCentralWidget(view_);

    for (const auto& [_, edge] : network_->edges()) {
        QPointF startPos(edge.entry().position().x, edge.entry().position().y);
        QPointF endPos(edge.exit().position().x, edge.exit().position().y);
        scene_->addLine(QLineF(startPos, endPos), QPen(Qt::yellow));
    }

    // for (const auto& [_, intersection] : network_->intersections()) {
    //     int radius = 100;
    //     QPointF pos(intersection.pos().x, intersection.pos().y);
    //     scene_->addEllipse(QRectF(pos - QPointF(radius / 2.0, radius / 2.0),
    //                               QSizeF(radius, radius)),
    //                        QPen(Qt::gray));

    //     for (const auto& [_, edge] : intersection.structure().edges()) {
    //         QPointF edgePos(edge.position().x, edge.position().y);
    //         utils::Position headingPos(edge.position() +
    //                                   utils::Vector2::fromAngle(edge.heading())
    //                                   *
    //                                       10);
    //         QPointF edgeHeading(headingPos.x, headingPos.y);
    //         scene_->addLine(QLineF(edgePos, edgeHeading), QPen(Qt::yellow));
    //         scene_->addEllipse(
    //             QRectF(edgePos - QPointF(1.0, 1.0), QSizeF(2.0, 2.0)),
    //             QPen(Qt::red));
    //     }
    // }
}

MainWindow::~MainWindow() {}
