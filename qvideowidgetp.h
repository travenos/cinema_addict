#ifndef QVIDEOWIDGETP_H
#define QVIDEOWIDGETP_H

#include <QVideoWidget>
#include <QKeyEvent>

class QVideoWidgetP : public QVideoWidget
{
    Q_OBJECT
public:
    explicit QVideoWidgetP(QWidget *parent = 0);
protected:
    void keyPressEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);

signals:
    void playPause();

};

#endif // QVIDEOWIDGETP_H
