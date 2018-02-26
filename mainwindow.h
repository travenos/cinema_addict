#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QVector>
#include <QDesktopServices>
#include <time.h>
#include <stdlib.h>
#include <QTimer>
#include <QInputDialog>
#include <QProgressDialog>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaMetaData>

#include "rejwindow.h"
#include "dbwindow.h"
#include "qvideowidgetp.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void setDir(QDir * &dir);
    ~MainWindow();


private slots:
    void on_action_Open_triggered();

    void on_action_Exit_triggered();

    void on_action_About_triggered();

    void on_action_Qt_triggered();

    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_pushButton_Watch_clicked();

    void on_pushButton_Random_clicked();

    void on_pushButton_CheckWatched_clicked();

    void on_listWidget_itemSelectionChanged();

    void on_pushButton_Kinopoisk_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_pushButton_Rejact_clicked();

    void on_action_Refresh_triggered();

    void on_action_RemovedFilms_triggered();

    void on_action_Database_triggered();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_action_Rename_triggered();

    void on_action_Delete_triggered();

    void filmInfo(QMediaPlayer::MediaStatus status);

    void changedPos(qint64 pos);

    void on_pushButton_Play_clicked();

    void on_horizontalSlider_2_sliderReleased();

    void on_pushButton_Position_clicked();

    void on_pushButton_Random_2_clicked();

    void on_pushButton_fullScreen_clicked();

    void closeFSPreview();

    void on_action_Clear_triggered();

    void on_action_sort_triggered(bool checked);

    void on_pushButton_folder_clicked();

    void on_pushButton_Mute_clicked();

    void on_pushButton_autoShow_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    void search(QDir workdir);     //Рекурсивный метод поиска фильмов
    void countD(QDir workdir);     //Рекурсивный метод подсчёта количества вложенных папок
    void sort();                     //Сортировка списка фильмов по алфавиту

    QVector<int> films;   //Список фильмов
    QVector<filmList> database;   //База данных когда либо открытых фильмов
    QDir progDir;               //Папка со служебными файлами программы
    QFile dataFile;             //Файл с библиотекой фильмов
    QFile setFile;              //Файл с настройками программы
    QDir lastDir;               //Папка, открытая при предыдущем запуске
    QDir filmsDir;              //Папка с фильмами
    int dirCount;               //Количество вложенных папок
    int curDirNumber;           //Количество просканированных вложенных папок
    RejWindow *rw;              //Окно с фильмами, убранными из списка
    DBWindow *db;               //Окно с базой данных фильмов
    QProgressDialog *progress;  //Окно прогресса сканирования папок
    QLabel *label;              //Надпись в окне сканирования


    QMediaPlayer *player;   //Видеоплеер

    QVideoWidgetP *fw;      //Виджет для отображения видео во весь экран

    QTimer *timer;          //Таймер для автоматического предпросмотра

    //Контекстное меню для видеовиджетов
    QAction *act_play_pause;
    QAction *act_stop;
    QAction *act_fullScreenOn;
    QAction *act_fullScreenOff;
    QAction *act_mute;
};

#endif // MAINWINDOW_H
