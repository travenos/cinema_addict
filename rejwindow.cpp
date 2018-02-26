#include "rejwindow.h"
#include "ui_rejwindow.h"

RejWindow::RejWindow(QWidget *parent, QVector<filmList> *db) :
    QDialog(parent),
    ui(new Ui::RejWindow)
{
    database=db;
    ui->setupUi(this);
    makemenu();
}

RejWindow::~RejWindow()
{
    act_rename->deleteLater();
    act_remove->deleteLater();
    act_checkWatched->deleteLater();
    act_checkRejected->deleteLater();
    delete ui;
}

void RejWindow::showEvent(QShowEvent *event)
{
    ui->listWidget->clear();
    films.clear();
    for(int i=0;i<=database->count()-1;i++)
        if(database->operator [](i).rejected)
        {
            ui->listWidget->addItem(database->operator [](i).movie);
            films.push_back(i);
        }
}

void RejWindow::on_pushButton_Restore_clicked()
{
    try
    {
        if(!ui->listWidget->currentIndex().isValid())
            throw 0;
        for (int i=0;i<=ui->listWidget->count()-1;i++)
        {
            if (ui->listWidget->item(i)->isSelected())
            {
                filmList f=database->operator [](films[i]);
                f.rejected=false;
                database->replace(films[i],f);
            }
        }
        showEvent(NULL);
        emit changed();
    }
    catch(...){QMessageBox::warning(this,"Ошибка","Невозможно вернуть в список фильм");}
}

void RejWindow::on_pushButton_Close_clicked()
{
    this->close();
}

//Создание контекстного меню
void RejWindow::makemenu()
{
    act_rename=new QAction(QIcon(":/new/img/icons/edit-rename.png"),"Переименовать",this);
    ui->listWidget->addAction(act_rename);
    connect(act_rename,SIGNAL(triggered()),this,SLOT(rename_trigered()));
    act_remove=new QAction(QIcon(":/new/img/icons/edit-delete.png"),"Удалить",this);
    ui->listWidget->addAction(act_remove);
    connect(act_remove,SIGNAL(triggered()),this,SLOT(remove_trigered()));
    act_checkWatched=new QAction(QIcon(":/new/img/icons/user-offline.png"),"Отметить как просмотренный/непросмотренный",this);
    ui->listWidget->addAction(act_checkWatched);
    connect(act_checkWatched,SIGNAL(triggered()),this,SLOT(checkWatched_trigered()));
    act_checkRejected=new QAction(QIcon(":/new/img/icons/task-reject.png"),"Отображать/не отображать в списке фильмов",this);
    ui->listWidget->addAction(act_checkRejected);
    connect(act_checkRejected,SIGNAL(triggered()),this,SLOT(checkRejected_trigered()));
}

//Нажатие на кнопку "Переименовать"
void RejWindow::rename_trigered()
{
    QString path;
    QString name;
    bool accept;
    for (int i=0;i<=ui->listWidget->count()-1;i++)
    {
        try
        {
            if (ui->listWidget->item(i)->isSelected())
            {
                path=database->operator [](films[i]).direcroty+QDir::separator();
                accept=false;
                name=QInputDialog::getText(this,"Переименовать фильм","Введите новое имя",QLineEdit::Normal,database->operator [](films[i]).movie,&accept);
                path+=name;
                if(accept)
                {
                    if (!QFile(database->operator [](films[i]).direcroty+QDir::separator()+database->operator [](films[i]).movie).rename(path))
                        throw 0;
                    filmList temp=database->operator [](films[i]);
                    temp.movie=name;
                    database->replace(films[i],temp);
                    ui->listWidget->item(i)->setText(name);
                }
            }
        }
        catch(...){QMessageBox::warning(this,"Ошибка","Невозможно переименовать фильм");}
    }
    emit changed();
}

//Нажатие на кнопку "Удалить"
void RejWindow::remove_trigered()
{
    for (int i=0;i<=films.count()-1;i++)
    {
        try
        {
            if (ui->listWidget->item(i)->isSelected())
            {
                if(QMessageBox::question(this,"Удалить фильм","Вы уверены, что хотите удалить фильм "+database->operator [](films[i]).movie+" с жёсткого диска?")==QMessageBox::Yes)
                {
                    if (!QFile(database->operator [](films[i]).direcroty+QDir::separator()+database->operator [](films[i]).movie).remove())
                        throw 0;
                    database->remove(films[i]);
                    for (int j=i+1;j<=films.count()-1;j++)
                        if (films[j]>films[i])
                            films[j]--;
                }
            }
        }
        catch(...){QMessageBox::warning(this,"Ошибка","Невозможно удалить фильм");}
    }
    emit changed();
    showEvent(NULL);
}

//Пометить как просмотренный/непросмотренный
void RejWindow::checkWatched_trigered()
{
    for (int i=0;i<=films.count()-1;i++)
    {
        if (ui->listWidget->item(i)->isSelected())
        {
            filmList temp=database->operator [](films[i]);
            temp.watched=!temp.watched;
            database->replace(films[i],temp);
        }
    }
    emit changed();
    showEvent(NULL);
}

//Не отоброжать в списке фильмов
void RejWindow::checkRejected_trigered()
{
    for (int i=0;i<=films.count()-1;i++)
    {
        if (ui->listWidget->item(i)->isSelected())
        {
            filmList temp=database->operator [](films[i]);
            temp.rejected=!temp.rejected;
            database->replace(films[i],temp);
        }
    }
    emit changed();
    showEvent(NULL);
}
