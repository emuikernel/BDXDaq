/*! 
  \class QCaLabelPluginManager
  \version $Revision: #3 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief CA Label Widget Plugin Manager for designer.
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

#include <QCaLabelPluginManager.h>
#include <QCaLabelPlugin.h>
#include <QtPlugin>

/*!
    ???
*/
QCaLabelPluginManager::QCaLabelPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void QCaLabelPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool QCaLabelPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a QCaLabel widget.
*/
QWidget *QCaLabelPluginManager::createWidget ( QWidget *parent ) {
    return new QCaLabelPlugin(parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString QCaLabelPluginManager::name() const {
    return "QCaLabelPlugin";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString QCaLabelPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon QCaLabelPluginManager::icon() const {
    return QIcon(":/icons/QCaLabel.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString QCaLabelPluginManager::toolTip() const {
    return "EPICS Label";
}

/*!
    ???
*/
QString QCaLabelPluginManager::whatsThis() const {
    return "EPICS Label";
}

/*!
    ???
*/
bool QCaLabelPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString QCaLabelPluginManager::domXml() const {
    return "<widget class=\"QCaLabel\" name=\"qCaLabel\">\n"
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
QString QCaLabelPluginManager::includeFile() const {
    return "QCaLabelPlugin.h";
}
