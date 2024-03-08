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

private:
    Ui::TimeStat *ui;

};

#endif // TIMESTAT_H
