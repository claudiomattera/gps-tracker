#ifndef APPLICATION_PROGRESSDIALOG_H
#define APPLICATION_PROGRESSDIALOG_H

#include <wobjectdefs.h>

#include <QDialog>

#include <QList>
#include <QPointF>

namespace Ui {
class ProgressDialog;
}

namespace QtCharts {
    class QLineSeries;
    class QChart;
}

class ProgressDialog: public QDialog
{
    W_OBJECT(ProgressDialog)

public:
    explicit ProgressDialog(QList<QPointF> const & points, QWidget * parent = nullptr);
    ~ProgressDialog() override;

private:
    Ui::ProgressDialog * ui;

    QtCharts::QLineSeries * series;
    QtCharts::QChart * chart;
};

#endif // APPLICATION_PROGRESSDIALOG_H
