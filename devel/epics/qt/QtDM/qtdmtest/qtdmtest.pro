#-------------------------------------------------
#
# Project created by QtCreator 2013-10-01T18:23:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtdmtest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui \
            motor_expert.ui

unix:!macx: LIBS += -L$$PWD/../AAA/extensions/lib/linux-x86/ -lcaQtDM_Lib

unix:!macx: LIBS += -L$$PWD/../AAA/extensions/lib/linux-x86/ -lqtcontrols

INCLUDEPATH += $$PWD/../caQtDM_QtControls/src
DEPENDPATH += $$PWD/../caQtDM_QtControls/src


