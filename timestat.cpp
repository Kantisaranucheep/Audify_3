#include "timestat.h"
#include "ui_timestat.h"

TimeStat::TimeStat(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TimeStat)
{
    ui->setupUi(this);
}

TimeStat::~TimeStat()
{
    delete ui;
}
