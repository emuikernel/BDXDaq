/*! 
  \class QCaLineEditPluginManager
  \version $Revision: #3 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief CA Line Edit Widget Plugin Manager for designer.
 */
/*
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#include <QCaLineEditPluginManager.h>
#include <QCaLineEditPlugin.h>
#include <QtPlugin>

/*!
    ???
*/
QCaLineEditPluginManager::QCaLineEditPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void QCaLineEditPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool QCaLineEditPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a QCaLabel widget.
*/
QWidget *QCaLineEditPluginManager::createWidget ( QWidget *parent ) {
    return new QCaLineEditPlugin(parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString QCaLineEditPluginManager::name() const {
    return "QCaLineEditPlugin";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString QCaLineEditPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon QCaLineEditPluginManager::icon() const {
    return QIcon(":/icons/QCaLineEdit.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString QCaLineEditPluginManager::toolTip() const {
    return "EPICS Line Edit";
}

/*!
    ???
*/
QString QCaLineEditPluginManager::whatsThis() const {
    return "EPICS Line Edit";
}

/*!
    ???
*/
bool QCaLineEditPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString QCaLineEditPluginManager::domXml() const {
    return "<widget class=\"QCaLineEdit\" name=\"qCaLineEdit\">\n"
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>100</width>\n"
           "   <height>100</height>\n"
           "  </rect>\n"
           " </property>\n"
           " <property name=\"toolTip\" >\n"
           "  <string></string>\n"
           " </property>\n"
           " <property name=\"whatsThis\" >\n"
           "  <string> "
           ".</string>\n"
           " </property>\n"
           "</widget>\n";
}*/

/*!
    ???
*/
QString QCaLineEditPluginManager::includeFile() const {
    return "QCaLineEditPlugin.h";
}
