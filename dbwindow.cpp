#include "dbwindow.h"
#include "ui_rejwindow.h"

DBWindow::DBWindow(QWidget *parent,  QVector<filmList> *db) :
    RejWindow(parent,db)
{
    ui->pushButton_Restore->setText("Убрать из библиотеки");
    ui->pushButton_Restore->setIcon(QIcon(":/new/img/icons/edit-clear-list.png"));
    this->setWindowTitle("Библиотека");
    delFDB= new QAction(QIcon(":/new/img/icons/edit-clear-list.png"),"Убрать из библиотеки",this);
    ui->listWidget->addAction(delFDB);
    connect(delFDB,SIGNAL(triggered()),this,SLOT(on_pushButton_Restore_clicked()));

}

void DBWindow::showEvent(QShowEvent *event)
{
    ui->listWidget->clear();
    films.clear();
    QColor color;
    for(int i=0;i<=database->count()-1;i++)
    {
        ui->listWidget->addItem(database->operator [](i).movie);
        if (database->operator [](i).rejected)
        {
            color.setRgb(255,0,0);
            ui->listWidget->item(i)->setTextColor(color);

        }
        else if (database->operator [](i).watched)
        {
            color.setRgb(128,128,128);
            ui->listWidget->item(i)->setTextColor(color);

        }

        films.push_back(i);
    }
}

void DBWindow::on_pushButton_Restore_clicked()
{
    try
    {
        if(!ui->listWidget->currentIndex().isValid())
            throw 0;
        for(int i=0;i<=films.count()-1;i++)
            if (ui->listWidget->item(i)->isSelected())
            {
                database->remove(films[i]);
                for (int j=i+1;j<=films.count()-1;j++)
                    if (films[j]>films[i])
                        films[j]--;
            }
        emit changed();
        showEvent(NULL);
    }
    catch(...){QMessageBox::warning(this,"Ошибка","Невозможно удалить из списка фильм");}
}

DBWindow::~DBWindow()
{
    delFDB->deleteLater();
}
