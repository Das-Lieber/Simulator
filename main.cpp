#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QTranslator aTranslator;
//    aTranslator.load(":/Simulator/Simulator_CN.qm");
//    a.installTranslator(&aTranslator);

    MainWindow window;
    window.show();
    QTimer::singleShot(10,&window, &MainWindow::initRL);//show first, then load the data
    return a.exec();
}
