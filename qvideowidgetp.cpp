#include "qvideowidgetp.h"

QVideoWidgetP::QVideoWidgetP(QWidget *parent) :
    QVideoWidget(parent)
{
}

void QVideoWidgetP::keyPressEvent(QKeyEvent* event)
{
    if(event->matches(QKeySequence::Close) ||event->matches(QKeySequence::Back)||event->matches(QKeySequence::FullScreen) || event->key()==Qt::Key_Escape|| event->key()==Qt::Key_F11)
    {
        deleteLater();
    }
    if(event->key()==Qt::Key_Space || event->key()==Qt::Key_Pause || event->key()==Qt::Key_Play)
        emit playPause();
}

void QVideoWidgetP::mouseDoubleClickEvent(QMouseEvent * event)
{
    deleteLater();
}

void QVideoWidgetP::mousePressEvent(QMouseEvent * event)
{
    if (event->button()==Qt::LeftButton && event->type()!=QEvent::MouseButtonDblClick)
    {
    emit playPause();
    }
}

void QVideoWidgetP::closeEvent(QCloseEvent* event)
{
    deleteLater();
}
