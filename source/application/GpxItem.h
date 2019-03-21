
#pragma once

#ifndef APPLICATION_GPX_ITEM_H
#define APPLICATION_GPX_ITEM_H

#include <QFileInfo>
#include <QGeoPath>
#include <QDate>
#include <QTime>

#include <optional>

class QSettings;

class GpxItem
{
public:
    static std::optional<GpxItem> load(QFileInfo const & fileInfo, QSettings & settings);

    QString getName() const { return this->name; }
    QDate getDate() const { return this->date; }
    QFileInfo getFileInfo() const { return this->fileInfo; }
    QGeoCoordinate getCenter() const { return this->center; }
    QGeoPath getGeoPath() const { return this->geopath; }
    QTime getDuration() const { return this->duration; }

private:
    GpxItem(
        QString name,
        QDate date,
        QTime duration,
        QFileInfo fileInfo,
        QGeoCoordinate center,
        QGeoPath geopath
    );

    static std::optional<GpxItem> parseGpxFile(QFileInfo const & fileInfo);
    static std::optional<GpxItem> loadFromCache(QFileInfo const & fileInfo, QSettings & settings);
    static void saveToCache(GpxItem const & gpxItem, QSettings & settings);

private:
    QString name;
    QDate date;
    QTime duration;
    QFileInfo fileInfo;
    QGeoCoordinate center;
    QGeoPath geopath;
};

#endif // APPLICATION_GPX_ITEM_H
