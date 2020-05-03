#include "mainwindow.h"
#include "ui_mainwindow.h"

//Перегрузка оператора << для структуры filmList
QDataStream &operator<<(QDataStream &out, const FilmInfo &fl)
{
    out<<fl.direcroty<<fl.movie<<fl.watched<<fl.rejected<<fl.mark;
    return out;
}
//Перегрузка оператора >> для структуры filmList
QDataStream &operator>>(QDataStream &in, FilmInfo &fl)
{
    in>>fl.direcroty>>fl.movie>>fl.watched>>fl.rejected>>fl.mark;
    return in;
}

//Конструктор главной формы
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    try
    {
        srand(time(0));
#if (defined __linux__)
        m_progDir.setPath(QDir::homePath()+QDir::separator()+".cinemaSettings");
#else
#if (defined __WIN32__ || defined __WIN64__)
        m_progDir.setPath(QDir::homePath()+"/Application Data/cinemaSettings");
#endif
#endif
        if (!m_progDir.exists())
            m_progDir.mkpath(m_progDir.path());
        m_dataFile.setFileName(m_progDir.path()+QDir::separator()+"database.cin");
        if (m_dataFile.exists())
        {
            m_dataFile.open(QIODevice::ReadOnly);
            QDataStream in(&m_dataFile);
            in>>m_database;
            m_dataFile.close();
        }
        m_rejWin=new RejWindow(this,&m_database);
        m_dbWin=new DBWindow(this, &m_database);
        connect(m_rejWin,SIGNAL(changed()),this,SLOT(on_action_Refresh_triggered()));
        connect(m_dbWin,SIGNAL(changed()),this,SLOT(on_action_Refresh_triggered()));
        m_player=new QMediaPlayer;
        m_player->setVideoOutput(ui->graphicsView);
        connect(m_player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(filmInfo(QMediaPlayer::MediaStatus)));
        connect(m_player,SIGNAL(positionChanged(qint64)),this,SLOT(changedPos(qint64)));
        connect(ui->graphicsView,SIGNAL(changeFS()),this,SLOT(on_pushButton_fullScreen_clicked()));
        connect(ui->graphicsView,SIGNAL(playPause()),this,SLOT(on_pushButton_Play_clicked()));
        m_act_play_pause=new QAction(QIcon(":/new/img/icons/media-playback-start.png"),"Воспроизведение",this);
        connect(m_act_play_pause,SIGNAL(triggered()),this,SLOT(on_pushButton_Play_clicked()));
        ui->graphicsView->addAction(m_act_play_pause);
        m_act_stop=new QAction(QIcon(":/new/img/icons/media-playback-stop.png"),"Стоп",this);
        connect(m_act_stop,SIGNAL(triggered()),this,SLOT(on_listWidget_itemSelectionChanged()));
        ui->graphicsView->addAction(m_act_stop);
        m_act_mute=new QAction(QIcon(":/new/img/icons/audio-volume-muted.png"),"Выключить звук",this);
        connect(m_act_mute,SIGNAL(triggered()),this,SLOT(on_pushButton_Mute_clicked()));
        ui->graphicsView->addAction(m_act_mute);
        m_act_fullScreenOn=new QAction(QIcon(":/new/img/icons/view-fullscreen.png"),"На весь экран",this);
        connect(m_act_fullScreenOn,SIGNAL(triggered()),this,SLOT(on_pushButton_fullScreen_clicked()));
        ui->graphicsView->addAction(m_act_fullScreenOn);

        ui->listWidget->addActions(this->ui->menu_Film->actions());

        m_timer=new QTimer;
        connect(m_timer,SIGNAL(timeout()),this,SLOT(on_pushButton_Random_2_clicked()));
        m_timer->setInterval(3000);
        m_timer->stop();

        ui->listWidget->setContextMenuPolicy(Qt::NoContextMenu);
        m_lastDir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
        m_setFile.setFileName(m_progDir.path()+QDir::separator()+"settings.cin");
        if (m_setFile.exists())
        {
            m_setFile.open(QIODevice::ReadOnly);
            QDataStream in(&m_setFile);
            bool onlyWatched=true;
            bool sortEnabled=false;
            bool muted=false;
            QString lastDirPath=QDir::homePath();
            in>>lastDirPath>>onlyWatched>>sortEnabled>>muted;
            m_lastDir.setPath(lastDirPath);
            ui->checkBox->setChecked(onlyWatched);
            ui->action_sort->setChecked(sortEnabled);
            m_player->setMuted(muted);
            if (muted)
            {
                ui->pushButton_Mute->setText("Включить звук");
                ui->pushButton_Mute->setIcon(QIcon(":/new/img/icons/audio-volume-medium.png"));
                m_act_mute->setText("Включить звук");
                m_act_mute->setIcon(QIcon(":/new/img/icons/audio-volume-medium.png"));
            }
            else
            {
                ui->pushButton_Mute->setText("Выключить звук");
                ui->pushButton_Mute->setIcon(QIcon(":/new/img/icons/audio-volume-muted.png"));
                m_act_mute->setText("Выключить звук");
                m_act_mute->setIcon(QIcon(":/new/img/icons/audio-volume-muted.png"));
            }
            m_setFile.close();
        }

    }
    catch(...)
    {
        QMessageBox::warning(this,"Ошибка","Неизвестная ошибка");
        QApplication::exit(0);
    }
}

