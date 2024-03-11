#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "timestat.h"

#include "song.h"
#include "playlist.h"
#include "inventory.h"

#include <QStyle>
#include <QLabel>
#include <QVBoxLayout>
#include <QFont>
#include <QDebug>
#include <QTime>
#include <QMovie>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QElapsedTimer>
#include <QDate>
#include <QMainWindow>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MPlayer = new QMediaPlayer();
    audioOutput = new QAudioOutput();

    this->setFixedSize(1080,720);

    // playlistTextEdit = ui->playlistTextEdit;
    inventory = new Inventory();

    MPlayer->setAudioOutput(audioOutput);


    int iconSize = 40;

    ui->push_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->push_play->setIconSize(QSize(iconSize, iconSize));

    ui->push_skip->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->push_skip->setIconSize(QSize(iconSize, iconSize));

    ui->push_back->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->push_back->setIconSize(QSize(iconSize, iconSize));

    ui->volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->volume->setIconSize(QSize(iconSize, iconSize));

    ui->horizontalSlider_2->setMinimum(0);
    ui->horizontalSlider_2->setMaximum(100);
    // ui->horizontalSlider_2->setValue(5);

    // QString iconPath = "D:/Kant_Isaranucheep/KMITL/software engineering year1/c++/project3/Audify_3/shuffle1";
    // QPixmap pixmap(iconPath);
    // QIcon buttonIcon(pixmap);
    // ui->push_shuffle->setIcon(buttonIcon);
    // ui->push_shuffle->setIconSize(QSize(100,100));  // Set the desired icon size

    connect(MPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(MPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);

    // connect(ui->pushdelplaylist_2, &QPushButton::clicked, this, &MainWindow::on_pushdelplaylist_2_clicked);
    // connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::on_listWidget_itemClicked);
    // connect(ui->pushaddsong, &QPushButton::clicked, this, &MainWindow::on_pushaddsong_clicked);

    connect(MPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::handleMediaStatusChanged);

    connect(MPlayer, &QMediaPlayer::durationChanged, this, [&](qint64 dur) {
        qDebug() << "duration = " << dur;

        currentSongDuration = dur;
        updatePlaylistLabels();
    });

    ui->horizontalSlider->setRange(0, MPlayer->duration()/1000);
    audioOutput->setVolume(ui->horizontalSlider_2->value() / 100.0);

    // Initialize the timer for scrolling the file name
    scrollTimer = new QTimer(this);
    connect(scrollTimer, &QTimer::timeout, this, &MainWindow::scrollFileName);

    // Set the interval for scrolling (adjust as needed)
    int scrollInterval = 160;  // milliseconds
    scrollTimer->setInterval(scrollInterval);

    srand(static_cast<uint>(QTime::currentTime().msec()));

    // Create a QMovie instance and set the GIF file path
    gifMovie = new QMovie("D:/Kant_Isaranucheep/KMITL/software engineering year1/c++/project3/Audify_3/disc7.gif");

    // Set the movie to the QLabel
    ui->label_4->setMovie(gifMovie);
    gifMovie->setScaledSize(ui->label_4->size());
    gifMovie->setSpeed(200);

    gifMovie->jumpToFrame(0);
    gifMovie->stop();

    loadDataFromJson("test7.json");

    // connect(ui->comboPlaylist, QOverload<int>::of(&QComboBox::currentIndexChanged),
    //         this, &MainWindow::on_comboPlaylist_currentIndexChanged);

    for (const Playlist& playlist : inventory->getPlaylists()) {
        qDebug() << "Adding Playlist to Combo Box:" << playlist.getName();
        // ui->comboPlaylist->addItem(playlist.getName());
    }

    elapsedTimer.start();

    // ui->total_duration->setText("Duration: 0:00");
    // ui->total_song->setText("0 song");

    if (MainWindow::isFileLoaded("test7.json") == true) {
        return;
    }
    else {
        Playlist defaultPlaylist("Liked Songs");
        inventory->addPlaylist(defaultPlaylist);

        QListWidgetItem *defaultPlaylistItem = new QListWidgetItem(ui->listWidget);
        defaultPlaylistItem->setText(defaultPlaylist.getName());
        ui->listWidget->addItem(defaultPlaylistItem);

        // ui->listWidget->setCurrentItem(defaultPlaylistItem);
        clickedItem = defaultPlaylistItem;
        ui->label_Playlist_Name->setText(defaultPlaylist.getName());
        ui->label_duration2->setText("00:00");

        // ui->comboPlaylist->addItem(defaultPlaylist.getName());
        totallistening =0;

        // updatePlaylistLabels();

    }


}


MainWindow::~MainWindow()
{
    delete ui;
    delete MPlayer;
    delete audioOutput;
    delete inventory;
    delete gifMovie;
    // Optionally add more cleanup code here
}

void MainWindow::durationChanged(qint64 duration) {

    mduration = duration/1000;
    ui->horizontalSlider->setMaximum(mduration);
}


void MainWindow::positionChanged(qint64 progress) {

    if (!ui->horizontalSlider->isSliderDown()) {
        ui->horizontalSlider->setValue(progress / 1000);
    }
    updateDuration(progress / 1000);
}

void MainWindow::updateDuration(qint64 position) {
    qint64 totalSeconds = position;
    QTime currentTime((totalSeconds / 3600) % 60, (totalSeconds / 60) % 60, totalSeconds % 60, (totalSeconds*1000)%1000);
    ui->label_duration1->setText(currentTime.toString("mm:ss"));

    // qint64 totalDuration = MPlayer->duration() / 1000;
    QTime totalDurationTime((mduration / 3600) % 60, (mduration / 60) % 60, mduration% 60, (mduration*1000)%1000);
    ui->label_duration2->setText(totalDurationTime.toString("mm:ss"));
}



void MainWindow::on_actionOpen_Audio_File_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"), "", tr("MP3 Files (*.mp3)"));

    if (!fileName.isEmpty()) {
        // Set media content to the selected file
        MPlayer->setSource(QUrl::fromLocalFile(fileName));
        audioOutput->setVolume(ui->horizontalSlider_2->value());
        // Update the label with the file name
        QFileInfo fileInfo(fileName);
        ui->label_File_Name->setText(fileInfo.fileName());

        // Optional: Start playing the media when a new file is loaded
    }

    if (!inventory->getPlaylists().isEmpty()) {
        Song newSong("filename");
        inventory->getPlaylists().first().importSong(newSong);
    }
    qDebug() << "on_actionOpen_Audio_File_triggered called";
}

