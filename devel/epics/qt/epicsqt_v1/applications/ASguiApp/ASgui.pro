# This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
# The EPICS QT Framework is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# The EPICS QT Framework is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with the EPICS QT Framework. If not, see <http://www.gnu.org/licenses/>.
# Copyright (c) 2009, 2010
# Author:
# Andrew Rhyder
# Contact details:
# andrew.rhyder@synchrotron.org.au
# To analyse code performance using the GNU gprof profiling tool:
# - Include the following two lines
# - Clean the project
# - Run qmake
# - Build the code
# - Run the program from a terminal
# - analyse the results with the command: gprof <your-program-name>
QMAKE_CXXFLAGS_DEBUG += -pg
QMAKE_LFLAGS_DEBUG += -pg
QT += network
TARGET = ASgui
TEMPLATE = app
CONFIG += uitools \
    debug_and_release
SOURCES += ./src/main.cpp \
    ./src/MainWindow.cpp \
    ./src/StartupParams.cpp \
    ./src/InstanceManager.cpp \
    $$(QTCREATOR)/gdbmacros/gdbmacros.cpp

# ./src/MainContext.cpp \
# include($$(QCAFRAMEWORK)/qtsingleapplication/src/qtsingleapplication.pri)
HEADERS += include/MainWindow.h \
    include/StartupParams.h \
    include/InstanceManager.h

# include/MainContext.h \
INCLUDEPATH += . \
    ./include \
    $$(QCAFRAMEWORK)/data/include \
    $$(QCAFRAMEWORK)/widgets/include \
    $$(QCAFRAMEWORK)/plugins/include
LIBS += -L$$(QCAFRAMEWORK)/plugins \
    -lQCaPlugin
LIBS += -L/opt/fontconfig/lib
FORMS += ./src/MainWindow.ui
