QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(debug, debug|release) {
    TARGET = Simulator_d
} else {
    TARGET = Simulator
}

DESTDIR  = $$PWD\bin

TEMPLATE = app

QMAKE_CXXFLAGS += /utf-8

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS \
            M_PI=3.1415926535

SOURCES += \
    RLAPI_PlanThread.cpp \
    RLAPI_Reader.cpp \
    RLAPI_Writer.cpp \
    RLConvertAPI.cpp \
    main.cpp \
    occWidget.cpp \
    widget.cpp

HEADERS += \
    RLAPI_ConfigurationOptimizer.h \
    RLAPI_PlanThread.h \
    RLAPI_Reader.h \
    RLAPI_Writer.h \
    RLConvertAPI.h \
    occWidget.h \
    widget.h

FORMS += \
    widget.ui

INCLUDEPATH += D:/OpenCASCADE/inc
LIBS += D:\OpenCASCADE\lib\*.lib
CONFIG(debug, debug|release) {
    INCLUDEPATH += D:/RoboticsLibrary/Debug/include
    LIBS += -LD:\RoboticsLibrary\Debug\lib
    LIBS += -lrlsgsd -lrlmdlsd -lrlplansd -llibxml2d -llibxsltd -lPQPd -lrlkinsd
} else {
    INCLUDEPATH += D:/RoboticsLibrary/Release/include
    LIBS += -LD:\RoboticsLibrary\Release\lib
    LIBS += -lrlsgs -lrlmdls -lrlplans -llibxml2 -llibxslt -lPQP -lrlkins
}
