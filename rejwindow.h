#ifndef REJWINDOW_H
#define REJWINDOW_H

#include <QDialog>
#include <QInputDialog>
#include <QFile>
#include <QDir>
#include <QMessageBox>

struct filmList{    //Информация о фильме
    QString direcroty;  //Папка с фильмом
    QString movie;  //Имя файла фильма
    bool watched;   //Просмотрен ли фильм
    int mark;       //Оценка фильму
    bool rejected;  //Удалён ли фильм из списка
};

namespace Ui {
class RejWindow;
}

class RejWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RejWindow(QWidget *parent = 0, QVector<filmList> *db = NULL);
    ~RejWindow();

protected:
    void showEvent(QShowEvent *event);
    Ui::RejWindow *ui;

    QVector<filmList> *database;
    QVector<int> films;

private slots:
    virtual void on_pushButton_Restore_clicked();

    void on_pushButton_Close_clicked();

    void rename_trigered();

    void remove_trigered();

    void checkWatched_trigered();

    void checkRejected_trigered();

private:
    void makemenu();
    QAction *act_rename;
    QAction *act_remove;
    QAction *act_checkWatched;
    QAction *act_checkRejected;

signals:
    void changed();
};

#endif // REJWINDOW_H
