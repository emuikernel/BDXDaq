/*!
  \class GuiPushButtonPlugin
  \version $Revision: #6 $
  \date $DateTime: 2010/09/06 11:58:56 $
  \author Last checked in by: $Author: rhydera $
  \brief Launch GUI Push Button Widget Plugin.
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

#ifndef GUIPUSHBUTTONPLUGIN_H
#define GUIPUSHBUTTONPLUGIN_H

#include <QString>
#include <GuiPushButton.h>

class GuiPushButtonPlugin : public GuiPushButton {
    Q_OBJECT

  public:
    /// Constructors
    GuiPushButtonPlugin( QWidget *parent = 0 );

    // Note, a property macro in the form 'Q_PROPERTY(QString guiName READ ...' doesn't work.
    // A property name ending with 'Name' results in some sort of string variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString guiFile READ getGuiName WRITE setGuiName)

    // Creation options
    Q_ENUMS(CreationOptionNames)
    Q_PROPERTY(CreationOptionNames creationOption READ getCreationOptionProperty WRITE setCreationOptionProperty)
    enum CreationOptionNames { Open = ASguiForm::CREATION_OPTION_OPEN,
                               NewTab = ASguiForm::CREATION_OPTION_NEW_TAB,
                               NewWindow = ASguiForm::CREATION_OPTION_NEW_WINDOW };
    void setCreationOptionProperty( CreationOptionNames creationOptionIn ){ setCreationOption( (ASguiForm::creationOptions)creationOptionIn ); }
    CreationOptionNames getCreationOptionProperty(){ return (CreationOptionNames)getCreationOption(); }

    // Variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
    // Note, unlike most objects with a variable name substitions, changes to this property do not take immediate effect, so the normal code which waits
    // for the user to finish typing, then propogates the substitutions is not required. the substitutions are simply used next time the button is pushed.
    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutions WRITE setVariableNameSubstitutions)
};

#endif /// GUIPUSHBUTTONPLUGIN_H