//Деструктор главной формы
MainWindow::~MainWindow()
{    
    m_player->stop();
    if (!m_progDir.exists())
        m_progDir.mkpath(m_progDir.path());
    m_dataFile.open(QIODevice::WriteOnly);
    QDataStream out(&m_dataFile);
    out<<m_database;
    m_dataFile.close();
    m_setFile.open(QIODevice::WriteOnly);
    QDataStream out2(&m_setFile);
    out2<<m_filmsDir.path()<<ui->checkBox->isChecked()<<ui->action_sort->isChecked()<<m_player->isMuted();
    m_setFile.close();
    delete m_rejWin;
    delete m_dbWin;
    delete m_player;
    delete m_act_play_pause;
    delete m_act_stop;
    delete m_act_mute;
    delete m_act_fullScreenOn;
    delete m_timer;
    delete ui;
}

//Открытие папки с фильмами
void MainWindow::on_action_Open_triggered()
{
    QString newDir=QFileDialog::getExistingDirectory(this,"Выберите папку с фильмами",m_filmsDir.path());
    if (!newDir.isEmpty())
    {
        m_filmsDir.setPath(newDir);
        on_action_Refresh_triggered();
        ui->action_Refresh->setEnabled(true);
    }
}

//Вывод списка фильмов
void MainWindow::on_action_Refresh_triggered()
{
    try
    {
        QString title;
        if (m_filmsDir==QDir::root())
            title="root – Киноман";
        else
           title=m_filmsDir.dirName()+" – Киноман";
        this->setWindowTitle(title);
        m_films.clear();
        m_player->setMedia(NULL);
        ui->listWidget->clear();
        m_dirCount=0;
        m_progressWin=new QProgressDialog(this);
        m_progressWin->setModal(true);
        m_progressWin->setWindowTitle("Идёт поиск фильмов:");
        m_progressWin->setMinimumHeight(200);
        m_progressWin->setMinimumWidth(800);
        m_label=new QLabel;
        m_label->setText("Подсчёт количества вложенных папок");
        m_label->setWordWrap(true);
        m_progressWin->setLabel(m_label);
        m_progressWin->show();
        countD(m_filmsDir);
        m_progressWin->setMinimum(0);
        m_progressWin->setMaximum(m_dirCount);
        m_curDirNumber=0;
        search(m_filmsDir);
        if(ui->action_sort->isChecked())
        {
            sort();
        }
        for (int i=0;i<=m_films.count()-1;i++)
        {
            ui->listWidget->addItem(m_database[m_films[i]].movie);
            ui->listWidget->item(i)->setIcon(QIcon(":/new/img/icons/video-x-generic.png"));
            if (m_database[m_films[i]].watched)
            {
                QColor color;
                color.setRgb(128,128,128);
                ui->listWidget->item(i)->setTextColor(color);
            }
        }
    }
    catch(int a)
    {
        switch (a) {
        case 1:
            QMessageBox::warning(this,"Ошибка","Прервано пользователем");
            break;
        case 2:
            break;
        default:
            QMessageBox::warning(this,"Ошибка","Неизвестная ошибка");
            break;
        }
        m_films.clear();
        ui->listWidget->clear();
    }
    m_progressWin->deleteLater();
    m_label->deleteLater();
    if (ui->listWidget->count()==0)
    {
        ui->listWidget->clearSelection();
        ui->groupBox->setEnabled(false);
        ui->listWidget->setContextMenuPolicy(Qt::NoContextMenu);
        ui->menu_Film->setEnabled(false);
        ui->statusBar->showMessage("Выбранная папка не содержит фильмов");

    }
    else
    {
        ui->listWidget->setCurrentRow(0);
        ui->groupBox->setEnabled(true);
        ui->statusBar->showMessage("Найдено фильмов в выбранной папке: "+QString::number(m_films.count()));
    }
}

