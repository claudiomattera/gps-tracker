
#pragma once

#ifndef APPLICATION_GPX_ITEM_H
#define APPLICATION_GPX_ITEM_H

#include <QFileInfo>
#include <QGeoPath>
#include <QDate>
#include <QTime>

class GpxItem
{
public:
    GpxItem(QFileInfo const & fileInfo);

    QString getName() const { return this->name; }
    QDate getDate() const { return this->date; }
    QFileInfo getFileInfo() const { return this->fileInfo; }
    QGeoCoordinate getCenter() const { return this->center; }
    QGeoPath getGeoPath() const { return this->geopath; }
    QTime getDuration() const { return this->duration; }

private:
    QString name;
    QDate date;
    QTime duration;
    QFileInfo fileInfo;
    QGeoCoordinate center;
    QGeoPath geopath;
};

#endif // APPLICATION_GPX_ITEM_H