void MainWindow::on_push_play_clicked()
{
    if (isPause == false)
    {
        // Resume functionality
        QIcon playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
        ui->push_play->setIcon(playIcon);

        MPlayer->pause();
        gifMovie->stop();

        totallistening += elapsedTimer.elapsed();
        qDebug() << "Total duration listening: " << totallistening;

        isPause = true;
    }
    else
    {
        // Pause functionality
        QIcon pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
        ui->push_play->setIcon(pauseIcon);

        MPlayer->play();
        gifMovie->start();
        elapsedTimer.restart();

        isPause = false;
    }
}

void MainWindow::on_volume_clicked()
{
    if (IS_MUTE == false) {
        QIcon volumeIcon = style()->standardIcon(QStyle::SP_MediaVolume);
        ui->volume->setIcon(volumeIcon);
        audioOutput->setMuted(false); // Set to false to unmute
        IS_MUTE = true;
    } else {
        QIcon volumeMutedIcon = style()->standardIcon(QStyle::SP_MediaVolumeMuted);
        ui->volume->setIcon(volumeMutedIcon);
        audioOutput->setMuted(true); // Set to true to mute
        IS_MUTE = false;
    }
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    audioOutput->setVolume(value / 100.0);

    if (value == 0) {
        ui->volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    } else {
        ui->volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }    // Set volume between 0.0 and 1.0
}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    if (!ui->horizontalSlider->isSliderDown()) {
        // Slider is not being dragged, update label only
        updateDuration(value);
    } else {
        // Slider is being dragged, update MediaPlayer position
        MPlayer->setPosition(value * 1000);
        updateDuration(value);
    }
}

void MainWindow::handleMediaPlayerError() {
    qDebug() << "MediaPlayer Error: " << MPlayer->errorString();
}



void MainWindow::on_push_repeat_clicked()
{
    // Set the position of the MediaPlayer to the start
    MPlayer->setPosition(0);

    // Ensure that the MediaPlayer is playing if it was paused
    if (isPause) {
        QIcon pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
        ui->push_play->setIcon(pauseIcon);
        MPlayer->play();
        gifMovie->start();
        isPause = false;
    }

}


void MainWindow::updateComboBox()
{
    // ui->comboPlaylist->clear();  // Clear existing items

    // // Get playlist names from the inventory and add them to the combobox
    // const QList<Playlist>& allPlaylists = inventory->getPlaylists();
    // for (const Playlist& playlist : allPlaylists) {
    //     ui->comboPlaylist->addItem(playlist.getName());
    // }
}

void MainWindow::on_comboPlaylist_currentIndexChanged(int index)
{
    // // Handle the combobox selection change
    // if (index >= 0 && index < ui->comboPlaylist->count()) {
    //     QString selectedPlaylistName = ui->comboPlaylist->itemText(index);

    //     // Update UI or perform other actions based on the selected playlist name
    //     qDebug() << "Selected Playlist: " << selectedPlaylistName;

    //     // You can also update the song list or any other related UI elements
    //     updateSongList(selectedPlaylistName);
    //     updatePlaylistLabels();
    //     ui->label_Playlist_Name->setText(selectedPlaylistName);

    //     // Find the corresponding item in the listWidget
    //     for (int i = 0; i < ui->listWidget->count(); ++i) {
    //         QListWidgetItem *item = ui->listWidget->item(i);
    //         if (item && item->text() == selectedPlaylistName) {
    //             clickedItem = item;  // Set clickedItem to the selected playlist item
    //             break;
    //         }
    //     }
    // }
}

void MainWindow::on_comboSong_currentIndexChanged(int index)
{
    // // Ensure the index is valid
    // if (index >= 0 && index < ui->comboPlaylist->count()) {
    //     QString selectedPlaylistName = ui->comboPlaylist->itemText(index);

    //     qDebug() << "Selected Playlist: " << selectedPlaylistName;

    //     // Get the selected playlist
    //     const Playlist* selectedPlaylist = nullptr;
    //     const QList<Playlist>& allPlaylists = inventory->getPlaylists();

    //     for (const Playlist& playlist : allPlaylists) {
    //         if (playlist.getName() == selectedPlaylistName) {
    //             selectedPlaylist = &playlist;
    //             break;
    //         }
    //     }

    //     // Check if the selected playlist is found
    //     if (selectedPlaylist) {
    //         // Populate the song list based on the selected playlist
    //         ui->comboSong->clear();
    //         const QList<Song>& songs = selectedPlaylist->getSongs();
    //         for (const Song& song : songs) {
    //             ui->comboSong->addItem(song.getfilename());
    //         }
    //     }
    // }
}


void MainWindow::on_pushaddplaylist_clicked()
{
    bool ok;
    QString playlistName = QInputDialog::getText(this, tr("Add Playlist"),
                                                 tr("Enter playlist name:"), QLineEdit::Normal,
                                                 QString(), &ok);

    if (ok && !playlistName.isEmpty()) {

        // Check if the playlist name is longer than 23 characters
        if (playlistName.length() > 23) {
            QMessageBox::warning(this, tr("Invalid Playlist Name"),
                                 tr("Playlist name cannot be longer than 23 characters."), QMessageBox::Ok);
            return;
        }
        // Check if a playlist with the same name already exists
        if (inventory->playlistExists(playlistName)) {
            // Inform the user that the playlist name is duplicate
            QMessageBox::warning(this, tr("Duplicate Playlist"),
                                 tr("A playlist with the same name already exists."), QMessageBox::Ok);
            return;
        }
        // Create a new Playlist and add it to the Inventory
        Playlist newPlaylist(playlistName);
        inventory->addPlaylist(newPlaylist);

        qDebug() << "Playlists after adding new playlist:";
        for (const Playlist& p : inventory->getPlaylists()) {
            qDebug() << p.getName();
        }

        // QLabel *playlistlabel = new QLabel(playlistName, this);
        // playlistlabel->setStyleSheet("QLabel { color : white; font-weight: bold; margin-left: 5px;"
        //                              "border-radius: 10px; }");  // Set the text color to blue for indication
        // playlistlabel->setCursor(Qt::PointingHandCursor);

        // playlistlabel->installEventFilter(this);

        // Add the playlist label to the QListWidget
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setText(playlistName);
        ui->listWidget->addItem(item);

        // ui->comboPlaylist->addItem(playlistName);
        // ui->listWidget->setItemWidget(item, playlistlabel);

        updatePlaylistLabels();

        qDebug() << "New Playlist Name: " << playlistName;
    }

}

