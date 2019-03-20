
#pragma once

#ifndef APPLICATION_GPX_MODEL_H
#define APPLICATION_GPX_MODEL_H

#include <QAbstractListModel>

#include <QModelIndex>
#include <QDir>
#include <QList>
#include "GpxItem.h"

class GpxModel: public QAbstractListModel
{
public:
    GpxModel(QDir const & directory);

    virtual int columnCount(QModelIndex const & parent = QModelIndex()) const;
    virtual QVariant data(QModelIndex const & index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual int rowCount(QModelIndex const & parent = QModelIndex()) const;

private:
    QDir directory;
    QList<GpxItem> gpxs;
    QList<QString> headers;
};

#endif // APPLICATION_GPX_MODEL_H
