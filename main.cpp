#include "mainwindow.h"
#include "MiniDumper.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MiniDumper dumper;

//    QTranslator aTranslator;
//    aTranslator.load(":/Simulator/Simulator_CN.qm");
//    a.installTranslator(&aTranslator);

    MainWindow window;
    window.show();
    QTimer::singleShot(100,&window, &MainWindow::initRL);//show first, then load the data
    return a.exec();
}
