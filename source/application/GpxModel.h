
#pragma once

#ifndef APPLICATION_GPX_MODEL_H
#define APPLICATION_GPX_MODEL_H

#include <wobjectdefs.h>

#include <QAbstractListModel>

#include <QModelIndex>
#include <QDir>
#include <QList>
#include <QFileInfo>
#include "GpxItem.h"

class GpxModel: public QAbstractListModel
{
    W_OBJECT(GpxModel)

    int const LOAD_WINDOW = 10000;

public:
    enum ItemDataRole
    {
        FileInfoRole = Qt::UserRole,
        CenterRole = Qt::UserRole + 1,
        GeoPathRole = Qt::UserRole + 2,
        DurationRole = Qt::UserRole + 3,
        DescriptionRole = Qt::UserRole + 4,
        DistanceRole = Qt::UserRole + 5,
        DateRole = Qt::UserRole + 6
    };

public:
    GpxModel(QDir const & directory);

    virtual int columnCount(QModelIndex const & parent = QModelIndex()) const;
    virtual QVariant data(QModelIndex const & index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual int rowCount(QModelIndex const & parent = QModelIndex()) const;

    void totalDistanceComputed(double totalDistance) const
    W_SIGNAL(totalDistanceComputed, totalDistance);

protected:
    bool canFetchMore(const QModelIndex & parent) const;
    void fetchMore(const QModelIndex & parent);

private:
    QFileInfoList files;
    int loadedFiles;
    QDir directory;
    QList<GpxItem> gpxs;
    QList<QString> headers;
};

#endif // APPLICATION_GPX_MODEL_H
