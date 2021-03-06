#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QTranslator translator;
    translator.load("qtbase_ru",":/new/img/");
    QApplication a(argc, argv);
    a.installTranslator(&translator);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    MainWindow w;
    QDir dir;
    QDir* dirPtr = NULL;
    if(argc >= 2)
    {
#if (defined __linux__)
        if(argv[1][0]!='/')
#else
#if (defined __WIN32__ || defined __WIN64__)
        if(QString(*argv).contains(":"))
#endif
#endif
          dir.setPath(QDir::currentPath()+QDir::separator()+argv[1]);
        else
          dir.setPath(argv[1]);
        dirPtr = &dir;
    }
    w.setDir(dirPtr);
    w.show();

    return a.exec();
}
