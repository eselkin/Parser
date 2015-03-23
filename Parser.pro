TEMPLATE = app
CONFIG += console
CONFIG += c++11
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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