//Рекурсивный метод подсчёта количества папок
void MainWindow::countD(const QDir& workdir)
{
    if (m_progressWin->wasCanceled())
        throw 1;
    if (++m_dirCount==1000)
        if(QMessageBox::question(this,"Слишком большая папка","Папка слишком большая. Её сканирование может занять много времени. Продолжить?")==QMessageBox::No)
            throw 2;
    QStringList subDirs;
    subDirs=workdir.entryList(QDir::Dirs|QDir::NoSymLinks);
    QDir subDir;
    qApp->processEvents();
    for (int i=0;i<=subDirs.count()-1;i++)
    {
        if (subDirs[i]!="." && subDirs[i]!=".." )
        {
            QString sep="";
            if(!workdir.path().endsWith('/'))
                sep=QDir::separator();
            subDir.setPath(workdir.path()+sep+subDirs[i]);
            countD(subDir);
        }

    }
}

//Рекурсивный метод поиска фильмов в папке
void MainWindow::search(const QDir& workdir)
{

    if (m_progressWin->wasCanceled())
        throw 2;
    m_label->setText(workdir.path());
    m_curDirNumber++;
    m_progressWin->setValue(m_curDirNumber);
    QStringList nameFilters;
    QStringList currentFilms;
    currentFilms.clear();
    nameFilters.clear();
    FilmInfo currentFilm;      //Информация о текущем фильме
    nameFilters<<"*.avi"<<"*.mkv"<<"*.mp4"<<"*.mpg"<<"*.mpeg"<<"*.ogv"<<"*.wmv";
    nameFilters<<"*.flv"<<"*.m4v"<<"*.3gp"<<"*.mov"<<"*.webm"<<"*.avf"<<"*.divx"<<"*.f4v";
    currentFilms=workdir.entryList(nameFilters);
    if (!currentFilms.isEmpty())
        for (int i=0;i<=currentFilms.count()-1;i++)
        {
            currentFilm.direcroty=workdir.path();
            currentFilm.movie=currentFilms[i];
            currentFilm.watched=false;
            currentFilm.rejected=false;
            currentFilm.mark=0;
            int j=0;
            for (j=0;j<=m_database.count()-1;j++)
            {
                if (m_database[j].movie==currentFilms[i])
                    if (m_database[j].direcroty==currentFilm.direcroty)
                        break;
            }
            if (j==m_database.count())
                m_database.push_back(currentFilm);
            if (!m_database[j].rejected)
                m_films.push_back(j);
        }
    QStringList subDirs;
    subDirs=workdir.entryList(QDir::Dirs|QDir::NoSymLinks);
    QDir subDir;
    qApp->processEvents();
    for (int i=0;i<=subDirs.count()-1;i++)
    {
        if (subDirs[i]!="." && subDirs[i]!=".." )
        {
            QString sep="";
            if(!workdir.path().endsWith('/'))
                sep=QDir::separator();
            subDir.setPath(workdir.path()+sep+subDirs[i]);
            search(subDir);
        }
    }
}


//Выход из программы
void MainWindow::on_action_Exit_triggered()
{
    QApplication::exit(0);
}

//О программе
void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this,"О программе","\"Киноман\" - программа для удобной работы со скачанными фильмами.\nДля корректной работы рекомендуется установить K-Lite Codec Pack для Windows или gstreamer-plugins-ffmpeg для Linux\nАвтор - Алексей Барашков\nВ программе испльзуются значки из темы Oxygen.");
}

//О Qt
void MainWindow::on_action_Qt_triggered()
{
    QApplication::aboutQt();
}

//Открытие фильма в медиаплеере
void MainWindow::on_pushButton_Watch_clicked()
{
    try
    {
        if (!ui->listWidget->currentIndex().isValid())
            throw 0;
        if (m_player->state()==QMediaPlayer::PlayingState)
            m_player->pause();
        ui->pushButton_Play->setText("Предпросмотр");
        ui->pushButton_Play->setIcon(QIcon(":/new/img/icons/media-playback-start.png"));
        m_act_play_pause->setText("Воспроизведение");
        m_act_play_pause->setIcon(QIcon(":/new/img/icons/media-playback-start.png"));
        int p=ui->listWidget->currentIndex().row();
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_database[m_films[p]].direcroty+QDir::separator()+m_database[m_films[p]].movie));
        if (!m_database[m_films[p]].watched)
            on_pushButton_CheckWatched_clicked();
        ui->statusBar->showMessage("Открытие фильма в медиаплеере по умолчанию...",3000);
    }
    catch(...){QMessageBox::warning(this,"Ошибка","Невозможно открыть фильм");}
}

