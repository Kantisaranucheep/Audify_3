#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultiMedia>
#include <QMediaPlayer>
#include <QtWidgets>
#include <QAudioOutput>
#include <QFileDialog>
#include <QFileInfo>
#include <QPlainTextEdit>

#include "inventory.h"  // Include the header file for Inventory
#include "playlist.h"   // Include the header file for Playlist
#include "song.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);

    void on_push_play_clicked();

    void on_push_skip_clicked();


    void on_push_back_clicked();

    void on_actionOpen_Audio_File_triggered();

    void on_volume_clicked();

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_valueChanged(int value);

    void handleMediaPlayerError();

    void updateSongList(const QString &playlistName);



    void on_push_repeat_clicked();

    void on_pushaddplaylist_clicked();

    void updatePlaylistLabels();

    void onPlaylistLabelClicked(QLabel *clickedLabel);

    // void playlistlabelcliked();


    void on_pushdelplaylist_2_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    // void on_pushaddplaylist_2_clicked();

    void on_pushaddsong_clicked();

    void on_pushdelsong_clicked();



    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_song_itemClicked(QListWidgetItem *item);

    void scrollFileName();


private:
    Ui::MainWindow *ui;
    bool IS_MUTE = false;
    bool isPause = false;
    QMediaPlayer *MPlayer;
    QAudioOutput *audioOutput;
    Inventory* inventory;

    qint64 mduration;
    void updateDuration(qint64 duration);

    QPlainTextEdit* playlistTextEdit;
    QStringList playlist;
    QVBoxLayout *playlistLayout;
    QScrollArea* scrollArea;
    int currentindex=0;
    bool eventFilter(QObject *obj, QEvent *event);
    QListWidgetItem* clickedItem;

    QTimer *scrollTimer;
    QString currentFileName;




};
#endif // MAINWINDOW_H
