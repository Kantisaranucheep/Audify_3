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

    QString projectRoot = getProjectRootDirectory();
    // Read JSON data from a file
    QFile file(projectRoot + "/Audify_3/test7.json");
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

            // Generate the graph for total listening duration
            generateTotalListeningGraph(totalListeningArray);
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
        if (ui->combomode->currentIndex() == 0) {
            // Adjust the Y-axis range based on the selected duration for total listening mode
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
        } else if (ui->combomode->currentIndex() == 1) {


            // Adjust the Y-axis range based on the selected play count for top play count mode
            switch (index) {
            case 0:  // 50 times
                axisY->setRange(0, 1000);
                break;
            case 1:  // 40 times
                axisY->setRange(0, 400);
                break;
            case 2:  // 50 times
                axisY->setRange(0, 100);
                break;
            case 3:  // 40 times
                axisY->setRange(0, 40);
                break;
            case 4:  // 30 times
                axisY->setRange(0, 28);
                break;
            case 5:  // 20 times
                axisY->setRange(0, 20);
                break;
            default:
                break;
            }
        }
    }
}

void TimeStat::generateTotalListeningGraph(const QJsonArray& totalListeningArray)
{
    QList<int> totallistening;
    QStringList dateLabels;

    QDateTime currentDate = QDateTime::currentDateTime();

    // Iterate through the array and retrieve total listening duration for all available days
    int daysCount = 0;
    int arraySize = totalListeningArray.size();
    for (int i = arraySize - 1; i >= 0; --i) {
        QJsonObject dayObject = totalListeningArray[i].toObject();
        QString dateString = dayObject.keys().first();
        int durationInMillis = dayObject.value(dateString).toInt();
        // Convert duration from milliseconds to decimal hours
        double durationInHours = static_cast<double>(durationInMillis) / (1000.0 * 60.0); // 1 hour = 1000 milliseconds * 60 seconds * 60 minutes
        totallistening.prepend(durationInHours); // Prepend to maintain reverse order
        dateLabels.prepend(QDateTime::fromString(dateString, "yyyy-MM-dd").toString("d/M/yyyy"));
        ++daysCount;
        if (daysCount >= 5) {
            break; // Stop iterating if we have collected data for the last 5 days
        }
    }

    // Create a bar series and set
    QBarSeries *series = new QBarSeries();
    QBarSet *set_1 = new QBarSet("duration stats");
    set_1->setLabelBrush(QBrush("#EEEEEE"));

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
    axisX->setTitleText("Dates");
    axisX->setLabelsColor(QColor("#EEEEEE")); // Set X-axis label color
    axisX->setTitleBrush(QBrush(QColor("#EEEEEE"))); // Set X-axis title color
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 24); // Adjust the range based on your data
    axisY->setTitleText("Minutes");
    axisY->setLabelsColor(QColor("#EEEEEE")); // Set Y-axis label color
    axisY->setTitleBrush(QBrush(QColor("#EEEEEE"))); // Set Y-axis title color
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(QColor("#EEEEEE")); // Set legend label color

    ui->graphicsView->setChart(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    // Add items to the comboduration combo box
    ui->comboduration->clear();
    ui->comboduration->addItem("24 hours");
    ui->comboduration->addItem("12 hours");
    ui->comboduration->addItem("6 hours");
    ui->comboduration->addItem("3 hours");

    chart->setBackgroundBrush(QBrush(QColor("#222831")));
    chart->setTitleBrush(QBrush(QColor("#EEEEEE"))); // Set chart title color
}


