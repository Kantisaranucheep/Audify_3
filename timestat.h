#ifndef TIMESTAT_H
#define TIMESTAT_H

#include <QDialog>
#include <QtCharts/QBarSeries>
#include <QtCore>
#include <QtGui>

namespace Ui {
class TimeStat;
}

class TimeStat : public QDialog
{
    Q_OBJECT

public:
    explicit TimeStat(QWidget *parent = nullptr);
    ~TimeStat();

private slots:
    void on_comboduration_currentIndexChanged(int index);

    void on_combomode_currentIndexChanged(int index);

private:
    Ui::TimeStat *ui;
    void generateTopPlayCountGraph(const QJsonArray& playlistsArray);
    void generateTotalListeningGraph(const QJsonArray& totalListeningArray);
    QString getProjectRootDirectory();

};

#endif // TIMESTAT_H
