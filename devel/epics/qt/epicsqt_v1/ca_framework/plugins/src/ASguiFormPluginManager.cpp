/*!
  \class ASguiFormPluginManager
  \version $Revision: #3 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief
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

#include <ASguiFormPluginManager.h>
#include <ASguiFormPlugin.h>

#include <QtPlugin>

ASguiFormPluginManager::ASguiFormPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

void ASguiFormPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

bool ASguiFormPluginManager::isInitialized() const {
    return initialized;
}

/// Widget factory. Creates a ASguiForm widget.
QWidget *ASguiFormPluginManager::createWidget ( QWidget *parent ) {
    return new ASguiFormPlugin(parent);
}

/// Name for widget. Used by Qt Designer in widget list
QString ASguiFormPluginManager::name() const {
    return "ASguiFormPlugin";
}

/// Name of group Qt Designer will add widget to
QString ASguiFormPluginManager::group() const {
    return "EPICS Widgets";
}

/// Icon for widget. Used by Qt Designer in widget list
QIcon ASguiFormPluginManager::icon() const {
    return QIcon(":/icons/ASguiForm.png");
}

/// Tool tip for widget. Used by Qt Designer in widget list
QString ASguiFormPluginManager::toolTip() const {
    return "ASgui form";
}

///
QString ASguiFormPluginManager::whatsThis() const {
    return "ASgui form";
}

///
bool ASguiFormPluginManager::isContainer() const {
    return false;
}

///
/*QString ASguiFormPluginManager::domXml() const {
    return "<widget class=\"ASguiForm\" name=\"ASguiForm\">\n"
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

///
QString ASguiFormPluginManager::includeFile() const {
    return "ASguiFormPlugin.h";
}
