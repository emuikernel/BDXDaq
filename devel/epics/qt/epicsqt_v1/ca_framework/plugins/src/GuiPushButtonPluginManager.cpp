/*!
  \class GuiPushButtonPluginManager
  \version $Revision: #3 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author Last checked in by: $Author: rhydera $
  \brief Launch GUI Push Button Widget Plugin Manager for designer.
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

#include <GuiPushButtonPluginManager.h>
#include <GuiPushButtonPlugin.h>
#include <QtPlugin>

/*!
    ???
*/
GuiPushButtonPluginManager::GuiPushButtonPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void GuiPushButtonPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool GuiPushButtonPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a QCaLabel widget.
*/
QWidget *GuiPushButtonPluginManager::createWidget ( QWidget *parent ) {
    return new GuiPushButtonPlugin(parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString GuiPushButtonPluginManager::name() const {
    return "GuiPushButtonPlugin";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString GuiPushButtonPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon GuiPushButtonPluginManager::icon() const {
    return QIcon(":/icons/GuiPushButton.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString GuiPushButtonPluginManager::toolTip() const {
    return "GUI Launch Push Button";
}

/*!
    ???
*/
QString GuiPushButtonPluginManager::whatsThis() const {
    return "GUI Launch Push Button";
}

/*!
    ???
*/
bool GuiPushButtonPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString GuiPushButtonPluginManager::domXml() const {
    return "<widget class=\"GuiPushButton\" name=\"GuiPushButton\">\n"
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
QString GuiPushButtonPluginManager::includeFile() const {
    return "GuiPushButtonPlugin.h";
}