void TimeStat::generateTopPlayCountGraph(const QJsonArray& playlistsArray)
{
    QBarSeries *series = new QBarSeries();

    // Create a map to store play counts for each song
    QMap<QString, int> playCountMap;

    // Iterate through playlists
    for (const QJsonValue& playlistValue : playlistsArray) {
        QJsonObject playlistObject = playlistValue.toObject();

        // Get songs array
        QJsonArray songsArray = playlistObject["songs"].toArray();

        // Iterate through songs
        for (const QJsonValue& songValue : songsArray) {
            QJsonObject songObject = songValue.toObject();
            QString filename = QFileInfo(songObject["filename"].toString()).fileName(); // Extract only the file name
            int playCount = songObject["playcount"].toInt();

            // Update play count in the map
            playCountMap[filename] += playCount;
        }
    }

    // Sort the map by play count in descending order
    QList<QPair<QString, int>> sortedPlayCounts;
    for (auto it = playCountMap.begin(); it != playCountMap.end(); ++it) {
        sortedPlayCounts.append(qMakePair(it.key(), it.value()));
    }
    std::sort(sortedPlayCounts.begin(), sortedPlayCounts.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // Add the top 5 songs to the bar series
    int count = 0;
    for (const auto& pair : sortedPlayCounts) {
        if (count >= 5) {
            break;
        }

        QBarSet *set = new QBarSet(pair.first);
        set->append(pair.second);
        series->append(set);
        set->setLabelColor(QColor("#EEEEEE"));  // Change label color to #EEEEEE

        ++count;
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Top 5 Played Songs");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->setTitleText("Songs");
    axisX->setTitleBrush(QBrush(QColor("#EEEEEE"))); // Change X-axis title color
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 20); // Adjust the range based on your data
    axisY->setTitleText("Play Count");
    axisY->setTitleBrush(QBrush(QColor("#EEEEEE"))); // Change Y-axis title color
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->graphicsView->setChart(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    ui->comboduration->clear();
    ui->comboduration->addItem("1000 times");
    ui->comboduration->addItem("400 times");
    ui->comboduration->addItem("100 times");
    ui->comboduration->addItem("40 times");
    ui->comboduration->addItem("28 times");
    ui->comboduration->addItem("20 times");

    chart->setBackgroundBrush(QBrush(QColor("#222831")));
    axisX->setTitleBrush(QBrush(QColor("#EEEEEE")));
    axisY->setTitleBrush(QBrush(QColor("#EEEEEE")));

    chart->legend()->setLabelColor(QColor("#EEEEEE"));  // Change legend text color
    chart->setTitleBrush(QBrush(QColor("#EEEEEE")));

    axisX->setLabelsColor(QColor("#EEEEEE"));  // Change axis label color
    axisY->setLabelsColor(QColor("#EEEEEE"));

    // Iterate over each bar set and set label color
    for (QBarSet *barSet : series->barSets()) {
        barSet->setLabelColor(QColor("#EEEEEE"));
    }
}

void TimeStat::on_combomode_currentIndexChanged(int index)
{
    QString projectRoot = getProjectRootDirectory();
    if (index == 0) {
        // Read JSON data from a file
        QFile file(projectRoot + "/Audify_3/test7.json");
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

                // Generate the graph for total listening duration
                generateTotalListeningGraph(totalListeningArray);
            } else {
                qDebug() << "Failed to parse JSON data.";
            }
        } else {
            qDebug() << "Failed to open JSON file.";
        }
    } else if (index == 1) {
        // Assuming index 1 represents the top play count mode
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

                // Get playlists array
                QJsonArray playlistsArray = rootObject["playlists"].toArray();

                // Generate the graph for top play count songs
                generateTopPlayCountGraph(playlistsArray);
            } else {
                qDebug() << "Failed to parse JSON data.";
            }
        } else {
            qDebug() << "Failed to open JSON file.";
        }
    }
}

QString TimeStat::getProjectRootDirectory() {
    QString executablePath = QCoreApplication::applicationDirPath();
    QFileInfo executableInfo(executablePath);
    QString projectRoot = executableInfo.absolutePath();
    QString normalizedProjectRoot = QDir::toNativeSeparators(projectRoot);
    qDebug() << "Project Root Directory:" << normalizedProjectRoot;
    return normalizedProjectRoot;
}

