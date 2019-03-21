
#include <wobjectimpl.h>

#include "ProgressDialog.h"
#include "ui_ProgressDialog.h"

#include <cmath>

#include <QLineSeries>
#include <QChart>
#include <QDateTimeAxis>
#include <QValueAxis>

W_OBJECT_IMPL(ProgressDialog)

ProgressDialog::ProgressDialog(QList<QPointF> const & points, QWidget * parent)
: QDialog(parent)
, ui(new Ui::ProgressDialog)
{
    this->ui->setupUi(this);

    this->series = new QtCharts::QLineSeries(this);
    this->series->append(points);

    this->chart = new QtCharts::QChart();
    chart->legend()->hide();
    chart->addSeries(this->series);
    chart->setTitle("Progress over time");

    this->ui->chartView->setChart(this->chart);

    QtCharts::QDateTimeAxis * axisX = new QtCharts::QDateTimeAxis(this);
    axisX->setFormat("MMM yyyy");
    this->ui->chartView->chart()->setAxisX(axisX, this->series);

    double const maxY = points.last().y();
    QtCharts::QValueAxis * axisY = new QtCharts::QValueAxis(this);
    axisY->setRange(0, 1000 * std::ceil(maxY / 1000));
    axisY->setLabelFormat("%.0f km");
    axisY->applyNiceNumbers();
    this->ui->chartView->chart()->setAxisY(axisY, this->series);
}

ProgressDialog::~ProgressDialog()
{
    delete this->ui;
}
