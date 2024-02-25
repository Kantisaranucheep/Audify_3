#include "mainwindow.h"
#include "./ui_mainwindow.h"

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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MPlayer = new QMediaPlayer();
    audioOutput = new QAudioOutput();

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

    connect(MPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(MPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);



    connect(ui->pushdelplaylist_2, &QPushButton::clicked, this, &MainWindow::on_pushdelplaylist_2_clicked);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::on_listWidget_itemClicked);
    // connect(ui->pushaddsong, &QPushButton::clicked, this, &MainWindow::on_pushaddsong_clicked);

    connect(MPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::handleMediaStatusChanged);

    connect(MPlayer, &QMediaPlayer::durationChanged, this, [&](qint64 dur) {
        qDebug() << "duration = " << dur;

        currentSongDuration = dur;
        updatePlaylistLabels();
    });

    connect(ui->comboPlaylist, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_comboPlaylist_currentIndexChanged);

    // updateComboBox();

    // disconnect(ui->listWidget, &QListWidget::itemClicked, 0, 0);

    // // Connect the listWidget itemClicked signal to the on_listWidget_itemClicked slot
    // connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::on_listWidget_itemClicked);

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
    gifMovie = new QMovie("D:/Kant_Isaranucheep/KMITL/software engineering year1/c++/project3/Audify_3/disc2.gif");

    // Set the movie to the QLabel
    ui->label_4->setMovie(gifMovie);
    gifMovie->setScaledSize(ui->label_4->size());
    gifMovie->setSpeed(20);

    gifMovie->jumpToFrame(0);
    gifMovie->stop();

    ui->total_duration->setText("Duration: 0:00");
    ui->total_song->setText("0 songs");

    Playlist defaultPlaylist("All Songs");
    inventory->addPlaylist(defaultPlaylist);

    QListWidgetItem *defaultPlaylistItem = new QListWidgetItem(ui->listWidget);
    defaultPlaylistItem->setText(defaultPlaylist.getName());
    ui->listWidget->addItem(defaultPlaylistItem);

    // ui->listWidget->setCurrentItem(defaultPlaylistItem);
    clickedItem = defaultPlaylistItem;
    ui->label_Playlist_Name->setText(defaultPlaylist.getName());
    ui->label_duration2->setText("00:00");

    ui->comboPlaylist->addItem(defaultPlaylist.getName());


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

        isPause = true;
    }
    else
    {
        // Pause functionality
        QIcon pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
        ui->push_play->setIcon(pauseIcon);

        MPlayer->play();
        gifMovie->start();

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
    // Handle the combobox selection change
    if (index >= 0 && index < ui->comboPlaylist->count()) {
        QString selectedPlaylistName = ui->comboPlaylist->itemText(index);

        // Update UI or perform other actions based on the selected playlist name
        qDebug() << "Selected Playlist: " << selectedPlaylistName;

        // You can also update the song list or any other related UI elements
        updateSongList(selectedPlaylistName);
        updatePlaylistLabels();
        ui->label_Playlist_Name->setText(selectedPlaylistName);

        // Find the corresponding item in the listWidget
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            if (item && item->text() == selectedPlaylistName) {
                clickedItem = item;  // Set clickedItem to the selected playlist item
                break;
            }
        }
    }
}

void MainWindow::on_comboSong_currentIndexChanged(int index)
{
    // Ensure the index is valid
    if (index >= 0 && index < ui->comboPlaylist->count()) {
        QString selectedPlaylistName = ui->comboPlaylist->itemText(index);

        qDebug() << "Selected Playlist: " << selectedPlaylistName;

        // Get the selected playlist
        const Playlist* selectedPlaylist = nullptr;
        const QList<Playlist>& allPlaylists = inventory->getPlaylists();

        for (const Playlist& playlist : allPlaylists) {
            if (playlist.getName() == selectedPlaylistName) {
                selectedPlaylist = &playlist;
                break;
            }
        }

        // Check if the selected playlist is found
        if (selectedPlaylist) {
            // Populate the song list based on the selected playlist
            ui->comboSong->clear();
            const QList<Song>& songs = selectedPlaylist->getSongs();
            for (const Song& song : songs) {
                ui->comboSong->addItem(song.getfilename());
            }
        }
    }
}


