TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += qtcore
QMAKE_CXXFLAGS += -std=gnu++11
SOURCES += main.cpp \
    mwindow.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    Parser.h \
    node.h \
    heapsort.h \
    presorter.h \
    bst.h \
    mwindow.h