// bool MainWindow::eventFilter(QObject *obj, QEvent *event)
// {
//     // if (obj->isWidgetType() && event->type() == QEvent::MouseButtonPress) {
//     //     // Check if the pressed widget is a QLabel
//     //     QLabel *pressedLabel = qobject_cast<QLabel*>(obj);
//     //     if (pressedLabel) {
//     //         // Handle the click event for the QLabel (playlist label)
//     //         onPlaylistLabelClicked(pressedLabel);
//     //         return true;  // Event handled
//     //     }
//     // }

//     return QObject::eventFilter(obj, event);
// }


void MainWindow::updatePlaylistLabels()
{
    if (clickedItem) {
        QString playlistName = clickedItem->text();

        const Playlist* selectedPlaylist = nullptr;
        const QList<Playlist>& allPlaylists = inventory->getPlaylists();

        for (const Playlist& playlist : allPlaylists) {
            if (playlist.getName() == playlistName) {
                selectedPlaylist = &playlist;
                break;
            }
        }

        if (selectedPlaylist) {
            int numberOfSongs = selectedPlaylist->getSongCount();

            if (numberOfSongs == 0 || numberOfSongs == 1) {
                ui->total_song->setText(QString::number(numberOfSongs) + " song");
            } else {
                ui->total_song->setText(QString::number(numberOfSongs) + " songs");
            }

            qint64 totalDuration = 0;

            // Loop through the songs in the playlist and accumulate their durations
            for (const Song& song : selectedPlaylist->getSongs()) {
                totalDuration += song.getDuration();  // Assuming there's a function in the Song class to get the duration
            }
            qDebug() << "total duration: " << totalDuration;

            // Update the label displaying the total duration
            QTime totalDurationTime(0, 0);
            totalDurationTime = totalDurationTime.addMSecs(totalDuration);

            // ui->total_duration->setText("Duration: " + totalDurationTime.toString("mm:ss"));

            qDebug() << "Total duration: " << totalDurationTime.toString("mm:ss");
        }
    }
}


// void MainWindow::onPlaylistLabelClicked(QLabel *clickedLabel)
// {

//     // QString playlistName = clickedLabel->text();

//     // ui->label_Playlist_Name->setText(playlistName);
//     // // You can now insert a song into the clicked playlist or perform any other actions
//     // qDebug() << "Playlist label clicked: " << playlistName;

// }


void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    qDebug() << "Item count: " << ui->listWidget->count();

    for (int i = 0; i < ui->listWidget->count(); ++i) {
        qDebug() << "Item at index " << i << ": " << ui->listWidget->item(i)->text();
    }

    clickedItem = item;
    QString playlistName = item->text();
    qDebug() << "Playlist label clicked: " << playlistName;

    ui->label_Playlist_Name->setText(playlistName);

    updatePlaylistLabels();

    // updateNextSongList();

    updateSongList(playlistName);

}

void MainWindow::updateSongList(const QString &playlistName)
{
    // Get the playlist from the Inventory based on the label text
    const Playlist* selectedPlaylist = nullptr;
    const QList<Playlist>& allPlaylists = inventory->getPlaylists();  // Use const reference

    for (const Playlist& playlist : allPlaylists) {
        if (playlist.getName() == playlistName) {
            selectedPlaylist = const_cast<Playlist*>(&playlist);
            break;
        }
    }

    if (selectedPlaylist) {

        qint64 totalDuration = 0;
        // Clear the current listWidget_song
        ui->listWidget_song->clear();

        // Add songs from the selected playlist to listWidget_song
        const QList<Song>& songs = selectedPlaylist->getSongs();
        for (const Song& song : songs) {
            QString filename2 = QFileInfo(song.getfilename()).fileName();
            ui->listWidget_song->addItem(filename2);

            // totalDuration += song.getDuration();
        }

        // Update the label with the total duration
        // QTime totalDurationTime((totalDuration / 3600) % 60, (totalDuration / 60) % 60, totalDuration % 60, (totalDuration * 1000) % 1000);
        // ui->total_duration->setText(totalDurationTime.toString("mm:ss"));

        qDebug() << "Updated listWidget_song for playlist: " << playlistName;
    }

    updatePlaylistLabels();
}


void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    // Get the playlist name
    QString playlistName = item->text();

    // Get the playlist from the Inventory based on the label text
    const Playlist* selectedPlaylist = nullptr;
    const QList<Playlist>& allPlaylists = inventory->getPlaylists();

    for (const Playlist& playlist : allPlaylists) {
        if (playlist.getName() == playlistName) {
            selectedPlaylist = &playlist;
            break;
        }
    }

    if (!selectedPlaylist) {
        qDebug() << "Error: Playlist not found!";
        return;
    }

    // Ask the user to choose an image file for the playlist thumbnail
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Choose Thumbnail Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));

    if (!imagePath.isEmpty()) {
        // Set the playlist thumbnail
        const_cast<Playlist*>(selectedPlaylist)->setThumbnailPath(imagePath);

        // Update the list widget item with the new information
        updatePlaylistItem(item, playlistName, imagePath);
    }
    qDebug() << "Playlist label double clicked: " << playlistName;
}


void MainWindow::on_pushdelplaylist_2_clicked()
{

    qDebug() << "on_pushdelplaylist_2_clicked function called.";
    // Check if the clickedItem is not null
    if (clickedItem) {
        // Get the selected item from the QListWidget
        QString playlistName = clickedItem->text();
        int selectedPlaylistIndex = ui->listWidget->row(clickedItem);

        // Check if the index is 0 (default playlist)
        if (selectedPlaylistIndex == 0) {
            // Show a message box indicating that the default playlist cannot be deleted
            QMessageBox::warning(this, "Cannot Delete Default Playlist",
                                 "The default playlist cannot be deleted.");
            return;
        } else {
            // Remove the playlist from the Inventory based on the label text
            inventory->removePlaylist(selectedPlaylistIndex);

            // Optionally update other UI or perform additional actions
            updatePlaylistLabels();

            // Remove the item from the QListWidget after finishing the removal process
            delete clickedItem;
            clickedItem = nullptr;

            ui->listWidget_song->clear();
            ui->label_Playlist_Name->setText("Playlist :");
            ui->total_song->setText(QString::number(0) + " song");
            // ui->total_duration->setText("Duration: 0:00");

            qDebug() << "Playlist label deleted: " << playlistName;

            // int index = ui->comboPlaylist->findText(playlistName);

            // // If the item is found, remove it
            // if (index != -1) {
            //     ui->comboPlaylist->removeItem(index);
            // }

            // Set clickedItem to the default playlist item
            clickedItem = ui->listWidget->item(0);
            updateSongList(clickedItem->text());
            ui->label_Playlist_Name->setText(clickedItem->text());
        }
    }
}

