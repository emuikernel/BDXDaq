/*! 
  \class LinkPluginManager
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

#include <LinkPluginManager.h>
#include <LinkPlugin.h>
#include <QtPlugin>

/*!
    ???
*/
LinkPluginManager::LinkPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void LinkPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool LinkPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a Link widget.
*/
QWidget *LinkPluginManager::createWidget ( QWidget *parent ) {
    return new LinkPlugin(parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString LinkPluginManager::name() const {
    return "LinkPlugin";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString LinkPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon LinkPluginManager::icon() const {
    return QIcon(":/icons/Link.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString LinkPluginManager::toolTip() const {
    return "Link";
}

/*!
    ???
*/
QString LinkPluginManager::whatsThis() const {
    return "Link";
}

/*!
    ???
*/
bool LinkPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString LinkPluginManager::domXml() const {
    return "<widget class=\"Link\" name=\"Link\">\n"
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
QString LinkPluginManager::includeFile() const {
    return "LinkPlugin.h";
}
