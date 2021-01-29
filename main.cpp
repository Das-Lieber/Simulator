#include "mainwindow.h"
#include "MiniDumper.h"

#if _DEBUG
#include "vld/vld.h"
#endif

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MiniDumper dumper;

    QTranslator aTranslator;
    aTranslator.load(":/Simulator/Simulator_CN.qm");
    a.installTranslator(&aTranslator);

    MainWindow window;
    window.showMaximized();
    QTimer::singleShot(100,&window, &MainWindow::initRL);//show first, then load the data
    return a.exec();
}
