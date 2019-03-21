#include "GpxItem.h"

#include <QFile>

#include <QtDebug>

#include <fstream>
#include <memory>
#include <limits>
#include <clocale>
#include <cmath>


#include "gpx/GPX.h"
#include "gpx/Parser.h"
#include "gpx/Writer.h"
#include "gpx/ReportCerr.h"


GpxItem::GpxItem(QFileInfo const & fileInfo)
{
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
            this->name = QString::fromStdString(root->metadata().name().getValue());
            if (this->name.isEmpty() || this->name[0].isDigit()) {
                this->name = QString::fromStdString(root->metadata().desc().getValue());
            }
            this->date = QDateTime::fromString(
                QString::fromStdString(root->metadata().time().getValue()),
                Qt::ISODate
            ).date();
            this->fileInfo = fileInfo;


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

                                    this->geopath.addCoordinate(QGeoCoordinate(latitude, longitude));
                                }
                            }
                        }
                    }
                }
            }
        }

        double const averageLatitude = (maximalLatitude - minimalLatitude) / 2 + minimalLatitude;
        double const averageLongitude = (maximalLongitude - minimalLongitude) / 2 + minimalLongitude;
        this->center = QGeoCoordinate(averageLatitude, averageLongitude);

        if (times.size() >= 2) {
            QDateTime const beginning = times.first();
            QDateTime const ending = times.last();
            this->duration = QTime(0, 0).addSecs(beginning.secsTo(ending));
        }

    } else {
        qDebug() << "Can't open stream";
    }
}
