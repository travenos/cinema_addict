#-------------------------------------------------
#
# Project created by QtCreator 2014-08-01T16:08:52
#
#-------------------------------------------------

QT       += core gui
QT  +=multimedia
QT += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Cinema
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    rejwindow.cpp \
    dbwindow.cpp \
    qvideowidgetp.cpp \
    qvideowidget2.cpp

HEADERS  += mainwindow.h \
    rejwindow.h \
    dbwindow.h \
    qvideowidgetp.h \
    qvideowidget2.h

FORMS    += mainwindow.ui \
    rejwindow.ui

RESOURCES += \
    resource.qrc
RC_FILE = icon.rc
