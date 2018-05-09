
#include <wobjectimpl.h>

#include <QMessageBox>
#include <QCoreApplication>

#include "MainWindow.h"
#include "ui_MainWindow.h"

W_OBJECT_IMPL(MainWindow)

MainWindow::MainWindow(QWidget * parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete this->ui;
}

void MainWindow::on_quitAction_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_aboutAction_triggered()
{
    QMessageBox::about(
        this,
        QString(),
        tr("%1 %2<br>Developed by <a href=\"%3\">%4</a>")
            .arg(QCoreApplication::applicationName())
            .arg(QCoreApplication::applicationVersion())
            .arg(QCoreApplication::organizationDomain())
            .arg(QCoreApplication::organizationName())
    );
}

void MainWindow::on_aboutQtAction_triggered()
{
    QMessageBox::aboutQt(this);
}
