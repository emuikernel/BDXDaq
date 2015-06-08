#-------------------------------------------------
#
# Project created by QtCreator 2013-06-19T19:09:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hv_control
TEMPLATE = app


SOURCES += main.cpp \
        mainwindow.cpp


HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

unix:!macx: LIBS += -L$$PWD/../ca_framework/qwt-5.2.1/lib/ -lqwt

INCLUDEPATH += $$MYEPICSQT/ca_framework/qwt-5.2.1/src/
DEPENDPATH += $$MYEPICSQT/ca_framework/qwt-5.2.1/src/



unix:!macx: LIBS += -L$$PWD/../ca_framework/plugins/ -lQCaPlugin

#INCLUDEPATH += $$MYEPICSQT/ca_framework/plugins/include
INCLUDEPATH += $$PWD/../ca_framework/plugins/include
DEPENDPATH += $$PWD/../ca_framework/plugins/include

INCLUDEPATH += $$PWD/../ca_framework/data/include
DEPENDPATH += $$PWD/../ca_framework/data/include
INCLUDEPATH += $$PWD/../ca_framework/api/include
DEPENDPATH += $$PWD/../ca_framework/api/include
INCLUDEPATH += $$PWD/../ca_framework/widgets/include
DEPENDPATH += $$PWD/../ca_framework/widgets/include
#INCLUDEPATH += $$PWD/../ca_framework//include
#DEPENDPATH += $$PWD/../ca_framework//include


#unix:!macx: LIBS += -L$$PWD/../../../../../../home/sytnik/dim/linux/ -ldim

#INCLUDEPATH += $$PWD/../../../../../../home/sytnik/dim/dim
#DEPENDPATH += $$PWD/../../../../../../home/sytnik/dim/dim

#unix:!macx: LIBS += -L$$PWD/../../../../../../home/sytnik/smixx/linux/ -lsmi

#INCLUDEPATH += $$PWD/../../../../../../home/sytnik/smixx/smixx
#DEPENDPATH += $$PWD/../../../../../../home/sytnik/smixx/smixx

#unix:!macx: LIBS += -L$$PWD/../../../../../../home/sytnik/smixx/linux/ -lsmiui

#INCLUDEPATH += $$PWD/../../../../../../home/sytnik/smixx/smixx
#DEPENDPATH += $$PWD/../../../../../../home/sytnik/smixx/smixx
