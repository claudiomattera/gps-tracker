
#pragma once

#ifndef APPLICATION_GPX_ITEM_H
#define APPLICATION_GPX_ITEM_H

#include <QFileInfo>
#include <QDate>

class GpxItem
{
public:
    GpxItem(QFileInfo const & fileInfo);

    QString getName() const { return this->name; };
    QDate getDate() const { return this->date; };
    QFileInfo getFileInfo() const { return this->fileInfo; };

private:
    QString name;
    QDate date;
    QFileInfo fileInfo;
};

#endif // APPLICATION_GPX_ITEM_H
