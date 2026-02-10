#pragma once

#include <QColor>

#include "utils/line.h"
#include "utils/position.h"

class QGraphicsScene;

struct DebugSink {
    static void setScene(QGraphicsScene* scene);
    static void drawPoint(utils::Position p, QColor c);
    static void drawLine(utils::Line l, QColor c);

   private:
    static QGraphicsScene* scene_;
};