void MainWindow::on_pushaddplaylist_clicked()
{
    bool ok;
    QString playlistName = QInputDialog::getText(this, tr("Add Playlist"),
                                                 tr("Enter playlist name:"), QLineEdit::Normal,
                                                 QString(), &ok);

    if (ok && !playlistName.isEmpty()) {
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

        ui->comboPlaylist->addItem(playlistName);
        // ui->listWidget->setItemWidget(item, playlistlabel);

        // updatePlaylistLabels();

        qDebug() << "New Playlist Name: " << playlistName;
    }

}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // if (obj->isWidgetType() && event->type() == QEvent::MouseButtonPress) {
    //     // Check if the pressed widget is a QLabel
    //     QLabel *pressedLabel = qobject_cast<QLabel*>(obj);
    //     if (pressedLabel) {
    //         // Handle the click event for the QLabel (playlist label)
    //         onPlaylistLabelClicked(pressedLabel);
    //         return true;  // Event handled
    //     }
    // }

    return QObject::eventFilter(obj, event);
}


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

            // Update the label displaying the number of songs
            ui->total_song->setText(QString::number(numberOfSongs) + " songs");

            // // Update the label displaying the total duration
            // qint64 totalDuration = selectedPlaylist->getTotalDuration();  // Use the function from Playlist class

            // QTime totalDurationTime((totalDuration / 3600) % 60, (totalDuration / 60) % 60, totalDuration % 60, (totalDuration * 1000) % 1000);
            // ui->total_duration->setText("Duration: " + totalDurationTime.toString("mm:ss"));

            // qDebug() << "Total duration: " << totalDurationTime.toString("mm:ss");
        }
    }
}

void MainWindow::onPlaylistLabelClicked(QLabel *clickedLabel)
{

    // QString playlistName = clickedLabel->text();

    // ui->label_Playlist_Name->setText(playlistName);
    // // You can now insert a song into the clicked playlist or perform any other actions
    // qDebug() << "Playlist label clicked: " << playlistName;

}


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

    updateSongList(playlistName);

    updatePlaylistLabels();

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
    QString playlistName = item->text();
    qDebug() << "Playlist label double clicked: " << playlistName;

}


