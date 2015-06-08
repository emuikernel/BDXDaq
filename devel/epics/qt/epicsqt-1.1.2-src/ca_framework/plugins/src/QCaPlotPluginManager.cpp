/*! 
  \class QCaPlotPluginManager
  \version $Revision: #1 $
  \date $DateTime: 2010/08/30 16:37:08 $
  \author glenn.jackson
  \brief CA Plot Widget Plugin Manager for designer.
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
 *    Glenn Jackson
 *  Contact details:
 *    glenn.jackson@synchrotron.org.au
 */

#include <QCaPlotPluginManager.h>
#include <QCaPlotPlugin.h>
#include <QtPlugin>

/*!
    ???
*/
QCaPlotPluginManager::QCaPlotPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void QCaPlotPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool QCaPlotPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a QCaPlot widget.
*/
QWidget *QCaPlotPluginManager::createWidget ( QWidget *parent ) {
    return new QCaPlotPlugin(parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString QCaPlotPluginManager::name() const {
    return "QCaPlotPlugin";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString QCaPlotPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon QCaPlotPluginManager::icon() const {
    return QIcon(":/icons/QCaPlot.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString QCaPlotPluginManager::toolTip() const {
    return "EPICS Strip Chart";
}

/*!
    ???
*/
QString QCaPlotPluginManager::whatsThis() const {
    return "EPICS Strip Chart";
}

/*!
    ???
*/
bool QCaPlotPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString QCaPlotPluginManager::domXml() const {
    return "<widget class=\"QCaPlot\" name=\"qCaPlot\">\n"
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
QString QCaPlotPluginManager::includeFile() const {
    return "QCaPlotPlugin.h";
}
