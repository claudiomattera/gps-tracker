#include "GpxModel.h"

#include <QtDebug>

GpxModel::GpxModel(QDir const & directory)
: directory(directory)
{
    this->headers.append("Date");
    this->headers.append("Name");

    this->files = directory.entryInfoList({"*.gpx"});
    this->loadedFiles = 0;
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

bool GpxModel::canFetchMore(const QModelIndex & parent) const
{
    return this->loadedFiles < this->files.size();
}

void GpxModel::fetchMore(const QModelIndex & parent)
{
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
