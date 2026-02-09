#pragma once

#include <QMainWindow>

class Network;
class QGraphicsScene;
class NetworkView;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private:
    Network* network_;
    QGraphicsScene* scene_;
    NetworkView* view_;
};