//Двойной щелчок по фильму
void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    on_pushButton_Watch_clicked();
}

//Выбор случайного фильма
void MainWindow::on_pushButton_Random_clicked()
{
    try
    {
        if (m_films.isEmpty())
            throw 0;
        long randFilm;
        if (!ui->checkBox->isChecked())
            randFilm=rand()%m_films.count();
        else
        {
            QVector<int> unWatched;
            for(int i=0;i<=m_films.count()-1;i++)
            {
                if(!m_database[m_films[i]].watched)
                    unWatched.push_back(i);
            }
            if (unWatched.isEmpty())
                throw 0;
            randFilm=rand()%unWatched.count();
            randFilm=unWatched[randFilm];
        }
        ui->listWidget->setCurrentRow(randFilm);

    }
    catch(...){QMessageBox::warning(this,"Ошибка","Невозможно выбрать случайный фильм");}
}

//Отметить фильм как просмотренный/непросмотренный
void MainWindow::on_pushButton_CheckWatched_clicked()
{
    try
    {
        if (!ui->listWidget->currentIndex().isValid())
            throw 0;
        int p=ui->listWidget->currentIndex().row();
        m_database[m_films[p]].watched=!m_database[m_films[p]].watched;
        QColor color;
        if (m_database[m_films[p]].watched)
            color.setRgb(128,128,128);
        else
            color.setRgb(0,0,0);
        ui->listWidget->item(p)->setTextColor(color);
        ui->pushButton_CheckWatched->setText((m_database[m_films[p]].watched)?"Отметить как не просмотренный":"Отметить как просмотренный");
        ui->action_CheckWatched->setText((m_database[m_films[p]].watched)?"Отметить как не просмотренный":"Отметить как просмотренный");
        ui->pushButton_CheckWatched->setIcon(QIcon((m_database[m_films[p]].watched)?":/new/img/icons/user-online.png":":/new/img/icons/user-offline.png"));
        ui->action_CheckWatched->setIcon(QIcon((m_database[m_films[p]].watched)?":/new/img/icons/user-online.png":":/new/img/icons/user-offline.png"));
    }
    catch(...){QMessageBox::warning(this,"Ошибка","Невозможно отметить");}
}


//Вывод информации о фильме в панель сбоку
void MainWindow::on_listWidget_itemSelectionChanged()
{
    ui->pushButton_autoShow->setChecked(false);
    on_pushButton_autoShow_clicked(false);
    if (ui->listWidget->currentIndex().isValid() && ui->listWidget->count()==m_films.count())
    {
        ui->menu_Film->setEnabled(true);
        ui->listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
        ui->textBrowser->clear();
        int p=ui->listWidget->currentRow();
        QString info;
        info=m_database[m_films[p]].movie+"\nПапка: ";
        info+=m_database[m_films[p]].direcroty;

        //Размер файла
        QString fileName("");
        fileName+=m_database[m_films[p]].direcroty;
        fileName+=QDir::separator();
        fileName+=m_database[m_films[p]].movie;
        QFile film(fileName);
        double fSize=film.size();
        int va=0;
        while(fSize/1024>1)
        {
           fSize/=1024;
           va++;
        }
        info=info+"\nРазмер файла: "+QString::number(fSize,'f',2);
        switch(va)
        {
        case 0:
            info+=" Б";
            break;
        case 1:
            info+=" КиБ";
            break;
        case 2:
            info+=" МиБ";
            break;
        case 3:
            info+=" ГиБ";
            break;
        case 4:
            info+=" ТиБ";
            break;
        case 5:
            info+=" ПиБ";
            break;
        default:
            info+=" ??Б";
            break;
        }

        ui->textBrowser->setText(info);
        m_player->setMedia(QUrl::fromLocalFile(fileName));


        ui->pushButton_CheckWatched->setText((m_database[m_films[p]].watched)?"Отметить как не просмотренный":"Отметить как просмотренный");
        ui->action_CheckWatched->setText((m_database[m_films[p]].watched)?"Отметить как не просмотренный":"Отметить как просмотренный");
        ui->pushButton_CheckWatched->setIcon(QIcon((m_database[m_films[p]].watched)?":/new/img/icons/user-online.png":":/new/img/icons/user-offline.png"));
        ui->action_CheckWatched->setIcon(QIcon((m_database[m_films[p]].watched)?":/new/img/icons/user-online.png":":/new/img/icons/user-offline.png"));
        ui->pushButton_Play->setText("Предпросмотр");
        ui->pushButton_Play->setIcon(QIcon(":/new/img/icons/media-playback-start.png"));
        m_act_play_pause->setText("Воспроизведение");
        m_act_play_pause->setIcon(QIcon(":/new/img/icons/media-playback-start.png"));
        ui->horizontalSlider->setValue(m_database[m_films[p]].mark);
        ui->label_Mark->setText((m_database[m_films[p]].mark>0)?QString::number(m_database[m_films[p]].mark):"-");

    }
    else
    {
        ui->menu_Film->setEnabled(false);
        ui->listWidget->setContextMenuPolicy(Qt::NoContextMenu);
    }

}

