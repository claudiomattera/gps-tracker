#include "GpxItem.h"

#include <QFile>
#include <QPointF>

#include <QtDebug>
#include <QSettings>

#include <fstream>
#include <memory>
#include <limits>
#include <clocale>
#include <cmath>


#include "gpx/GPX.h"
#include "gpx/Parser.h"
#include "gpx/Writer.h"
#include "gpx/ReportCerr.h"


GpxItem::GpxItem(
        QString name,
        QDate date,
        QTime duration,
        QFileInfo fileInfo,
        QGeoCoordinate center,
        QGeoPath geopath
    )
: name(name)
, date(date)
, duration(duration)
, fileInfo(fileInfo)
, center(center)
, geopath(geopath)
{
}

/*static*/
std::optional<GpxItem> GpxItem::parseGpxFile(QFileInfo const & fileInfo)
{
    QString name;
    QDate date;
    QTime duration;
    QGeoCoordinate center;
    QGeoPath geopath;

    double minimalLatitude = std::numeric_limits<double>::infinity();
    double maximalLatitude = -std::numeric_limits<double>::infinity();
    double minimalLongitude = std::numeric_limits<double>::infinity();
    double maximalLongitude = -std::numeric_limits<double>::infinity();
    QList<QDateTime> times;

    // Necessary to correctly read doubles.
    // Otherwise it might ignore the decimal point, if another character
    // is used in the current locale.
    std::setlocale(LC_ALL, "C");


    std::ifstream stream(fileInfo.absoluteFilePath().toStdString());
    if (stream.is_open()) {
        gpx::ReportCerr report;
        gpx::Parser parser(&report);
        std::unique_ptr<gpx::GPX> root(parser.parse(stream));

        if (root == nullptr) {
            qDebug() << "Parsing of " << fileInfo.absoluteFilePath() << " failed due to " << QString::fromStdString(parser.errorText()) << " on line " << parser.errorLineNumber() << " and column " << parser.errorColumnNumber();
        } else {
            name = QString::fromStdString(root->metadata().name().getValue());
            if (name.isEmpty() || name[0].isDigit()) {
                name = QString::fromStdString(root->metadata().desc().getValue());
            }
            date = QDateTime::fromString(
                QString::fromStdString(root->metadata().time().getValue()),
                Qt::ISODate
            ).date();


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

                                    double const latitude = std::stod(trkpt->lat().getValue());
                                    double const longitude = std::stod(trkpt->lon().getValue());
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
                                }
                            }
                        }
                    }
                }
            }
        }

        double const averageLatitude = (maximalLatitude - minimalLatitude) / 2 + minimalLatitude;
        double const averageLongitude = (maximalLongitude - minimalLongitude) / 2 + minimalLongitude;
        center = QGeoCoordinate(averageLatitude, averageLongitude);

        if (times.size() >= 2) {
            QDateTime const beginning = times.first();
            QDateTime const ending = times.last();
            duration = QTime(0, 0).addSecs(beginning.secsTo(ending));
        }

        return std::make_optional(
                GpxItem(
                    name,
                    date,
                    duration,
                    fileInfo,
                    center,
                    geopath
                )
            );

    } else {
        qDebug() << "Can't open stream";
        return {};
    }
}

/*static*/
std::optional<GpxItem> GpxItem::load(QFileInfo const & fileInfo, QSettings & settings)
{
    std::optional<GpxItem> item = GpxItem::loadFromCache(fileInfo, settings);
    if (item) {
        return item;
    } else {
        std::optional<GpxItem> item = GpxItem::parseGpxFile(fileInfo);
        if (item) {
            GpxItem::saveToCache(item.value(), settings);
        }
        return item;
    }
}

/*static*/
std::optional<GpxItem> GpxItem::loadFromCache(QFileInfo const & fileInfo, QSettings & settings)
{
    return {};

    QString cacheEntryKey = fileInfo.fileName();

    settings.beginGroup("cache");
    if (settings.childGroups().contains(cacheEntryKey)) {
        settings.beginGroup(cacheEntryKey);

        QString name = settings.value("name").toString();
        QDate date = settings.value("date").toDate();
        QTime duration = settings.value("duration").toTime();
        QPointF center = settings.value("center").value<QPointF>();
        QList<QPointF> points = settings.value("geopath").value<QList<QPointF>>();
        QList<QGeoCoordinate> path;
        for (QPointF point: points) {
            path.append(QGeoCoordinate(point.x(), point.y()));
        }

        settings.endGroup();
        settings.endGroup();

        return std::make_optional(
                GpxItem(
                    name,
                    date,
                    duration,
                    fileInfo,
                    QGeoCoordinate(center.x(), center.y()),
                    QGeoPath(path)
                )
            );
    } else {
        settings.endGroup();
        return {};
    }
}

/*static*/
void GpxItem::saveToCache(GpxItem const & gpxItem, QSettings & settings)
{
    return;

    QString cacheEntryKey = gpxItem.getFileInfo().fileName();

    QPointF center(gpxItem.getCenter().latitude(), gpxItem.getCenter().longitude());
    QList<QPointF> points;
    for (QGeoCoordinate coordinate: gpxItem.getGeoPath().path()) {
        points.append(QPointF(coordinate.latitude(), coordinate.longitude()));
    }

    settings.beginGroup("cache");
    settings.beginGroup(cacheEntryKey);

    settings.setValue("name", gpxItem.getName());
    settings.setValue("date", gpxItem.getDate());
    settings.setValue("duration", gpxItem.getDuration());
    settings.setValue("center", QVariant::fromValue(center));
    settings.setValue("geopath", QVariant::fromValue(points));

    settings.endGroup();
    settings.endGroup();
}
