#ifndef APPLICATION_MAINWINDOW_H
#define APPLICATION_MAINWINDOW_H

#include <wobjectdefs.h>

#include <QMainWindow>

#include <QQuickWidget>
#include <QQuickView>

namespace Ui {
class MainWindow;
}

class QModelIndex;
class PathController;

class MainWindow: public QMainWindow
{
    W_OBJECT(MainWindow)

public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow() override;

public slots:
    void on_quitAction_triggered();
    W_SLOT(on_quitAction_triggered)

    void on_aboutAction_triggered();
    W_SLOT(on_aboutAction_triggered)

    void on_aboutQtAction_triggered();
    W_SLOT(on_aboutQtAction_triggered)

    void on_progressChartAction_triggered();
    W_SLOT(on_progressChartAction_triggered)

    void on_gpxView_activated(const QModelIndex &index);
    W_SLOT(on_gpxView_activated)

    void on_totalDistanceComputed(double totalDistanxe);
    W_SLOT(on_totalDistanceComputed)

private:
    Ui::MainWindow * ui;

    QQuickView * quickView;
    PathController * pathController;
};

#endif // APPLICATION_MAINWINDOW_H