void MainWindow::on_pushdelplaylist_2_clicked()
{
    // Check if the clickedItem is not null
    if (clickedItem) {
        // Get the selected item from the QListWidget
        QString playlistName = clickedItem->text();
        int selectedPlaylistIndex = ui->listWidget->row(clickedItem);

        // Remove the playlist from the Inventory based on the label text
        inventory->removePlaylist(selectedPlaylistIndex);

        // Remove the item from the QListWidget
        delete clickedItem;// Set it to null after deletion to avoid accessing it again

        qDebug() << "Playlist label deleted: " << playlistName;

        clickedItem = nullptr;

        int index = ui->comboPlaylist->findText(playlistName);

        // If the item is found, remove it
        if (index != -1) {
            ui->comboPlaylist->removeItem(index);
        }

        // Optionally update other UI or perform additional actions
        updatePlaylistLabels();
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
            // Open a file dialog to get the selected audio file
            QString fileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"), "", tr("MP3 Files (*.mp3)"));

            if (!fileName.isEmpty()) {
                // Add the selected file to the playlist
                Song newSong(fileName);
                const_cast<Playlist*>(selectedPlaylist)->importSong(newSong);

                // Update the QListWidget with the file names
                ui->listWidget_song->clear();
                const QList<Song>& songs = selectedPlaylist->getSongs();
                for (const Song& song : songs) {
                    QString filename2 = QFileInfo(song.getfilename()).fileName();
                    ui->listWidget_song->addItem(filename2);
                }

                qDebug() << "Playlist size after adding a song: " << selectedPlaylist->getSongCount();
            }
        } else {
            // Display a message box indicating that no playlist is selected
            QMessageBox::information(this, tr("No Playlist Selected"), tr("Please select a playlist before adding a song."));
        }
    } else {
        // Display a message box indicating that no item is clicked
        QMessageBox::information(this, tr("No Item Clicked"), tr("Please select a playlist before adding a song."));
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
        int selectedSongIndex = ui->listWidget_song->currentRow();

        // Ensure the selected song index is within the playlist size
        if (selectedSongIndex >= 0 && selectedSongIndex < selectedPlaylist->getSongs().size()) {
            const Song& selectedSong = selectedPlaylist->getSongs().at(selectedSongIndex);

            QString fullFilePath = selectedSong.getfilename();
            MPlayer->setSource(QUrl::fromLocalFile(fullFilePath));
            audioOutput->setVolume(ui->horizontalSlider_2->value()/100.0);

            QIcon pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
            ui->push_play->setIcon(pauseIcon);

            QFileInfo fileInfo(fullFilePath);
            currentFileName = fileInfo.fileName();  // Extract only the file name
            ui->label_File_Name->setText(currentFileName);
            MPlayer->play();
            gifMovie->start();

            // Start the timer for scrolling
            scrollTimer->start();
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
            selectedPlaylist = &playlist;
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
            qDebug() << "Current Song (Shuffled): " << nextSong.getfilename();
            playNextSong(nextSong);
        } else {
            // If shuffle is not enabled, increment the index and loop back to the first song if needed
            currentindex = (currentindex + 1) % selectedPlaylist->getSongCount();
            const Song& nextSong = selectedPlaylist->getSongs().at(currentindex);
            qDebug() << "Current Song: " << nextSong.getfilename();
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
            qDebug() << "Current Song (Shuffled): " << nextSong.getfilename();
            playNextSong(nextSong);
        } else {
            // If shuffle is not enabled, decrement the index and loop back to the last song if needed
            currentindex = (currentindex - 1 + selectedPlaylist->getSongCount()) % selectedPlaylist->getSongCount();
            const Song& nextSong = selectedPlaylist->getSongs().at(currentindex);
            qDebug() << "Current Song: " << nextSong.getfilename();
            playNextSong(nextSong);
        }
    }
}

void MainWindow::playNextSong(const Song& nextSong)
{
    // Set the source and start playing the next song
    MPlayer->setSource(QUrl::fromLocalFile(nextSong.getfilename()));
    audioOutput->setVolume(ui->horizontalSlider_2->value() / 100.0);
    MPlayer->play();

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

            if(selectedPlaylist->getSongCount() == 1){
                MPlayer->setPosition(0);  // Restart the same song from the beginning
                MPlayer->play();
            }else {
                if (isShuffleEnabled) {
                    // Shuffle is enabled, randomize the next song
                    shuffleIndices= generateShuffledIndices(selectedPlaylist->getSongCount());
                    currentindex = shuffleIndices.at((currentindex + 1) % selectedPlaylist->getSongCount());
                } else {
                    // Shuffle is disabled, play the next song in order
                    currentindex = (currentindex + 1) % selectedPlaylist->getSongCount();
                }
            }

            // Get the next song in the playlist
            const Song& nextSong = selectedPlaylist->getSongs().at(currentindex);

            qDebug() << "Next Song: " << nextSong.getfilename();

            // Set the source and start playing the next song
            MPlayer->setSource(QUrl::fromLocalFile(nextSong.getfilename()));
            audioOutput->setVolume(ui->horizontalSlider_2->value()/100.0);
            MPlayer->play();

            // Update the label with the file name
            ui->label_File_Name->setText(QFileInfo(nextSong.getfilename()).fileName());

            // Update selectedSongIndex to currentindex
            int selectedSongIndex = currentindex;
            qDebug() << "Selected Song Index: " << selectedSongIndex;

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


