
#include <wobjectimpl.h>

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

#include <fstream>
#include <memory>
#include <iostream>
#include <list>
#include <limits>
#include <clocale>
#include <cmath>

#include "gpx/GPX.h"
#include "gpx/Parser.h"
#include "gpx/Writer.h"
#include "gpx/ReportCerr.h"


#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "GpxModel.h"
#include "PathController.h"
#include "distance.h"

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

void MainWindow::on_gpxView_activated(const QModelIndex &index)
{
    if (index.isValid()) {
        QString filePath = index.data(Qt::UserRole).toString();

        double minimalLatitude = std::numeric_limits<double>::infinity();
        double maximalLatitude = -std::numeric_limits<double>::infinity();
        double minimalLongitude = std::numeric_limits<double>::infinity();
        double maximalLongitude = -std::numeric_limits<double>::infinity();
        double distance = 0;
        double previousLatitude = std::numeric_limits<double>::quiet_NaN();
        double previousLongitude = std::numeric_limits<double>::quiet_NaN();
        QGeoPath geopath;
        QList<QDateTime> times;

        // Necessary to correctly read doubles.
        // Otherwise it might ignore the decimal point, if another character
        // is used in the current locale.
        std::setlocale(LC_ALL, "C");

        std::ifstream stream(filePath.toStdString());
        if (stream.is_open()) {
            gpx::ReportCerr report;
            gpx::Parser parser(&report);
            std::unique_ptr<gpx::GPX> root(parser.parse(stream));

            if (root == nullptr) {
                qDebug() << "Parsing of " << filePath << " failed due to " << QString::fromStdString(parser.errorText()) << " on line " << parser.errorLineNumber() << " and column " << parser.errorColumnNumber();
            } else {
                std::list<gpx::TRK*> &trks = root->trks().list();

                for (std::list<gpx::TRK*>::iterator node = trks.begin(); node != trks.end(); ++node) {
                    gpx::TRK *trk = (*node);

                    if (trk != nullptr) {

                        std::list<gpx::TRKSeg*> &trksegs = trk->trksegs().list();

                        for (std::list<gpx::TRKSeg*>::iterator iter2 = trksegs.begin(); iter2 != trksegs.end(); ++iter2) {
                            gpx::TRKSeg *seg = (*iter2);

                            if (seg != nullptr) {
                                std::list<gpx::WPT*> &trkpts = seg->trkpts().list();

                                for (std::list<gpx::WPT*>::iterator iter3 = trkpts.begin(); iter3 != trkpts.end(); ++iter3) {
                                    gpx::WPT *trkpt = (*iter3);

                                    if (trkpt != nullptr) {

                                        double latitude = std::stod(trkpt->lat().getValue());
                                        double longitude = std::stod(trkpt->lon().getValue());
                                        times.append(
                                            QDateTime::fromString(
                                                QString::fromStdString(trkpt->time().getValue()),
                                                Qt::ISODate
                                            )
                                        );
                                        // double elevation = std::stod(trkpt->ele().getValue());

                                        maximalLatitude = std::max(latitude, maximalLatitude);
                                        maximalLongitude = std::max(longitude, maximalLongitude);

                                        minimalLatitude = std::min(latitude, minimalLatitude);
                                        minimalLongitude = std::min(longitude, minimalLongitude);

                                        geopath.addCoordinate(QGeoCoordinate(latitude, longitude));

                                        if (!std::isnan(previousLatitude)) {
                                            distance += compute_distance(latitude, longitude, previousLatitude, previousLongitude);
                                        }
                                        previousLatitude = latitude;
                                        previousLongitude = longitude;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            qDebug() << "Can't open stream";
        }

        double averageLatitude = (maximalLatitude - minimalLatitude) / 2 + minimalLatitude;
        double averageLongitude = (maximalLongitude - minimalLongitude) / 2 + minimalLongitude;
        QGeoCoordinate center = QGeoCoordinate(averageLatitude, averageLongitude);

        QDateTime beginning = times.first();
        QDateTime ending = times.last();
        QTime duration = QTime(0, 0).addSecs(beginning.secsTo(ending));

        this->pathController->setGeoPath(geopath);
        this->pathController->setCenter(center);

        this->ui->distanceSpinBox->setValue(distance);
        this->ui->durationEdit->setTime(duration);
    }
}