void MainWindow::on_pushrename_clicked()
{
    // Check if the clickedItem is not null
    if (clickedItem) {
        // Get the selected item from the QListWidget
        QString playlistName = clickedItem->text();

        // Check if the clickedItem is the default playlist
        if (ui->listWidget->row(clickedItem) == 0) {
            // Show a message box indicating that the default playlist cannot be renamed
            QMessageBox::warning(this, "Cannot Rename Default Playlist",
                                 "The default playlist cannot be renamed.");
            return;
        }

        bool ok;
        QString newPlaylistName = QInputDialog::getText(this, tr("Rename Playlist"),
                                                        tr("Enter new playlist name:"), QLineEdit::Normal,
                                                        playlistName, &ok);

        if (ok && !newPlaylistName.isEmpty()) {
            // Check if the new playlist name is longer than 23 characters
            if (newPlaylistName.length() > 23) {
                QMessageBox::warning(this, tr("Invalid Playlist Name"),
                                     tr("Playlist name cannot be longer than 23 characters."), QMessageBox::Ok);
                return;
            }

            // Check if a playlist with the same name already exists
            if (inventory->playlistExists(newPlaylistName)) {
                // Inform the user that the playlist name is duplicate
                QMessageBox::warning(this, tr("Duplicate Playlist"),
                                     tr("A playlist with the same name already exists."), QMessageBox::Ok);
                return;
            }

            // Rename the playlist in the Inventory
            inventory->renamePlaylist(playlistName, newPlaylistName);

            // Update the list widget item with the new information
            updatePlaylistItem(clickedItem, newPlaylistName, "");

            // Optionally update other UI or perform additional actions
            updatePlaylistLabels();
            ui->label_Playlist_Name->setText(newPlaylistName);
            qDebug() << "Playlist label renamed: " << newPlaylistName;
        }
    }
}

void MainWindow::on_pushaddsong_clicked()
{
    qDebug() << "import clicked ";
    // Check if a playlist item is selected in the QListWidget
    if (clickedItem) {
        QString playlistName = clickedItem->text();

        // Get the playlist from the Inventory based on the label text
        const Playlist* selectedPlaylist = nullptr;
        const QList<Playlist>& allPlaylists = inventory->getPlaylists();

        for (const Playlist& playlist : allPlaylists) {
            if (playlist.getName() == playlistName) {
                selectedPlaylist = &playlist;
                break;
            }
        }

        // Ensure the selected playlist is valid
        if (selectedPlaylist) {
            // Open a file dialog to get the selected audio files
            QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Select Audio Files"), "", tr("MP3 Files (*.mp3)"));

            if (!fileNames.isEmpty()) {
                // Add the selected files to the playlist
                for (const QString& fileName : fileNames) {
                    Song newSong(fileName);
                    const_cast<Playlist*>(selectedPlaylist)->importSong(newSong);
                }

                // Update the QListWidget with the file names
                ui->listWidget_song->clear();
                const QList<Song>& songs = selectedPlaylist->getSongs();
                for (const Song& song : songs) {
                    QString filename2 = QFileInfo(song.getfilename()).fileName();
                    ui->listWidget_song->addItem(filename2);
                }

                qDebug() << "Playlist size after adding songs: " << selectedPlaylist->getSongCount();
                updateNextSongList();
            }
        } else {
            // Display a message box indicating that no playlist is selected
            QMessageBox::information(this, tr("No Playlist Selected"), tr("Please select a playlist before adding songs."));
        }
    } else {
        // Display a message box indicating that no item is clicked
        QMessageBox::information(this, tr("No Item Clicked"), tr("Please select a playlist before adding songs."));
    }

    updatePlaylistLabels();
}


void MainWindow::on_pushdelsong_clicked()
{
    // Check if a playlist item and a song item are selected in the QListWidgets
    if (clickedItem && ui->listWidget_song->currentRow() >= 0) {
        QString playlistName = clickedItem->text();
        int selectedSongIndex = ui->listWidget_song->currentRow();

        // Get the playlist from the Inventory based on the label text
        const Playlist* selectedPlaylist = nullptr;
        const QList<Playlist>& allPlaylists = inventory->getPlaylists();

        for (const Playlist& playlist : allPlaylists) {
            if (playlist.getName() == playlistName) {
                selectedPlaylist = &playlist;
                break;
            }
        }

        // Ensure the selected playlist is valid
        if (selectedPlaylist) {
            // Remove the selected song from the playlist
            const_cast<Playlist*>(selectedPlaylist)->removeSong(selectedSongIndex);

            // Update the QListWidget with the remaining file names
            ui->listWidget_song->clear();
            const QList<Song>& songs = selectedPlaylist->getSongs();
            for (const Song& song : songs) {
                QString filename = QFileInfo(song.getfilename()).fileName();
                ui->listWidget_song->addItem(filename);
            }
        }
    }
    updatePlaylistLabels();

}


