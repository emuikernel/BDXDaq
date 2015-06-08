/*!
  \class GuiPushButton
  \version $Revision: #6 $
  \date $DateTime: 2010/09/06 13:16:04 $
  \author Last checked in by: $Author: rhydera $
  \brief Launch GUI Push Button Widget.
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

#ifndef GUIPUSHBUTTON_H
#define GUIPUSHBUTTON_H

#include <QPushButton>
#include <ASguiForm.h>
#include <UserMessage.h>
#include <ContainerProfile.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT GuiPushButton : public QPushButton {
    Q_OBJECT

  public:
    GuiPushButton( QWidget *parent = 0 );

    QString variableNameSubstitutions;
    ASguiForm::creationOptions creationOption;

    // Property convenience functions

    // GUI name
    void setGuiName( QString guiName );
    QString getGuiName();

    // Qt Designer Properties Creation options
    void setCreationOption( ASguiForm::creationOptions creationOption );
    ASguiForm::creationOptions getCreationOption();

    // Qt Designer Properties - variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
    // Note, unlike most objects with a variable name substitions, changes to this property do not take immediate effect, so the normal code which waits
    // for the user to finish typing, then propogates the substitutions is not required. the substitutions are simply used next time the button is pushed.
    void    setVariableNameSubstitutions( QString variableNameSubstitutions );
    QString getVariableNameSubstitutions();


  public slots:
    void userClicked();
    void onGeneralMessage( QString message );

    void requestEnabled( const bool& state );


private slots:
    void launchGui( QString guiName, QString substitutions, ASguiForm::creationOptions creationOption );

  signals:
    void newGui( QString guiName, QString substitutions, ASguiForm::creationOptions creationOption );

  protected:
    QString guiName;      /// GUI file name to launch

  private:
    void updateToolTip ( const QString & toolTip );
    UserMessage userMessage;
    ContainerProfile profile;

};

#endif /// GUIPUSHBUTTON_H