//Поиск информации о фильме на сайте "Кинопоиск"
void MainWindow::on_pushButton_Kinopoisk_clicked()
{
    QString adress("http://www.kinopoisk.ru/index.php?first=no&what=&kp_query=");;
    int p=ui->listWidget->currentRow();
    adress+=m_database[m_films[p]].movie;
    adress.remove(adress.lastIndexOf("."),adress.length()-adress.lastIndexOf("."));
    QDesktopServices::openUrl(QUrl(adress));
    ui->statusBar->showMessage("Поиск фильма на сайте \"Кинопоиск\" по имени файла...",3000);
}

//Изменение оценки фильму
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    int p=ui->listWidget->currentRow();
    m_database[m_films[p]].mark=value;
    ui->label_Mark->setText((value>0)?QString::number(value):"-");
}

//Не отображать фильм в списке
void MainWindow::on_pushButton_Rejact_clicked()
{
    try
    {
        if (!ui->listWidget->currentIndex().isValid())
            throw 0;
        int p=ui->listWidget->currentRow();
        m_database[m_films[p]].rejected=true;
        on_action_Refresh_triggered();
        if(p<=ui->listWidget->count()-1)
        {
           ui->listWidget->setCurrentRow(p);
        }
    }
    catch(...){QMessageBox::warning(this,"Ошибка","Невозможно убрать из списка фильм");}
}

//Открытие окна редактирования убранных из списка фильмов
void MainWindow::on_action_RemovedFilms_triggered()
{
    m_rejWin->show();
    m_rejWin->raise();
}

//Открытие окна редактирования библиотеки
void MainWindow::on_action_Database_triggered()
{
    m_dbWin->show();
    m_dbWin->raise();
}

//Поиск по списку фильмов
void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    for (int i=0;i<=ui->listWidget->count()-1;i++)
    {
        if(ui->listWidget->item(i)->text().startsWith(arg1,Qt::CaseInsensitive))
        {
            ui->listWidget->setCurrentRow(i);
            break;
        }
    }
}

//Переименование фильма
void MainWindow::on_action_Rename_triggered()
{
    try
    {
        if (!ui->listWidget->currentIndex().isValid())
            throw 0;
        int p=ui->listWidget->currentRow();
        QString path;
        QString name;
        path=m_database[m_films[p]].direcroty+QDir::separator();
        bool accept=false;
        name=QInputDialog::getText(this,"Переименовать фильм","Введите новое имя",QLineEdit::Normal,m_database[m_films[p]].movie,&accept);
        path+=name;
        if(accept)
        {
            if (!QFile(m_database[m_films[p]].direcroty+QDir::separator()+m_database[m_films[p]].movie).rename(path))
                throw 0;
            m_database[m_films[p]].movie=name;
            ui->listWidget->item(p)->setText(name);
            on_listWidget_itemSelectionChanged();
        }
    }
    catch(...){QMessageBox::warning(this,"Ошибка","Невозможно переименовать фильм");}

}

//Удаление фильма с жёсткого диска
void MainWindow::on_action_Delete_triggered()
{
    try
    {
        if (!ui->listWidget->currentIndex().isValid())
            throw 0;
        if(QMessageBox::question(this,"Удалить фильм","Вы уверены, что хотите удалить фильм с жёсткого диска?")==QMessageBox::Yes)
        {
            if (!ui->listWidget->currentIndex().isValid())
                throw 0;
            int p=ui->listWidget->currentRow();
            if (!QFile(m_database[m_films[p]].direcroty+QDir::separator()+m_database[m_films[p]].movie).remove())
                throw 0;
            m_database.remove(p);
            m_films.clear();
            on_action_Refresh_triggered();
            if (ui->listWidget->count()-1>=p)
                ui->listWidget->setCurrentRow(p);
        }
    }
    catch(...){QMessageBox::warning(this,"Ошибка","Невозможно удалить фильм");}
}

