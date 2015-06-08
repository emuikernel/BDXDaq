/*! 
  \class QCaDesignerPlugin
  \version $Revision: #6 $
  \date $DateTime: 2010/09/06 13:29:44 $
  \author andrew.rhyder
  \brief Collection of CA plugins for QT Designer.
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
#include <QCaLineEditPluginManager.h>
#include <QCaPushButtonPluginManager.h>
#include <GuiPushButtonPluginManager.h>
#include <CmdPushButtonPluginManager.h>
#include <QCaShapePluginManager.h>
#include <QCaSliderPluginManager.h>
#include <QCaSpinBoxPluginManager.h>
#include <QCaComboBoxPluginManager.h>
#include <ASguiFormPluginManager.h>
#include <LinkPluginManager.h>
#include <QCaPlotPluginManager.h>

#include <QtCore/qplugin.h>

#include <QCaDesignerPlugin.h>

/*!
    Add all plugins that will be provided in QT Designer.
*/
QCaWidgets::QCaWidgets(QObject *parent) : QObject(parent) {
    widgets.append(new QCaLabelPluginManager(this));
    widgets.append(new QCaLineEditPluginManager(this));
    widgets.append(new QCaPushButtonPluginManager(this));
    widgets.append(new GuiPushButtonPluginManager(this));
    widgets.append(new CmdPushButtonPluginManager(this));
    widgets.append(new QCaShapePluginManager(this));
    widgets.append(new QCaSliderPluginManager(this));
    widgets.append(new QCaSpinBoxPluginManager(this));
    widgets.append(new QCaComboBoxPluginManager(this));
    widgets.append(new ASguiFormPluginManager(this));
    widgets.append(new LinkPluginManager(this));
    widgets.append(new QCaPlotPluginManager(this));
}

/*!
    Multiplugin interface.
*/
QList<QDesignerCustomWidgetInterface*> QCaWidgets::customWidgets() const {
    return widgets;
}

//! Publish plugins through to QT designer.
Q_EXPORT_PLUGIN2(QCaWidgetsPlugin, QCaWidgets)
