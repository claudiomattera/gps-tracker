
#include <wobjectimpl.h>

#include "ProgressDialog.h"
#include "ui_ProgressDialog.h"

#include <cmath>

#include <QLineSeries>
#include <QChart>
#include <QDateTimeAxis>
#include <QValueAxis>

namespace {

void setupChart(
            QString const & title,
            QtCharts::QLineSeries * & series,
            QtCharts::QChart * & chart,
            QtCharts::QChartView * const & chartView,
            QList<QPointF> const & points,
            QWidget * parent
        ) {
    series = new QtCharts::QLineSeries(parent);
    series->append(points);

    chart = new QtCharts::QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle(title);

    chartView->setChart(chart);

    QtCharts::QDateTimeAxis * axisX = new QtCharts::QDateTimeAxis(parent);
    axisX->setFormat("MMM yyyy");
    chartView->chart()->setAxisX(axisX, series);

    double const maxY = points.last().y();
    QtCharts::QValueAxis * axisY = new QtCharts::QValueAxis(parent);
    axisY->setRange(0, 1000 * std::ceil(maxY / 1000));
    axisY->setLabelFormat("%.0f km");
    axisY->applyNiceNumbers();
    chartView->chart()->setAxisY(axisY, series);
}

} // unnamed namespace

W_OBJECT_IMPL(ProgressDialog)

ProgressDialog::ProgressDialog(
            QList<QPointF> const & cumulativeDailyPoints,
            QList<QPointF> const & monthlyPoints,
            QWidget * parent
        )
: QDialog(parent)
, ui(new Ui::ProgressDialog)
{
    this->ui->setupUi(this);

    setupChart(
        "Progress over time",
        this->cumulativeDailySeries,
        this->cumulativeDailyChart,
        this->ui->cumulativeDailyView,
        cumulativeDailyPoints,
        this
    );

    setupChart(
        "Monthly aggregate",
        this->monthlySeries,
        this->monthlyChart,
        this->ui->monthlyView,
        monthlyPoints,
        this
    );
}

ProgressDialog::~ProgressDialog()
{
    delete this->ui;
}
