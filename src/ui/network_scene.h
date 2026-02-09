#pragma once

#include <QGraphicsScene>

class NetworkScene : public QGraphicsScene {
    Q_OBJECT
   public:
    NetworkScene(QObject* parent = nullptr);
};