#include "GpxModel.h"

GpxModel::GpxModel(QDir const & directory)
: directory(directory)
{
    this->files = directory.entryInfoList({"*.gpx"});
}

int GpxModel::columnCount(QModelIndex const & parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return 1;
    }
}

QVariant GpxModel::data(QModelIndex const & index, int role) const
{
    if (index.isValid()) {
        if (role == Qt::DisplayRole) {
            return this->files[index.row()].baseName();
        }
    }

    return QVariant();
}

int GpxModel::rowCount(QModelIndex const & parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return this->files.size();
    }
}
