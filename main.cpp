#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator aTranslator;
    aTranslator.load(":/Simulator/Simulator_CN.qm");
    a.installTranslator(&aTranslator);

    MainWindow w;
    w.show();
    return a.exec();
}
