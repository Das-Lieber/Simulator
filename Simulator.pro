QT       += core gui widgets sql winextras

CONFIG += c++11

RC_ICONS += Simulator.ico

DEFINES += QT_DEPRECATED_WARNINGS \
            M_PI=3.1415926535

QMAKE_CXXFLAGS += /utf-8

SOURCES += \
    OCC/AIS_Coordinate.cpp \
    OCC/occWidget.cpp \
    QRibbon/QRibbon.cpp \
	delegate/SqlTableViewDelegate.cpp \
    delegate/tableViewJointDelegate.cpp \
    delegate/tableViewJointModel.cpp \
    delegate/tableViewPosDelegate.cpp \
    delegate/tableViewPosModel.cpp \
    dialog/DHSettingWidget.cpp \
    dialog/EditLocationWidget.cpp \
    dialog/ProcessDataWidget.cpp \    
    dialog/CustomSplashScreen.cpp \
    dock/CustomDockTabBar.cpp \
    dock/CustomDockTabBarPushButton.cpp \
    dock/CustomDockWidget.cpp \
    dock/CustomDockWidgetBar.cpp \    
    main.cpp \
    mainwindow.cpp \
    robotics/RLAPI_DHSetting.cpp \
    robotics/RLAPI_PlanThread.cpp \
    robotics/RLAPI_Reader.cpp \
    robotics/RLAPI_Writer.cpp \
    robotics/RLConvertAPI.cpp \
	tools/gifthread.cpp \
	tools/MiniDumper.cpp

HEADERS += \
    OCC/AIS_Coordinate.h \
    OCC/occWidget.h \
    QRibbon/QRibbon.h \
	delegate/SqlTableViewDelegate.h \
    delegate/tableViewJointDelegate.h \
    delegate/tableViewJointModel.h \
    delegate/tableViewPosDelegate.h \
    delegate/tableViewPosModel.h \
    dialog/DHSettingWidget.h \
    dialog/EditLocationWidget.h \
    dialog/ProcessDataWidget.h \    
    dialog/CustomSplashScreen.h \
    dock/CustomDockTabBar.h \
    dock/CustomDockTabBarPushButton.h \
    dock/CustomDockWidget.h \
    dock/CustomDockWidgetBar.h \    
    mainwindow.h \
    robotics/RLAPI_ConfigurationOptimizer.h \
    robotics/RLAPI_DHSetting.h \
    robotics/RLAPI_PlanThread.h \
    robotics/RLAPI_Reader.h \
    robotics/RLAPI_Writer.h \
    robotics/RLConvertAPI.h \
	tools/gif.h \
	tools/gifthread.h \
	tools/MiniDumper.h

FORMS += \    
    QRibbon/qribbon.ui \
    dialog/DHSettingWidget.ui \
    dialog/EditLocationWidget.ui \
    dialog/ProcessDataWidget.ui \
    dialog/CustomSplashScreen.ui \
    mainwindow.ui

#TRANSLATIONS += Simulator_CN.ts

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
    LIBS += -lrlsgsd -lrlmdlsd -lrlplansd -llibxml2d -llibxsltd -lPQPd -lrlkinsd -lnloptd
} else {
    LIBS += -lrlsgs -lrlmdls -lrlplans -llibxml2 -llibxslt -lPQP -lrlkins -lnlopt
}

CONFIG(debug, debug|release) {
    LIBS += $$PWD/vld/vld.lib
}

CONFIG(debug, debug|release) {
    TARGET = Simulatord
} else {
    TARGET = Simulator
}

DESTDIR  = $$PWD\bin
