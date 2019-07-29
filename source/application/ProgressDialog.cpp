
#include <wobjectimpl.h>

#include "ProgressDialog.h"
#include "ui_ProgressDialog.h"

#include <cmath>

#include <QDateTime>

#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QLineSeries>
#include <QChart>
#include <QBarSet>
#include <QDateTimeAxis>
#include <QValueAxis>

namespace {

void setupLineChart(
            QString const & title,
            QtCharts::QChartView * const & chartView,
            QList<QPointF> const & points,
            QWidget * parent
        )
{
    QtCharts::QLineSeries * series = new QtCharts::QLineSeries(parent);
    series->append(points);

    QtCharts::QChart * chart = new QtCharts::QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle(title);

    chartView->setChart(chart);

    QtCharts::QDateTimeAxis * axisX = new QtCharts::QDateTimeAxis(parent);
    axisX->setFormat("MMM yyyy");
    chartView->chart()->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    double const maxY = points.last().y();
    QtCharts::QValueAxis * axisY = new QtCharts::QValueAxis(parent);
    axisY->setRange(0, 1000 * std::ceil(maxY / 1000));
    axisY->setLabelFormat("%.0f km");
    axisY->applyNiceNumbers();
    chartView->chart()->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void setupBarChart(
            QList<QPointF> const & points,
            QtCharts::QChartView * const & chartView
        )
{
    QStringList categories;
    QtCharts::QBarSeries * series = new QtCharts::QBarSeries();
    QtCharts::QBarSet * barSet = new QtCharts::QBarSet("Monthly");
    for (QPointF point: points) {
        QDateTime datetime = QDateTime::fromMSecsSinceEpoch(point.x());
        *barSet << point.y();
        categories << datetime.toString("yyyy-MM");
        series->append(barSet);
    }

    QtCharts::QChart * chart = new QtCharts::QChart();
    chart->addSeries(series);
    chartView->setChart(chart);

    QtCharts::QBarCategoryAxis *axisX = new QtCharts::QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
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

    setupLineChart(
        "Progress over time",
        this->ui->cumulativeDailyView,
        cumulativeDailyPoints,
        this
    );

    setupBarChart(monthlyPoints, this->ui->monthlyView);
}

ProgressDialog::~ProgressDialog()
{
    delete this->ui;
}