void MainWindow::on_listWidget_song_itemClicked(QListWidgetItem *item)
{
    QString playlistName = ui->label_Playlist_Name->text();

    // Get the playlist from the Inventory based on the label text
    const Playlist* selectedPlaylist = nullptr;
    const QList<Playlist>& allPlaylists = inventory->getPlaylists();

    for (const Playlist& playlist : allPlaylists) {
        if (playlist.getName() == playlistName) {
            selectedPlaylist = &playlist;
            break;
        }
    }

    if (selectedPlaylist) {
        // Find the selected song in the search results based on the filename
        QString selectedSongFilename = item->text();
        const Song* selectedSong = nullptr;

        // Find the selected song in the playlist based on the filename
        for (int i = 0; i < selectedPlaylist->getSongs().size(); ++i) {
            const Song& song = selectedPlaylist->getSongs().at(i);
            if (QFileInfo(song.getfilename()).fileName() == selectedSongFilename) {
                selectedSong = &song;
                currentindex = i; // Set currentindex to the index of the selected song
                break;
            }
        }

        if (selectedSong) {

            ++selectedSong->playCount;
            qDebug() << "play count: " << selectedSong->getPlayCount();
            // Play the selected song
            QString fullFilePath = selectedSong->getfilename();
            MPlayer->setSource(QUrl::fromLocalFile(fullFilePath));
            audioOutput->setVolume(ui->horizontalSlider_2->value() / 100.0);

            QIcon pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
            ui->push_play->setIcon(pauseIcon);

            QFileInfo fileInfo(fullFilePath);
            currentFileName = fileInfo.fileName();  // Extract only the file name
            ui->label_File_Name->setText(currentFileName);
            MPlayer->play();
            gifMovie->start();

            // Start the timer for scrolling
            scrollTimer->start();

            // Update the next song list
            ui->nextsong->clear();

            int nextIndex;
            if (isShuffleEnabled) {
                nextIndex = (currentindex + 1) % shuffleIndices.size();
            } else {
                nextIndex = (currentindex + 1) % selectedPlaylist->getSongCount();
            }

            // Get the next song from the playlist using the calculated index
            const Song& nextSong = selectedPlaylist->getSongs().at(shuffleIndices.value(nextIndex, nextIndex));

            // Add the next song to listWidget_nextSong
            ui->nextsong->addItem(QFileInfo(nextSong.getfilename()).fileName());
        }
    }
}


void MainWindow::on_push_skip_clicked()
{
    QString playlistName = clickedItem->text();


    // Get the playlist from the Inventory based on the label text
    const Playlist* selectedPlaylist = nullptr;
    const QList<Playlist>& allPlaylists = inventory->getPlaylists();

    // // Check if the selected playlist is found
    // if (!selectedPlaylist) {
    //     qDebug() << "Error: Playlist not found!";
    //     return;
    // }

    for (const Playlist& playlist : allPlaylists) {
        if (playlist.getName() == playlistName) {
            selectedPlaylist = new Playlist(playlist); // Allocate a new Playlist object
            break;
        }
    }



    qDebug() << "Playlist size: " << selectedPlaylist->getSongCount();

    // Check if the playlist is not empty
    if (selectedPlaylist && selectedPlaylist->getSongCount() > 0 && ui->listWidget_song->currentRow() >= 0) {
        if (isShuffleEnabled) {
            // If shuffle is enabled, increment the shuffle index and play the corresponding song
            currentindex = ( currentindex + 1) % shuffleIndices.size();
            int shuffledIndex = shuffleIndices[ currentindex];
            const Song& nextSong = selectedPlaylist->getSongs().at(shuffledIndex);
            ++nextSong.playCount;
            qDebug() << "Current Song (Shuffled): " << nextSong.getfilename();
            updateNextSongList();
            playNextSong(nextSong);
        } else {
            // If shuffle is not enabled, increment the index and loop back to the first song if needed
            currentindex = (currentindex + 1) % selectedPlaylist->getSongCount();
            const Song& nextSong = selectedPlaylist->getSongs().at(currentindex);
            ++nextSong.playCount;
            qDebug() << "Current Song: " << nextSong.getfilename();
            updateNextSongList();
            playNextSong(nextSong);
        }
    }
}

void MainWindow::on_push_back_clicked()
{
    QString playlistName = clickedItem->text();

    // Get the playlist from the Inventory based on the label text
    const Playlist* selectedPlaylist = nullptr;
    const QList<Playlist>& allPlaylists = inventory->getPlaylists();

    for (const Playlist& playlist : allPlaylists) {
        if (playlist.getName() == playlistName) {
            selectedPlaylist = &playlist;
            break;
        }
    }

    // Check if the selected playlist is found
    if (!selectedPlaylist) {
        qDebug() << "Error: Playlist not found!";
        return;
    }

    qDebug() << "Playlist size: " << selectedPlaylist->getSongCount();

    // Check if the playlist is not empty
    if (selectedPlaylist && selectedPlaylist->getSongCount() > 0 && ui->listWidget_song->currentRow() >= 0) {
        if (isShuffleEnabled) {
            // If shuffle is enabled, decrement the shuffle index and play the corresponding song
             currentindex = ( currentindex - 1 + shuffleIndices.size()) % shuffleIndices.size();
            int shuffledIndex = shuffleIndices[ currentindex];
            const Song& nextSong = selectedPlaylist->getSongs().at(shuffledIndex);
            ++nextSong.playCount;
            qDebug() << "Current Song (Shuffled): " << nextSong.getfilename();
            updateNextSongList();
            playNextSong(nextSong);
        } else {
            // If shuffle is not enabled, decrement the index and loop back to the last song if needed
            currentindex = (currentindex - 1 + selectedPlaylist->getSongCount()) % selectedPlaylist->getSongCount();
            const Song& nextSong = selectedPlaylist->getSongs().at(currentindex);
            ++nextSong.playCount;
            qDebug() << "Current Song: " << nextSong.getfilename();
            updateNextSongList();
            playNextSong(nextSong);
        }
    }
}

void MainWindow::playNextSong(const Song& nextSong)
{
    // Set the source and start playing the next song
    MPlayer->setSource(QUrl::fromLocalFile(nextSong.getfilename()));
    audioOutput->setVolume(ui->horizontalSlider_2->value() / 100.0);
    QIcon pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
    ui->push_play->setIcon(pauseIcon);
    MPlayer->play();
    gifMovie->start();

    // Update the label with the file name
    ui->label_File_Name->setText(QFileInfo(nextSong.getfilename()).fileName());

    // Update selectedSongIndex to currentindex
    int selectedSongIndex = isShuffleEnabled ? shuffleIndices[currentindex] : currentindex;
    qDebug() << "Selected Song Index: " << selectedSongIndex;

    // Optional: Update UI or perform additional actions
}

void MainWindow::scrollFileName()
{
    if (!currentFileName.isEmpty()) {
        // Shift the displayed text to create a scrolling effect
        QString displayedText = ui->label_File_Name->text();
        displayedText = displayedText.mid(1) + displayedText.at(0);
        ui->label_File_Name->setText(displayedText);
    }
}


