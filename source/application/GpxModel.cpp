#include "GpxModel.h"

#include <QtDebug>

GpxModel::GpxModel(QDir const & directory)
: directory(directory)
{
    this->headers.append("Date");
    this->headers.append("Name");
    int max = 10;
    int i = 0;
    QFileInfoList files = directory.entryInfoList({"*.gpx"});
    for (QFileInfo file: files) {
        if (i > max) {
            break;
        }
        i += 1;
        GpxItem gpxItem(file);

        this->gpxs.append(gpxItem);
    }
}

int GpxModel::columnCount(QModelIndex const & parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return 2;
    }
}

QVariant GpxModel::data(QModelIndex const & index, int role) const
{
    if (index.isValid()) {
        GpxItem gpxItem = this->gpxs[index.row()];
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case 0:
                    return gpxItem.getDate();
                case 1:
                    return gpxItem.getName();
            }
        } else if (role == Qt::UserRole) {
            return gpxItem.getFileInfo().absoluteFilePath();
        }
    }

    return QVariant();
}

QVariant GpxModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (section < 0 || section >= this->headers.size()) {
        return QVariant();
    }

    return this->headers.at(section);
}

int GpxModel::rowCount(QModelIndex const & parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return this->gpxs.size();
    }
}
