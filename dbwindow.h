#ifndef DBWINDOW_H
#define DBWINDOW_H

#include "rejwindow.h"
#include <QDialog>

//namespace Ui {
//class DBWindow;
//}

class DBWindow : public RejWindow
{
    Q_OBJECT
public:
    explicit DBWindow(QWidget *parent = 0, QVector<filmList> *db = NULL);
    ~DBWindow();

protected:
    void showEvent(QShowEvent *event);
    void action();

signals:

public slots:

private slots:
    void on_pushButton_Restore_clicked();

private:
    QAction *delFDB;

//private:
 //   Ui::DBWindow *ui;


};

#endif // DBWINDOW_H