void MainWindow::on_push_shuffle_clicked()
{
    isShuffleEnabled = !isShuffleEnabled;

    // Change the shuffle icon text color to blue if shuffle is enabled
    if (isShuffleEnabled) {
        ui->push_shuffle->setStyleSheet("QPushButton { color: blue;"
                                        "border: none;"
                                        "border-radius:30px;"
                                        "background-color:rgb(186, 186, 186);"
                                        "font-weight: bold; "
                                        "min-width: 60px;"
                                        "max-width: 60px;"
                                        "min-height: 60px;"
                                        "padding:0px;"
                                        "max-height: 60px;}"
                                        "QPushButton::Hover{background-color:rgb(140,140,140);}");

        // If shuffle is enabled, generate shuffled indices for the playlist
         shuffleIndices = generateShuffledIndices(ui->listWidget_song->count());
        currentindex = 0;
    } else {
        ui->push_shuffle->setStyleSheet("QPushButton { color: black;"
                                        "border: none;"
                                        "border-radius:30px;"
                                        "background-color:rgb(186, 186, 186);"
                                        "font-weight: bold; "
                                        "min-width: 60px;"
                                        "max-width: 60px;"
                                        "min-height: 60px;"
                                        "padding:0px;"
                                        "max-height: 60px;}"
                                        "QPushButton::Hover{background-color:rgb(140,140,140);}"
                                        );

        // If shuffle is disabled, reset shuffled indices
         shuffleIndices.clear();
        currentindex = -1;
    }
}

void MainWindow::handleMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        // The current media has finished playing
        // Automatically play the next song based on shuffle status

        QString playlistName = clickedItem->text();

        // Get the playlist from the Inventory based on the label text
        const Playlist* selectedPlaylist = nullptr;
        const QList<Playlist>& allPlaylists = inventory->getPlaylists();

        for (const Playlist& playlist : allPlaylists) {
            if (playlist.getName() == playlistName) {
                selectedPlaylist = &playlist;
                break;
            }
        }

        qDebug() << "Playlist size: " << selectedPlaylist->getSongCount();

        // Check if the playlist is not empty
        if (selectedPlaylist && selectedPlaylist->getSongCount() > 0) {
            int nextIndex;

            if (selectedPlaylist->getSongCount() == 1) {
                MPlayer->setPosition(0);  // Restart the same song from the beginning
                MPlayer->play();
            } else {
                if (isShuffleEnabled) {
                    nextIndex = (currentindex + 1) % shuffleIndices.size();
                } else {
                    nextIndex = (currentindex + 1) % selectedPlaylist->getSongCount();
                }
            }

            // Get the next song in the playlist
            const Song& nextSong = isShuffleEnabled ? selectedPlaylist->getSongs().at(shuffleIndices.at(nextIndex))
                                                    : selectedPlaylist->getSongs().at(nextIndex);

            qDebug() << "Next Song: " << nextSong.getfilename();
            ++nextSong.playCount;

            // Set the source and start playing the next song
            MPlayer->setSource(QUrl::fromLocalFile(nextSong.getfilename()));
            audioOutput->setVolume(ui->horizontalSlider_2->value() / 100.0);
            MPlayer->play();

            // Update the label with the file name
            ui->label_File_Name->setText(QFileInfo(nextSong.getfilename()).fileName());

            // Update currentindex for future use
            currentindex = nextIndex;

            updateNextSongList();

            // Optional: Update UI or perform additional actions
        }
    }
}

QList<int> MainWindow::generateShuffledIndices(int count)
{
    QList<int> indices;
    for (int i = 0; i < count; ++i) {
        indices.append(i);
    }

    // Shuffle the indices using Fisher-Yates algorithm
    for (int i = count - 1; i > 0; --i) {
        int j = rand() % (i + 1);

        // Swap elements manually
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    return indices;
}

void MainWindow::updateNextSongList()
{
    // Clear the current listWidget_nextSong
    ui->nextsong->clear();

    QString playlistName = ui->label_Playlist_Name->text();

    // Get the playlist from the Inventory based on the label text
    const Playlist* selectedPlaylist = nullptr;
    const QList<Playlist>& allPlaylists = inventory->getPlaylists();

    for (const Playlist& playlist : allPlaylists) {
        if (playlist.getName() == playlistName) {
            selectedPlaylist = &playlist;
            break;
        }
    }

    // Check if the selected playlist is found
    if (selectedPlaylist) {
        // Get the next song index based on shuffle status
        int nextIndex;
        if (isShuffleEnabled) {
            nextIndex = (currentindex + 1) % shuffleIndices.size();
        } else {
            nextIndex = (currentindex + 1) % selectedPlaylist->getSongCount();
        }

        // Get the next song from the playlist using the calculated index
        const Song& nextSong = selectedPlaylist->getSongs().at(shuffleIndices.value(nextIndex, nextIndex));

        // Add the next song to listWidget_nextSong
        ui->nextsong->addItem(QFileInfo(nextSong.getfilename()).fileName());
    }
}


void setThumbnail(QListWidgetItem* item, const QString& thumbnailPath, int width, int height, int borderRadius)
{
    // Load the original thumbnail image
    QPixmap originalThumbnail(thumbnailPath);

    // Create a new pixmap with rounded corners
    QPixmap roundedThumbnail(originalThumbnail.size());
    roundedThumbnail.fill(Qt::transparent);

    // Use QPainter to draw the rounded image
    QPainter painter(&roundedThumbnail);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath path;
    path.addRoundedRect(0, 0, originalThumbnail.width(), originalThumbnail.height(), borderRadius, borderRadius);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, originalThumbnail);

    // Set the rounded thumbnail image as the icon on the right side
    QIcon icon(roundedThumbnail);
    item->setIcon(icon);

    // Optionally, you can also set the size of the icon
    QListWidget* listWidget = qobject_cast<QListWidget*>(item->listWidget());
    if (listWidget) {
        listWidget->setIconSize(QSize(width, height));
    }
}

