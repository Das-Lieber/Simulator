QT       += core gui widgets

CONFIG += c++11

RC_ICONS += Simulator.ico

DEFINES += QT_DEPRECATED_WARNINGS \
            M_PI=3.1415926535

QMAKE_CXXFLAGS += /utf-8

SOURCES += \
    QRibbon/QRibbon.cpp \
    delegate/ConfigurationDelegate.cpp \
    delegate/ConfigurationModel.cpp \
    delegate/OperationalDelegate.cpp \
    delegate/OperationalModel.cpp \
    dock/CustomDockTabBar.cpp \
    dock/CustomDockTabBarPushButton.cpp \
    dock/CustomDockWidget.cpp \
    dock/CustomDockWidgetBar.cpp \
    main.cpp \
    mainwindow.cpp \
    occWidget.cpp \
    robotics/RLAPI_PlanThread.cpp \
    robotics/RLAPI_Reader.cpp \
    robotics/RLAPI_Writer.cpp \
    robotics/RLConvertAPI.cpp

HEADERS += \
    QRibbon/QRibbon.h \
    delegate/ConfigurationDelegate.h \
    delegate/ConfigurationModel.h \
    delegate/OperationalDelegate.h \
    delegate/OperationalModel.h \
    dock/CustomDockTabBar.h \
    dock/CustomDockTabBarPushButton.h \
    dock/CustomDockWidget.h \
    dock/CustomDockWidgetBar.h \
    mainwindow.h \
    occWidget.h \
    robotics/RLAPI_ConfigurationOptimizer.h \
    robotics/RLAPI_PlanThread.h \
    robotics/RLAPI_Reader.h \
    robotics/RLAPI_Writer.h \
    robotics/RLConvertAPI.h

FORMS += \
    QRibbon/qribbon.ui \
    mainwindow.ui

#TRANSLATIONS += Simulator_CN.ts \
#                Simulator_EN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    QRibbon/QRibbon.qrc \
    Simulator.qrc \
    dock/CustomDock.qrc

INCLUDEPATH += D:/OpenCASCADE/inc
LIBS += D:\OpenCASCADE\lib\*.lib
INCLUDEPATH += D:/RoboticsLibrary/include
LIBS += -LD:\RoboticsLibrary\lib
CONFIG(debug, debug|release) {    
    LIBS += -lrlsgsd -lrlmdlsd -lrlplansd -llibxml2d -llibxsltd -lPQPd -lrlkinsd
} else {
    LIBS += -lrlsgs -lrlmdls -lrlplans -llibxml2 -llibxslt -lPQP -lrlkins
}

CONFIG(debug, debug|release) {
    TARGET = Simulatord
} else {
    TARGET = Simulator
}

DESTDIR  = $$PWD\bin
