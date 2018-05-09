
#include <wobjectimpl.h>

#include <QMessageBox>
#include <QCoreApplication>

#include <QSettings>

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "GpxModel.h"

W_OBJECT_IMPL(MainWindow)

MainWindow::MainWindow(QWidget * parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);

    QSettings settings;
    QDir directory(settings.value("directory").toString());
    QAbstractItemModel * model = new GpxModel(directory);
    this->ui->gpxView->setModel(model);
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
