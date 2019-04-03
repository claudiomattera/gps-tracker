
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
    QGeoCoordinate getCenter() const;
    QGeoPath getGeoPath() const;
    QTime getDuration() const { return this->duration; }
    double getDistance() const { return this->distance; }

private:
    GpxItem(
        QString name,
        QDate date,
        QTime duration,
        QFileInfo fileInfo,
        std::optional<QGeoCoordinate> center,
        std::optional<QGeoPath> geopath,
        double distance
    );

    static std::optional<GpxItem> parseGpxFile(QFileInfo const & fileInfo);
    static std::optional<GpxItem> loadFromCache(QFileInfo const & fileInfo, QSettings & settings);
    static void saveToCache(GpxItem const & gpxItem, QSettings & settings);

    void loadPath() const;

private:
    QString name;
    QDate date;
    QTime duration;
    QFileInfo fileInfo;
    double distance;

    mutable std::optional<QGeoCoordinate> center;
    mutable std::optional<QGeoPath> geopath;
};

#endif // APPLICATION_GPX_ITEM_H
