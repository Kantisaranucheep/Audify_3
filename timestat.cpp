#include "timestat.h"
#include "ui_timestat.h"

#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QChart>
#include <QValueAxis>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

TimeStat::TimeStat(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TimeStat)
{
    ui->setupUi(this);
    this->setFixedSize(720, 480);

    connect(ui->comboduration, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TimeStat::on_comboduration_currentIndexChanged);

    // Read JSON data from a file
    QFile file("test7.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonData = file.readAll();
        file.close();

        // Parse JSON data
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
        // Check if the JSON document is valid
        if (!jsonDocument.isNull()) {
            // Get the root object
            QJsonObject rootObject = jsonDocument.object();

            // Get totalListeningDuration array
            QJsonArray totalListeningArray = rootObject["totalListeningDuration"].toArray();
            qDebug() << "total listening array size:" << totalListeningArray.size();

            QList<int> totallistening;
            QStringList dateLabels;

            // Iterate through the array and retrieve total listening duration for all available days
            for (const QJsonValue& dateObjectValue : totalListeningArray) {
                QJsonObject dayObject = dateObjectValue.toObject();
                QString dateString = dayObject.keys().first();
                int durationInMillis = dayObject.value(dateString).toInt();
                // Convert duration from milliseconds to decimal hours
                double durationInHours = static_cast<double>(durationInMillis) / (1000.0 * 60.0); // 1 hour = 1000 milliseconds * 60 seconds * 60 minutes
                totallistening.append(durationInHours);
                dateLabels.append(QDateTime::fromString(dateString, "yyyy-MM-dd").toString("d/M/yyyy"));
            }

            // Create a bar series and set
            QBarSeries *series = new QBarSeries();
            QBarSet *set_1 = new QBarSet("duration stats");

            // Append data to the bar set
            for (int i = 0; i < totallistening.size(); ++i) {
                set_1->append(totallistening[i]);
                qDebug() << "json duration : " << totallistening[i];
            }

            series->append(set_1);

            QChart *chart = new QChart();
            chart->addSeries(series);
            chart->setTitle("Total Listening Duration for Available Days");
            chart->setAnimationOptions(QChart::SeriesAnimations);

            QBarCategoryAxis *axisX = new QBarCategoryAxis();
            axisX->append(dateLabels);
            chart->addAxis(axisX, Qt::AlignBottom);
            axisX->setTitleText("Dates");
            series->attachAxis(axisX);

            QValueAxis *axisY = new QValueAxis();
            axisY->setRange(0, 1440);
            axisY->setTitleText("Minutes");
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);

            chart->legend()->setVisible(true);
            chart->legend()->setAlignment(Qt::AlignBottom);

            ui->graphicsView->setChart(chart);
            ui->graphicsView->setRenderHint(QPainter::Antialiasing);
        } else {
            qDebug() << "Failed to parse JSON data.";
        }
    } else {
        qDebug() << "Failed to open JSON file.";
    }
}

TimeStat::~TimeStat()
{
    delete ui;
}

void TimeStat::on_comboduration_currentIndexChanged(int index)
{
    QValueAxis *axisY = dynamic_cast<QValueAxis*>(ui->graphicsView->chart()->axisY());

    if (axisY) {
        // Adjust the Y-axis range based on the selected duration
        switch (index) {
        case 0:  // 24 hours
            axisY->setRange(0, 1440);
            break;
        case 1:  // 12 hours
            axisY->setRange(0, 720);
            break;
        case 2:  // 6 hours
            axisY->setRange(0, 360);
            break;
        case 3:  // 3 hours
            axisY->setRange(0, 180);
            break;
        default:
            break;
        }
    }
}