void MainWindow::updatePlaylistItem(QListWidgetItem* item, const QString& newPlaylistName, const QString& thumbnailPath)
{
    // Update the item text with the new playlist name
    item->setText(newPlaylistName);

    // Get the playlist associated with the item
    const Playlist* playlist = nullptr;
    const QList<Playlist>& allPlaylists = inventory->getPlaylists();

    for (const Playlist& p : allPlaylists) {
        if (p.getName() == newPlaylistName) {
            playlist = &p;
            break;
        }
    }

    // If the playlist is found and a thumbnail path is provided, update the thumbnail
    if (playlist && !thumbnailPath.isEmpty()) {
        // Assuming you have a QLabel for displaying the thumbnail in the QListWidget
        QLabel* thumbnailLabel = qobject_cast<QLabel*>(ui->listWidget->itemWidget(item));
        QPixmap thumbnail(thumbnailPath);
        QIcon icon(thumbnail);
        item->setIcon(icon);

        setThumbnail(item, thumbnailPath, 50, 50, 20);

        if (thumbnailLabel) {
            QPixmap thumbnail(thumbnailPath);
            QIcon icon(thumbnail);
            item->setIcon(icon);

            setThumbnail(item, thumbnailPath, 50, 50, 20);
        }
    }

    // Optionally update other properties of the item

    // Refresh the QListWidget to reflect the changes
    ui->listWidget->update();
}


void MainWindow::saveDataToJson(const QString& filename)
{
    // Read existing JSON data from the file
    QJsonObject rootObject;

    QFile existingFile(filename);
    if (existingFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray existingData = existingFile.readAll();
        existingFile.close();

        QJsonDocument existingJsonDocument = QJsonDocument::fromJson(existingData);

        // Check if the existing JSON document is valid
        if (!existingJsonDocument.isNull()) {
            rootObject = existingJsonDocument.object();
        }
    }

    // Save playlists
    QJsonArray playlistsArray;
    const QList<Playlist>& allPlaylists = inventory->getPlaylists();

    for (const Playlist& playlist : allPlaylists) {
        QJsonObject playlistObject;
        playlistObject["name"] = playlist.getName();

        // Save playlist thumbnail path if available
        if (!playlist.getThumbnailPath().isEmpty()) {
            playlistObject["thumbnail"] = playlist.getThumbnailPath();
        }

        QJsonArray songsArray;
        const QList<Song>& songs = playlist.getSongs();

        for (const Song& song : songs) {
            QJsonObject songObject;
            songObject["filename"] = song.getfilename();
            songObject["playcount"] = song.getPlayCount();
            // Add more song properties as needed

            songsArray.append(songObject);
        }

        playlistObject["songs"] = songsArray;
        playlistsArray.append(playlistObject);
    }

    // Get the current date as a string
    QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");

    // Create a QJsonObject for totalListeningDuration
    QJsonObject totalListeningObject;

    // Load existing totalListeningDuration array if it exists
    if (rootObject.contains("totalListeningDuration")) {
        QJsonArray existingTotalListeningArray = rootObject["totalListeningDuration"].toArray();

        // Iterate through existing array and remove the entry for the current date if it exists
        QJsonArray updatedTotalListeningArray;
        for (const QJsonValue& dateObjectValue : existingTotalListeningArray) {
            QJsonObject dateObject = dateObjectValue.toObject();
            if (!dateObject.contains(currentDate)) {
                updatedTotalListeningArray.append(dateObject);
            }
        }

        // Add current date's total listening duration to the array
        QJsonObject currentDateObject;
        currentDateObject[currentDate] = totallistening;
        updatedTotalListeningArray.append(currentDateObject);

        // Add updated totalListeningDuration to rootObject
        rootObject["totalListeningDuration"] = updatedTotalListeningArray;
    } else {
        // Add current date's total listening duration to the array
        totalListeningObject[currentDate] = totallistening;

        // Add totalListeningDuration to rootObject
        rootObject["totalListeningDuration"] = QJsonArray{ totalListeningObject };
    }

    // Save playlists array
    rootObject["playlists"] = playlistsArray;

    // Convert JSON object to a QByteArray
    QJsonDocument jsonDocument(rootObject);
    QByteArray jsonData = jsonDocument.toJson();

    // Save JSON data to a file
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(jsonData);
        file.close();
        qDebug() << "Data saved to" << filename;
    } else {
        qDebug() << "Failed to save data to" << filename;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save data to JSON before closing the application
    saveDataToJson("test7.json");

    // Call the base class implementation
    QMainWindow::closeEvent(event);
}

void MainWindow::on_pushsavedata_clicked()
{
    // saveDataToJson("test.json");

    // Specify the full path to the directory where you want to save the JSON file
    QString directoryPath = "D:/Kant_Isaranucheep/KMITL/software engineering year1/c++/project3/Audify_3";

    // Specify the filename
    QString filename = "test7.json";

    // Combine the directory path and filename to get the full file path
    QString fullPath = QDir(directoryPath).filePath(filename);

    // Save data to JSON using the specified directory and filename
    saveDataToJson(fullPath);

}