//Cчитывание метаданных
void MainWindow::filmInfo(QMediaPlayer::MediaStatus status)
{
    if(status==QMediaPlayer::LoadedMedia)
    {
        QString metaInfo("");
        QString temp("");
        //Название из метаданных
        temp=m_player->metaData(QMediaMetaData::Title).toString();
        if (!temp.isEmpty())
            metaInfo=metaInfo+"\nНазвание: "+temp;
        //Жанр
        temp=m_player->metaData(QMediaMetaData::Genre).toString();
        if (!temp.isEmpty())
            metaInfo=metaInfo+"\nЖанр: "+temp;
        //Длина мультимедиа
        qint64 t=m_player->duration();
        int ms=t%1000; //Милисекунды
        t/=1000;
        QString sec("0");   //Секунды
        if((t%60+((ms>=500)?1:0))>10)
            sec=QString::number(t%60+((ms>=500)?1:0));
        else
            sec+=QString::number(t%60+((ms>=500)?1:0));
        t/=60;
        QString min("0");   //Минуты
        if(t%60>10)
            min=QString::number(t%60);
        else
            min+=QString::number(t%60);
        t/=60;      //Часы
        metaInfo=metaInfo+"\nПродолжительность: "+QString::number(t)+":"+min+":"+sec;
        //Разрешение видео
        QSize s;
        s=m_player->metaData(QMediaMetaData::Resolution).toSize();
        if(!s.isEmpty())
        {
            metaInfo=metaInfo+"\nРазрешение: "+QString::number(s.width())+"x"+QString::number(s.height());
        }
        //Название видеокодека
        temp.clear();
        temp=m_player->metaData(QMediaMetaData::VideoCodec).toString();
        if (!temp.isEmpty())
            metaInfo=metaInfo+"\nВидеокодек: "+temp;
        //Название аудиокодека
        temp.clear();
        temp=m_player->metaData(QMediaMetaData::AudioCodec).toString();
        if (!temp.isEmpty())
            metaInfo=metaInfo+"\nАудиокодек: "+temp;
        //Название контейнера
        temp.clear();
        temp=m_player->metaData("container-format").toString();
        if (!temp.isEmpty())
            metaInfo=metaInfo+"\nКонтейнер: "+temp;

        ui->textBrowser->setText(ui->textBrowser->toPlainText()+metaInfo);

        ui->graphicsView->setVisible(true);
        ui->graphicsView->repaint();

        m_player->setPosition(0);
        m_player->pause();

        ui->horizontalSlider_2->setValue(0);
        ui->horizontalSlider_2->setMaximum(m_player->duration());

        ui->pushButton_Position->setEnabled(true);
        ui->pushButton_Random_2->setEnabled(true);
        ui->pushButton_fullScreen->setEnabled(true);
        ui->timeEdit->setEnabled(true);
        ui->horizontalSlider_2->setEnabled(true);
        ui->pushButton_Play->setEnabled(true);

    }
    else
    {
        if(status==QMediaPlayer::EndOfMedia && m_player->media()!=NULL)
        {

           on_listWidget_itemSelectionChanged();
        }
        else
        {
            if(status==QMediaPlayer::InvalidMedia || m_player->media()==NULL)
            {

                ui->pushButton_Position->setEnabled(false);
                ui->pushButton_Random_2->setEnabled(false);
                ui->pushButton_fullScreen->setEnabled(false);
                ui->graphicsView->setVisible(false);
                ui->timeEdit->setEnabled(false);
                ui->horizontalSlider_2->setEnabled(false);
                if(status==QMediaPlayer::InvalidMedia)
                {
                    QString metaInfo("");
                    QString temp("");
                    //Название видеокодека
                    temp.clear();
                    temp=m_player->metaData(QMediaMetaData::VideoCodec).toString();
                    if (!temp.isEmpty())
                        metaInfo=metaInfo+"\nВидеокодек: "+temp;
                    //Название аудиокодека
                    temp.clear();
                    temp=m_player->metaData(QMediaMetaData::AudioCodec).toString();
                    if (!temp.isEmpty())
                        metaInfo=metaInfo+"\nАудиокодек: "+temp;
                    //Название контейнера
                    temp.clear();
                    temp=m_player->metaData("container-format").toString();
                    if (!temp.isEmpty())
                        metaInfo=metaInfo+"\nКонтейнер: "+temp;
                    ui->textBrowser->setText(ui->textBrowser->toPlainText()+metaInfo);
                    ui->statusBar->showMessage("Невозможно открыть предпросмотр",3000);
                }
            }
        }
    }

}

