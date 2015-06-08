#-------------------------------------------------
#
# Project created by QtCreator 2013-06-19T19:09:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = smi_browse
TEMPLATE = app


SOURCES += main.cpp \
        mainwindow.cpp \
        smidevice.cpp \
        smibrowse.cpp \
        graphbrowse.cpp

HEADERS  += mainwindow.h \
            smidevice.h \
            smibrowse.h \
            graphbrowse.h

FORMS    += mainwindow.ui

#-------------------------------------------------------------------
unix:!macx: LIBS += -L$$PWD/../ca_framework/qwt-5.2.1/lib/ -lqwt

INCLUDEPATH += $$PWD/../ca_framework/qwt-5.2.1/src/
DEPENDPATH += $$PWD/../ca_framework/qwt-5.2.1/src/

#unix:!macx: LIBS += -L$$PWD/../../../../../../apps/Trolltech/qwt-6.0.0/lib/ -lqwt

#INCLUDEPATH += $$PWD/../../../../../../apps/Trolltech/qwt-6.0.0
#DEPENDPATH += $$PWD/../../../../../../apps/Trolltech/qwt-6.0.0
#-------------------------------------------------------------------

unix:!macx: LIBS += -L$$PWD/../ca_framework/plugins/ -lQCaPlugin

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


unix:!macx: LIBS += -L$$PWD/../../../../../../apps/dim/linux/ -ldim

INCLUDEPATH += $$PWD/../../../../../../apps/dim/dim
DEPENDPATH += $$PWD/../../../../../../apps/dim/dim

unix:!macx: LIBS += -L$$PWD/../../../../../../apps/smixx/linux/ -lsmi

INCLUDEPATH += $$PWD/../../../../../../apps/smixx/smixx
#DEPENDPATH += $$PWD/../../../../../../apps/smixx/smixx

unix:!macx: LIBS += -L$$PWD/../../../../../../apps/smixx/linux/ -lsmiui

#INCLUDEPATH += $$PWD/../../../../../../apps/smixx/smixx
#DEPENDPATH += $$PWD/../../../../../../apps/smixx/smixx

