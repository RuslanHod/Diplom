#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{ 
    //Создаем кодек CP1251
    QTextCodec* codec_cp = QTextCodec::codecForName("CP1251");
    QTextCodec::setCodecForLocale(codec_cp);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
