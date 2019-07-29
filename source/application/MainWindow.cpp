
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

namespace {

bool sameDayPredicate(QDateTime const & first, QDateTime const & second) {
    return first.date() == second.date();
}

bool sameWeekPredicate(QDateTime const & first, QDateTime const & second) {
    return
        first.date().year() == second.date().year()
        && first.date().weekNumber() == second.date().weekNumber();
}

bool sameMonthPredicate(QDateTime const & first, QDateTime const & second) {
    return
        first.date().year() == second.date().year()
        && first.date().month() == second.date().month();
}

QList<QPointF> groupByPredicate(
            QList<QPointF> const & original,
            std::function<bool(QDateTime const&, QDateTime const&)> predicate
        ) {
    return std::accumulate(
        std::begin(original),
        std::end(original),
        QList<QPointF>({original.first()}),
        [&predicate](QList<QPointF> & grouped, QPointF point) {
            QDateTime const lastDatetime = QDateTime::fromMSecsSinceEpoch(grouped.last().x());
            QDateTime const nextDatetime = QDateTime::fromMSecsSinceEpoch(point.x());
            if (predicate(lastDatetime, nextDatetime)) {
                grouped.last().ry() += point.y();
            } else {
                grouped.append(point);
            }
            return grouped;
        });
}

QList<QPointF> cumulate(QList<QPointF> const & original) {
    return std::accumulate(
        std::begin(original) + 1,
        std::end(original),
        QList<QPointF>({original.first()}),
        [](QList<QPointF> & cumulative, QPointF point) {
            double const last = cumulative.last().y();
            cumulative.append(QPointF(point.x(), point.y() + last));
            return cumulative;
        });
}

} // unnamed namespace

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
    quickView->setSource(QUrl("qrc:///MapWidget.qml"));

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

    QList<QPointF> dailyPoints = groupByPredicate(points, sameDayPredicate);
    QList<QPointF> monthlyPoints = groupByPredicate(points, sameMonthPredicate);

    QList<QPointF> cumulativeDailyPoints = cumulate(dailyPoints);

    ProgressDialog progressDialog(cumulativeDailyPoints, monthlyPoints, this);
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