void MainWindow::loadDataFromJson(const QString& filename)
{
    // Read JSON data from the file
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonData = file.readAll();
        file.close();

        // Parse JSON data
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);

        // Check if the JSON document is valid
        if (!jsonDocument.isNull()) {
            // Get the root object
            QJsonObject rootObject = jsonDocument.object();

            // Load date-specific total listening duration
            QJsonArray totalListeningArray = rootObject["totalListeningDuration"].toArray();
            QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");

            // Iterate through totalListeningArray to find the current date
            for (const QJsonValue& dateObjectValue : totalListeningArray) {
                QJsonObject dateObject = dateObjectValue.toObject();
                if (dateObject.contains(currentDate)) {
                    totallistening = dateObject[currentDate].toInt();
                    qDebug() << "Total Listening Duration for" << currentDate << "loaded:" << totallistening;
                    break;
                }
            }

            // Get playlists array
            QJsonArray playlistsArray = rootObject["playlists"].toArray();

            // Iterate through playlists
            for (const QJsonValue& playlistValue : playlistsArray) {
                QJsonObject playlistObject = playlistValue.toObject();

                // Get playlist name
                QString playlistName = playlistObject["name"].toString();

                // Check if the playlist already exists
                const Playlist* existingPlaylist = nullptr;
                const QList<Playlist>& allPlaylists = inventory->getPlaylists();

                QListWidgetItem* item = nullptr;

                for (const Playlist& playlist : allPlaylists) {
                    if (playlist.getName() == playlistName) {
                        existingPlaylist = &playlist;
                        // Find the corresponding item in the QListWidget
                        item = findPlaylistItem(playlistName);
                        break;
                    }
                }

                // Declare thumbnailPath outside the block
                QString thumbnailPath;

                if (playlistObject.contains("thumbnail")) {
                    thumbnailPath = playlistObject["thumbnail"].toString();
                }

                if (existingPlaylist) {
                    // Playlist already exists, update it
                    const_cast<Playlist*>(existingPlaylist)->clearSongs();  // Clear existing songs

                    // Get songs array
                    QJsonArray songsArray = playlistObject["songs"].toArray();

                    // Iterate through songs
                    for (const QJsonValue& songValue : songsArray) {
                        QJsonObject songObject = songValue.toObject();

                        // Get song properties
                        QString filename = songObject["filename"].toString();
                        int playCount = songObject["playcount"].toInt();

                        qDebug() << "Loaded Play Count for" << filename << ":" << playCount;
                        // Add more song properties as needed

                        // Create a new song and add it to the playlist
                        Song song(filename);
                        // Set more song properties as needed

                        song.setPlayCount(playCount);

                        const_cast<Playlist*>(existingPlaylist)->importSong(song);
                    }

                    // Set playlist thumbnail if available
                    if (!thumbnailPath.isEmpty()) {
                        const_cast<Playlist*>(existingPlaylist)->setThumbnailPath(thumbnailPath);
                    }

                    // Check if the current playlist is the "Liked Songs" playlist
                    if (playlistName == "Liked Songs") {
                        // Set the clicked item to the "Liked Songs" playlist item
                        clickedItem = item;
                        updateSongList(playlistName);
                        updatePlaylistLabels();
                        ui->label_Playlist_Name->setText(playlistName);
                    }

                    // updateSongList(playlistName);
                } else {
                    // Playlist doesn't exist, create a new one
                    Playlist playlist(playlistName);

                    // Get songs array
                    QJsonArray songsArray = playlistObject["songs"].toArray();

                    // Iterate through songs
                    for (const QJsonValue& songValue : songsArray) {
                        QJsonObject songObject = songValue.toObject();

                        // Get song properties
                        QString filename = songObject["filename"].toString();
                        int playCount = songObject["playcount"].toInt();
                        // Add more song properties as needed

                        qDebug() << "Loaded Play Count for" << filename << ":" << playCount;

                        // Create a new song and add it to the playlist
                        Song song(filename);
                        // Set more song properties as needed
                        song.setPlayCount(playCount);

                        playlist.importSong(song);
                    }

                    // Set playlist thumbnail if available
                    if (!thumbnailPath.isEmpty()) {
                        playlist.setThumbnailPath(thumbnailPath);
                    }


                    // Add the playlist to the inventory
                    inventory->addPlaylist(playlist);

                    // Add the playlist label to the QListWidget
                    item = new QListWidgetItem(ui->listWidget);
                    item->setText(playlistName);
                    ui->listWidget->addItem(item);

                    // ui->label_Playlist_Name->setText(playlistName);

                    // updateSongList(playlistName);

                    // Set the thumbnail for the newly added playlist
                    setThumbnail(item, thumbnailPath, 50, 50, 20);

                    // ui->comboPlaylist->addItem(playlistName);
                    qDebug() << "Added new playlist:" << playlistName;

                    // Check if the current playlist is the "Liked Songs" playlist
                    if (playlistName == "Liked Songs") {
                        // Set the clicked item to the "Liked Songs" playlist item
                        clickedItem = item;
                        updateSongList(playlistName);
                        updatePlaylistLabels();
                        ui->label_Playlist_Name->setText(playlistName);
                    }
                }
            }

            // Update playlist labels after loading data
            updatePlaylistLabels();

            qDebug() << "Data loaded from" << filename;
        } else {
            qDebug() << "Failed to parse JSON data from" << filename;
        }
    } else {
        qDebug() << "Failed to open file for reading" << filename;
    }
}



bool MainWindow::isFileLoaded(const QString& filename)
{
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonData = file.readAll();
        file.close();

        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);

        // Check if the JSON document is valid
        if (!jsonDocument.isNull()) {
            qDebug() << "File loaded successfully:" << filename;
            return true;
        } else {
            qDebug() << "Failed to parse JSON data from" << filename;
        }
    } else {
        qDebug() << "Failed to open file for reading" << filename;
    }

    return false;
}

QListWidgetItem* MainWindow::findPlaylistItem(const QString& playlistName)
{
    // Find the corresponding item in the QListWidget
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem* item = ui->listWidget->item(i);
        if (item && item->text() == playlistName) {
            return item;  // Return the found item
        }
    }
    return nullptr;  // Return nullptr if not found
}


void MainWindow::on_pushstat_clicked()
{
    TimeStat timestat;
    timestat.setModal(true);
    timestat.exec();

}


void MainWindow::on_lineEdit_textChanged(const QString& searchText)
{
    // Get the clicked playlist name
    QString playlistName = ui->label_Playlist_Name->text();

    // Get the playlist from the Inventory based on the label text
    const Playlist* selectedPlaylist = nullptr;
    const QList<Playlist>& allPlaylists = inventory->getPlaylists();

    for (const Playlist& playlist : allPlaylists) {
        if (playlist.getName() == playlistName) {
            selectedPlaylist = &playlist;
            break;
        }
    }

    // Check if the selected playlist is found
    if (selectedPlaylist) {
        // Clear the current listWidget_song
        ui->listWidget_song->clear();

        // Get the search results based on the entered text
        QList<Song> searchResults = selectedPlaylist->searchSongs(searchText);

        // Add songs from the search results to listWidget_song
        for (const Song& song : searchResults) {
            QString filename = QFileInfo(song.getfilename()).fileName();
            ui->listWidget_song->addItem(filename);
        }

        qDebug() << "Updated listWidget_song for playlist (search): " << playlistName;
    }

}

// void MainWindow::on_lineEdit_2_textChanged(const QString& searchText) {
//     // Clear the current items in listWidget along with their thumbnails
//     ui->listWidget->clear();

//     // Get the search results based on the entered text for playlists
//     QList<QString> searchResults = inventory->searchPlaylists(searchText);

//     // Add the search results to listWidget
//     for (const QString& playlistName : searchResults) {
//         ui->listWidget->addItem(playlistName);

//         // Find the corresponding playlist in the Inventory
//         const Playlist* playlist = inventory->findPlaylist(playlistName);

//         if (playlist) {
//             // Get the thumbnail path for the playlist
//             QString thumbnailPath = playlist->getThumbnailPath();

//             // Verify that the item is valid before setting the thumbnail
//             QListWidgetItem* item = ui->listWidget->item(ui->listWidget->count() - 1);
//             if (item) {
//                 setThumbnail(item, thumbnailPath, 50, 50, 20);
//             }
//         }
//     }
// }



