#
#    This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
#
#    The EPICS QT Framework is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    The EPICS QT Framework is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
#
#    Copyright (c) 2009, 2010
#
#    Author:
#      Andrew Rhyder
#    Contact details:
#      andrew.rhyder@synchrotron.org.au
#

QT -= gui
TARGET = QCaMonitor
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += \
    ./src/main.cpp \
    ./src/monitor.cpp \
    $$(QTCREATOR)/gdbmacros/gdbmacros.cpp

HEADERS += \
    include/monitor.h

INCLUDEPATH += \
    ./include \
    $$(QCAFRAMEWORK)/api/include \
    $$(QCAFRAMEWORK)/data/include \
    $$(QCAFRAMEWORK)/plugins/include \
    $$(EPICS_BASE)/include

# Uncomment if linking agains QCa library in Qt Designer plugin directory
#LIBS += -L$$(QTDIR)/plugins/designer -lQCaPlugin

# Uncomment if linking agains QCa library in Qt Designer plugin directory
LIBS += -L$$(QCAFRAMEWORK)/plugins/release -lQCaPlugin
