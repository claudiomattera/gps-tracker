#include "GpxModel.h"

#include <wobjectimpl.h>

#include <QtDebug>

W_OBJECT_IMPL(GpxModel)

GpxModel::GpxModel(QDir const & directory)
: directory(directory)
{
    this->headers.append("Date");
    this->headers.append("Name");
    this->headers.append("Distance");

    this->files = directory.entryInfoList({"*.gpx"});
    this->loadedFiles = 0;
}

int GpxModel::columnCount(QModelIndex const & parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return this->headers.size();
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
                case 2:
                    return tr("%1 km")
                        .arg(gpxItem.getGeoPath().length() / 1000, 0, 'f', 1);
            }
        } else if (role == FileInfoRole) {
            return gpxItem.getFileInfo().absoluteFilePath();
        } else if (role == CenterRole) {
            return QVariant::fromValue(gpxItem.getCenter());
        } else if (role == GeoPathRole) {
            return QVariant::fromValue(gpxItem.getGeoPath());
        } else if (role == DurationRole) {
            return gpxItem.getDuration();
        } else if (role == DistanceRole) {
            return gpxItem.getGeoPath().length() / 1000;
        } else if (role == DescriptionRole) {
            return gpxItem.getName();
        } else if (role == Qt::ToolTipRole) {
            return gpxItem.getFileInfo().fileName();
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

bool GpxModel::canFetchMore(const QModelIndex & parent) const
{
    Q_UNUSED(parent)

    if (this->loadedFiles < this->files.size()) {
        return true;
    } else {
        double distance = 0;
        for (GpxItem gpx: this->gpxs) {
            distance += gpx.getGeoPath().length();
        }
        emit totalDistanceComputed(distance);

        return false;
    }
}

void GpxModel::fetchMore(const QModelIndex & parent)
{
    Q_UNUSED(parent)

    int remainder = this->files.size() - this->loadedFiles;
    int filesToLoad = std::min(GpxModel::LOAD_WINDOW, remainder);

    if (filesToLoad <= 0) {
        return;
    }

    qDebug() << "Adding " << filesToLoad << " files";

    beginInsertRows(QModelIndex(), this->loadedFiles, this->loadedFiles + filesToLoad - 1);

    for (int i = this->loadedFiles; i < this->loadedFiles + filesToLoad; ++i) {
        QFileInfo file = this->files.at(i);
        GpxItem gpxItem(file);
        this->gpxs.append(gpxItem);
    }

    this->loadedFiles += filesToLoad;

    endInsertRows();

    qDebug() << "loaded " << this->loadedFiles << " files of " << this->files.size();
}
