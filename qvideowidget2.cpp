#include "qvideowidget2.h"

QVideoWidget2::QVideoWidget2(QWidget *parent) :
    QVideoWidget(parent)
{
}

void QVideoWidget2::mouseDoubleClickEvent(QMouseEvent * event)
{
    emit changeFS();
}

void QVideoWidget2::mousePressEvent(QMouseEvent * event)
{
    if (event->button()==Qt::LeftButton && event->type()!=QEvent::MouseButtonDblClick)
    {
    emit playPause();
    }
}
