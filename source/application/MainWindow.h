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
    // Q_OBJECT
    W_OBJECT(MainWindow)

public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow();

public slots:
    void on_quitAction_triggered();
    W_SLOT(on_quitAction_triggered)

    void on_aboutAction_triggered();
    W_SLOT(on_aboutAction_triggered)

    void on_aboutQtAction_triggered();
    W_SLOT(on_aboutQtAction_triggered)

    void on_gpxView_activated(const QModelIndex &index);
    W_SLOT(on_gpxView_activated)

private:
    Ui::MainWindow * ui;

    QQuickView * quickView;
    PathController * pathController;
};

#endif // APPLICATION_MAINWINDOW_H
