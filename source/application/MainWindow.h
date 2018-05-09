#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <wobjectdefs.h>

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow: public QMainWindow
{
    W_OBJECT(MainWindow)

public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow();

public:
    void on_quitAction_triggered();
    W_SLOT(on_quitAction_triggered)

    void on_aboutAction_triggered();
    W_SLOT(on_aboutAction_triggered)

    void on_aboutQtAction_triggered();
    W_SLOT(on_aboutQtAction_triggered)

private:
    Ui::MainWindow * ui;
};

#endif // MAINWINDOW_H
