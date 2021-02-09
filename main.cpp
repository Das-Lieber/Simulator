#include "mainwindow.h"
#include "MiniDumper.h"
#include "CustomSplashScreen.h"

#if _DEBUG
#include "vld/vld.h"
#endif

#include <QApplication>
#include <QTranslator>

QString InitStepToString(const ApplicationInitSteps &progress)
{
    switch(progress)
    {
    case ApplicationInitSteps::Start:
        return QObject::tr("Start initialization...");break;
    case ApplicationInitSteps::OpenCasCadeModel:
        return QObject::tr("initializing OpenCasCade Models...");break;
    case ApplicationInitSteps::LoadUI:
        return QObject::tr("Loading UI...");break;
    case ApplicationInitSteps::CreateDynamicModel:
        return QObject::tr("Creating Dynamic Model...");break;
    case ApplicationInitSteps::CreateScene:
        return QObject::tr("Creating Scene...");break;
    case ApplicationInitSteps::ParseDHArguments:
        return QObject::tr("Parsing DH Arguments...");break;
    case ApplicationInitSteps::ParseScene:
        return QObject::tr("Parsing Scene Arguments...");break;
    case ApplicationInitSteps::Load3DFile:
        return QObject::tr("Loading 3D Model Files...");break;
    case ApplicationInitSteps::ParsePTDContents:
        return QObject::tr("Parsing PTD File...");break;
    }
}

int InitStepToInt(const ApplicationInitSteps &progress)
{
    switch(progress)
    {
    case ApplicationInitSteps::Start:
        return 0;break;
    case ApplicationInitSteps::OpenCasCadeModel:
        return 0;break;
    case ApplicationInitSteps::LoadUI:
        return 10;break;
    case ApplicationInitSteps::CreateDynamicModel:
        return 20;break;
    case ApplicationInitSteps::CreateScene:
        return 25;break;
    case ApplicationInitSteps::ParseDHArguments:
        return 30;break;
    case ApplicationInitSteps::ParseScene:
        return 35;break;
    case ApplicationInitSteps::Load3DFile:
        return 40;break;
    case ApplicationInitSteps::ParsePTDContents:
        return 75;break;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MiniDumper dumper;

    QTranslator aTranslator;
    aTranslator.load(":/Simulator/Simulator_CN.qm");
    a.installTranslator(&aTranslator);

    QPixmap *splashMap = new QPixmap(":/Simulator/icons/splash.png");
    CustomSplashScreen *splash = new CustomSplashScreen(splashMap);

    splash->show();

    splash->setMessage(QObject::tr("Simulator V0.1"));

    MainWindow window;
    QObject::connect(&window,&MainWindow::initStepChanged,splash,[=](const ApplicationInitSteps &progress){
        splash->setMessage(InitStepToString(progress));
        splash->setProgressValue(InitStepToInt(progress));
    });

    window.initOCC();
    window.initInterface();
    window.initRL();

    splash->setProgressValue(100);
    window.showMaximized();
    splash->finish(&window);
    delete splash;

    return a.exec();
}
