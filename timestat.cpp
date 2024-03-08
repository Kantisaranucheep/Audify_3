#include "timestat.h"
#include "ui_timestat.h"

#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QChart>
#include <QValueAxis>

TimeStat::TimeStat(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TimeStat)
{
    ui->setupUi(this);
    this->setFixedSize(720,480);

    QBarSeries *series = new QBarSeries();
    QBarSet *set_1 = new QBarSet("duration stats");

    set_1->append(55);
    series->append(set_1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("duration stat");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList date;
    date.append("7/3/2024");
    date.append("8/3/2024");
    date.append("9/3/2024");
    date.append("10/3/2024");
    date.append("11/3/2024");

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(date);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 24);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->graphicsView->setChart(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

TimeStat::~TimeStat()
{
    delete ui;
}

