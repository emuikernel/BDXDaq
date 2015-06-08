/*!
  \class CmdPushButton
  \version $Revision: #6 $
  \date $DateTime: 2010/09/06 13:16:04 $
  \author Last checked in by: $Author: rhydera $
  \brief Command Push Button Widget.
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

#ifndef CMDPUSHBUTTON_H
#define CMDPUSHBUTTON_H

#include <QPushButton>
#include <ContainerProfile.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT CmdPushButton : public QPushButton {
    Q_OBJECT

  public:
    CmdPushButton( QWidget *parent = 0 );

    // Property convenience functions

    // Program String
    void setProgram( QString program );
    QString getProgram();

    // Arguments String
    void setArguments( QStringList arguments );
    QStringList getArguments();

    // Variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
    // Note, unlike most objects with a variable name substitions, changes to this property do not take immediate effect, so the normal code which waits
    // for the user to finish typing, then propogates the substitutions is not required. the substitutions are simply used next time the button is pushed.
    void    setVariableNameSubstitutions( QString variableNameSubstitutions );
    QString getVariableNameSubstitutions();


  public slots:
    void userClicked();

    void requestEnabled( const bool& state );


  protected:
    QString program;        /// Program to run
    QStringList arguments;  /// Program arguments

  private:
    void updateToolTip ( const QString & toolTip );
    ContainerProfile profile;

    QString variableNameSubstitutions;

};

#endif /// CMDPUSHBUTTON_H