//Предпросмотр
void MainWindow::on_pushButton_Play_clicked()
{
    if (ui->pushButton_autoShow->isChecked())
        return;
    if (m_player->media()==NULL && ui->listWidget->currentIndex().isValid())
    {
        int p=ui->listWidget->currentRow();
        m_player->setMedia(QUrl::fromLocalFile(m_database[m_films[p]].direcroty+QDir::separator()+m_database[m_films[p]].movie));
        m_player->play();
        ui->pushButton_Play->setText("Остановить");
        m_act_play_pause->setText("Пауза");

        ui->pushButton_Play->setIcon(QIcon(":/new/img/icons/media-playback-pause.png"));
        m_act_play_pause->setIcon(QIcon(":/new/img/icons/media-playback-pause.png"));

        ui->pushButton_Position->setEnabled(true);
        ui->pushButton_Random_2->setEnabled(true);
        ui->pushButton_fullScreen->setEnabled(true);
        ui->graphicsView->setVisible(true);
        ui->timeEdit->setEnabled(true);
        ui->horizontalSlider_2->setEnabled(true);
        ui->pushButton_Play->setEnabled(true);
    }
    else
        if (m_player->state()==QMediaPlayer::PausedState)
        {
            m_player->play();
            ui->pushButton_Play->setText("Остановить");          
            m_act_play_pause->setText("Пауза");

            ui->pushButton_Play->setIcon(QIcon(":/new/img/icons/media-playback-pause.png"));
            m_act_play_pause->setIcon(QIcon(":/new/img/icons/media-playback-pause.png"));
        }
        else
            if (m_player->state()==QMediaPlayer::PlayingState)
            {
                m_player->pause();
                ui->pushButton_Play->setText("Предпросмотр");         
                m_act_play_pause->setText("Воспроизведение");
                ui->pushButton_Play->setIcon(QIcon(":/new/img/icons/media-playback-start.png"));
                m_act_play_pause->setIcon(QIcon(":/new/img/icons/media-playback-start.png"));
            }
            else
                QMessageBox::warning(this,"Ошибка","Невозможно воспроизвести");
}

//Отображение текущей позиции видео
void MainWindow::changedPos(qint64 pos)
{
    ui->horizontalSlider_2->setValue(pos);
    QTime time;
    int ms=pos%1000;
    pos/=1000;
    int s=pos%60;
    pos/=60;
    int min=pos%60;
    pos/=60;
    time.setHMS(pos,min,s,ms);
    ui->timeEdit->setTime(time);
}

//Изменение позиции видео со слайдером
void MainWindow::on_horizontalSlider_2_sliderReleased()
{
    m_player->setPosition(ui->horizontalSlider_2->value());
}

//Изменение позиции видео вручную
void MainWindow::on_pushButton_Position_clicked()
{
    qint64 pos;
    pos=ui->timeEdit->time().hour()*3600000+ui->timeEdit->time().minute()*60000+ui->timeEdit->time().second()*1000+ui->timeEdit->time().msec();
    if (pos<=m_player->duration())
        m_player->setPosition(pos);
    else
        QMessageBox::warning(this,"Ошибка","Позиция не найдена");
}

//Переход к случайному кадру
void MainWindow::on_pushButton_Random_2_clicked()
{
    qint64 temp=1;
    if (RAND_MAX<32400000)
      temp=32400000/RAND_MAX+1;
    if (m_player->duration() > 0)
    {
        qint64 randPos=(rand()*temp)%m_player->duration();
        m_player->setPosition(randPos);
    }
}

//Предпросмотр на весь экран
void MainWindow::on_pushButton_fullScreen_clicked()
{
    QMediaContent media = m_player->media();
    QMediaPlayer::State state = m_player->state();
    qint64 position = m_player->position();
    fullScreenVideo=new QVideoWidgetP;
    fullScreenVideo->setParent(this, Qt::Tool | Qt::CustomizeWindowHint | Qt::FramelessWindowHint );
    fullScreenVideo->setContextMenuPolicy(Qt::ActionsContextMenu);
    fullScreenVideo->addAction(m_act_play_pause);
    fullScreenVideo->addAction(m_act_mute);
    m_act_fullScreenOff=new QAction(QIcon(":/new/img/icons/view-restore.png"),"Выйти из полноэкранного режима",fullScreenVideo);
    connect(m_act_fullScreenOff,SIGNAL(triggered()),fullScreenVideo,SLOT(deleteLater()));
    fullScreenVideo->addAction(m_act_fullScreenOff);
    connect(fullScreenVideo,SIGNAL(destroyed()),this,SLOT(closeFSPreview()));
    connect(fullScreenVideo,SIGNAL(playPause()),this,SLOT(on_pushButton_Play_clicked()));
    fullScreenVideo->showFullScreen();
    m_player->setVideoOutput(fullScreenVideo);
    m_player->setMedia(media);
    m_player->pause();
    m_player->setPosition(position);
    if (state == QMediaPlayer::PlayingState)
    {
        m_player->play();
    }
}

