
#pragma once

#ifndef GPX_MODEL_H
#define GPX_MODEL_H

#include <QAbstractListModel>

#include <QModelIndex>
#include <QDir>
#include <QFileInfoList>

class GpxModel: public QAbstractListModel
{
public:
    GpxModel(QDir const & directory);

    virtual int columnCount(QModelIndex const & parent = QModelIndex()) const;
    virtual QVariant data(QModelIndex const & index, int role = Qt::DisplayRole) const;
    virtual int rowCount(QModelIndex const & parent = QModelIndex()) const;

private:
    QDir directory;
    QFileInfoList files;
};

#endif // GPX_MODEL_H
