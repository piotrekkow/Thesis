#include "debug_sink.h"

#include <QGraphicsScene>

QGraphicsScene* DebugSink::scene_ = nullptr;

void DebugSink::setScene(QGraphicsScene* scene) { scene_ = scene; }

void DebugSink::drawPoint(utils::Position p, QColor c) {
    if (!scene_) return;
    scene_->addEllipse(p.x - 0.5, p.y - 0.5, 1, 1, QPen(c, 0.5));
}

void DebugSink::drawLine(utils::Line l, QColor c) {
    if (!scene_) return;
    scene_->addLine(l.p1().x, l.p1().y, l.p2().x, l.p2().y, QPen(c, 0.2));
}