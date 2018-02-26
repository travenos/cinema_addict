#ifndef QVIDEOWIDGET2_H
#define QVIDEOWIDGET2_H

#include <QVideoWidget>
#include <QKeyEvent>

class QVideoWidget2 : public QVideoWidget
{
    Q_OBJECT
public:
    explicit QVideoWidget2(QWidget *parent = 0);
protected:
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);

signals:
    void playPause();
    void changeFS();

};

#endif // QVIDEOWIDGET2_H