//Выход из полноэкранного предпросмотра
void MainWindow::closeFSPreview()
{
    m_act_fullScreenOff->deleteLater();
    QMediaContent media = m_player->media();
    QMediaPlayer::State state = m_player->state();
    qint64 position = m_player->position();
    m_player->setVideoOutput(ui->graphicsView);
    m_player->setMedia(media);
    m_player->pause();
    m_player->setPosition(position);
    if (state == QMediaPlayer::PlayingState)
    {
        m_player->play();
    }
}

//Очистка библиотеки
void MainWindow::on_action_Clear_triggered()
{
    if(QMessageBox::question(this,"Очистить библиотеку","Вы уверены, что хотите полностью очистить все данные о ваших фильмах?")==QMessageBox::Yes)
    {
        m_database.clear();
        m_dataFile.remove();
        on_action_Refresh_triggered();
    }
}

//Сортировка списка фильмов
void MainWindow::sort()
{
    struct Sorter
    {
        Sorter(const QVector<FilmInfo>& database) :
            m_filmsDatabase(database)
        {}

        bool operator()(int filmId1, int filmId2) const
        {
            return m_filmsDatabase[filmId1].movie < m_filmsDatabase[filmId2].movie;
        }

    private:
        const QVector<FilmInfo>& m_filmsDatabase;
    };

    std::sort(m_films.begin(), m_films.end(), Sorter(m_database));
}

//Сортировка по требованию пользователя
void MainWindow::on_action_sort_triggered(bool checked)
{
    if(checked)
    {
        sort();
        ui->listWidget->clear();
        for (int i=0;i<=m_films.count()-1;i++)
        {
            ui->listWidget->addItem(m_database[m_films[i]].movie);
            ui->listWidget->item(i)->setIcon(QIcon(":/new/img/icons/video-x-generic.png"));
            if (m_database[m_films[i]].watched)
            {
                QColor color;
                color.setRgb(128,128,128);
                ui->listWidget->item(i)->setTextColor(color);
            }
        }
    }
}

//Открытие папки с фильмом
void MainWindow::on_pushButton_folder_clicked()
{
    try
    {
        if (!ui->listWidget->currentIndex().isValid())
            throw 0;
        int p=ui->listWidget->currentRow();
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_database[m_films[p]].direcroty));
        ui->statusBar->showMessage("Открытие папки с фильмом...",3000);
    }
    catch(...){QMessageBox::warning(this,"Ошибка","Невозможно открыть папку");}
}

//Открытие папки при запуске
void MainWindow::setDir(const QDir* dir)
{
    if(dir!=NULL)
    {
        if (dir->exists())
            m_filmsDir=*dir;
        else
        {
            QMessageBox::warning(this,"Ошибка","Неверно указано имя папки");
            m_filmsDir = m_lastDir;
        }
    }
    else
        m_filmsDir = m_lastDir;
    ui->action_Refresh->setEnabled(true);
    on_action_Refresh_triggered();
}

//Включение/выключение звука в предпросмотре
void MainWindow::on_pushButton_Mute_clicked()
{
    m_player->setMuted(!m_player->isMuted());
    if (m_player->isMuted())
    {
        ui->pushButton_Mute->setText("Включить звук");
        ui->pushButton_Mute->setIcon(QIcon(":/new/img/icons/audio-volume-medium.png"));
        m_act_mute->setText("Включить звук");
        m_act_mute->setIcon(QIcon(":/new/img/icons/audio-volume-medium.png"));
    }
    else
    {
        ui->pushButton_Mute->setText("Выключить звук");
        ui->pushButton_Mute->setIcon(QIcon(":/new/img/icons/audio-volume-muted.png"));
        m_act_mute->setText("Выключить звук");
        m_act_mute->setIcon(QIcon(":/new/img/icons/audio-volume-muted.png"));
    }
}

//Показ случайных фрагментов
void MainWindow::on_pushButton_autoShow_clicked(bool checked)
{
    if(checked)
    {
        on_pushButton_Random_2_clicked();
        m_player->play();
        m_timer->start();
    }
    else
    {
        m_player->pause();
        m_timer->stop();
    }
    ui->pushButton_Play->setEnabled(!checked);
    m_act_play_pause->setEnabled(!checked);
    ui->pushButton_Play->setText("Предпросмотр");
    m_act_play_pause->setText("Воспроизведение");
    ui->pushButton_Play->setIcon(QIcon(":/new/img/icons/media-playback-start.png"));
    m_act_play_pause->setIcon(QIcon(":/new/img/icons/media-playback-start.png"));
}
