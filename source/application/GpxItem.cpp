#include "GpxItem.h"

#include <QFile>

#include <QtDebug>

#include <fstream>
#include <memory>

#include "gpx/GPX.h"
#include "gpx/Parser.h"
#include "gpx/Writer.h"
#include "gpx/ReportCerr.h"

GpxItem::GpxItem(QFileInfo const & fileInfo)
{
    std::ifstream stream(fileInfo.absoluteFilePath().toStdString());
    if (stream.is_open()) {
        gpx::ReportCerr report;
        gpx::Parser parser(&report);
        std::unique_ptr<gpx::GPX> root(parser.parse(stream));

        if (root == nullptr) {
            qDebug() << "Parsing of " << fileInfo.absolutePath() << " failed due to " << QString::fromStdString(parser.errorText()) << " on line " << parser.errorLineNumber() << " and column " << parser.errorColumnNumber();
        } else {
            this->name = QString::fromStdString(root->metadata().name().getValue());
            // this->date = QDateTime::fromString(
            //     QString::fromStdString(root->metadata().name().getValue()),
            //     Qt::ISODate
            // ).date();

            this->date = QDateTime::currentDateTime().date();

            this->fileInfo = fileInfo;
        }
    } else {
        qDebug() << "Can't open stream";
    }
}
