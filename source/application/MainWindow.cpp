
#include <wobjectimpl.h>

#include <algorithm>
#include <vector>
#include <numeric>

#include <QMessageBox>
#include <QCoreApplication>
#include <QModelIndex>
#include <QQuickView>
#include <QQmlError>
#include <QUrl>

#include <QtDebug>

#include <QSettings>

#include <QGeoCoordinate>
#include <QGeoPath>
#include <QQmlEngine>
#include <QQmlContext>

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "GpxModel.h"
#include "PathController.h"
#include "ProgressDialog.h"

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

    connect(model, SIGNAL(totalDistanceComputed(double)), this, SLOT(on_totalDistanceComputed(double)));

    this->quickView = new QQuickView();

    this->pathController = new PathController(this);
    QQmlEngine * engine = this->quickView->engine();
    engine->rootContext()->setContextProperty("pathController", this->pathController);

    QWidget *container = QWidget::createWindowContainer(quickView, this);
    quickView->setSource(QUrl("qrc:///places.qml"));

    QLayout * layout = this->ui->trackGroupBox->layout();
    if (layout == nullptr) {
        qDebug() << "NULL";
    }
    layout->addWidget(container);
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

void MainWindow::on_progressChartAction_triggered()
{
    QModelIndexList const indexes = this->ui->gpxView
        ->selectionModel()
        ->selectedRows();

    if (indexes.isEmpty()) {
        return;
    }

    QList<QPointF> points;

    for (QModelIndex index: indexes) {
        double distance = index.data(GpxModel::DistanceRole).toDouble();
        QDate date = index.data(GpxModel::DateRole).toDate();
        qint64 timestamp = QDateTime(date).toMSecsSinceEpoch();

        QPointF point(timestamp, distance);
        points.push_back(point);
    }

    std::sort(
        std::begin(points),
        std::end(points),
        [](QPointF a, QPointF b) {
            return a.x() < b.x();
        });

    QList<QPointF> cumulativePoints = std::accumulate(
        std::begin(points) + 1,
        std::end(points),
        QList<QPointF>({points.first()}),
        [](QList<QPointF> & cumulative, QPointF point) {
            double const last = cumulative.last().y();
            cumulative.append(QPointF(point.x(), point.y() + last));
            return cumulative;
        });

    ProgressDialog progressDialog(cumulativePoints, this);
    progressDialog.exec();
}

void MainWindow::on_gpxView_activated(const QModelIndex &index)
{
    if (index.isValid()) {
        QGeoCoordinate center = index.data(GpxModel::CenterRole).value<QGeoCoordinate>();
        QGeoPath geopath = index.data(GpxModel::GeoPathRole).value<QGeoPath>();
        QTime duration = index.data(GpxModel::DurationRole).toTime();
        double distance = index.data(GpxModel::DistanceRole).toDouble();
        QString description = index.data(GpxModel::DescriptionRole).toString();

        this->pathController->setGeoPath(geopath);
        this->pathController->setCenter(center);

        this->ui->distanceSpinBox->setValue(distance);
        this->ui->durationEdit->setTime(duration);
        this->ui->descriptionEdit->setText(description);
    }
}

void MainWindow::on_totalDistanceComputed(double totalDistance)
{
    statusBar()->showMessage(
        tr("Total distance: %1 km")
            .arg(totalDistance / 1000)
    );
}
