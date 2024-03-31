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
#include <QMovie>

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

    qint64 totallistening;

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

    void on_pushdelplaylist_2_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushaddsong_clicked();

    void on_pushdelsong_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_song_itemClicked(QListWidgetItem *item);

    void scrollFileName();

    void handleMediaStatusChanged(QMediaPlayer::MediaStatus status);

    void on_push_shuffle_clicked();

    QList<int> generateShuffledIndices(int count);

    void playNextSong(const Song& nextSong);

    void updateComboBox();

    void on_comboPlaylist_currentIndexChanged(int index);

    void on_comboSong_currentIndexChanged(int index);

    void updateNextSongList();

    void updatePlaylistItem(QListWidgetItem *item, const QString& playlistName, const QString& thumbnailPath);

    void saveDataToJson(const QString& filename);

    void closeEvent(QCloseEvent *event);

    void on_pushsavedata_clicked();

    void loadDataFromJson(const QString& filename);

    bool isFileLoaded(const QString& filename);

    void on_pushstat_clicked();

    QListWidgetItem* findPlaylistItem(const QString& playlistName);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_pushrename_clicked();

    void on_combodisc_currentIndexChanged(int index);

    void on_pushplaysong_clicked();

    QString getProjectRootDirectory();

    // void onPlaylistLabelClicked(QLabel *clickedLabel);

    // void playlistlabelcliked();

    // void on_pushaddplaylist_2_clicked();

    // void handleMovieResized(const QSize &size);

    // void createEmptyJsonFile(const QString& filename);

    // void saveTotallisteningToJson(const QString& filename);

    // void on_lineEdit_2_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    bool IS_MUTE = true;
    bool isPause = true;
    bool isShuffleEnabled = false;

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
    QListWidgetItem* clickedItem;

    QTimer *scrollTimer;
    QString currentFileName;

    QList<int> shuffleIndices;
    QMovie* gifMovie;

    qint64 currentSongDuration;

    QElapsedTimer elapsedTimer;
    // bool eventFilter(QObject *obj, QEvent *event);
    // qint64 totallistening;



};
#endif // MAINWINDOW_H
