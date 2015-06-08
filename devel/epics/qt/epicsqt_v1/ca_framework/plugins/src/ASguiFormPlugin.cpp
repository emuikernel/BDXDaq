/*!
  \class ASguiFormPlugin
  \version $Revision: #4 $
  \date $DateTime: 2010/09/06 11:58:56 $
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

#include <ASguiFormPlugin.h>

#include <QtCore>


/// Constructor
ASguiFormPlugin::ASguiFormPlugin( QWidget* parent ) : ASguiForm( parent )
{
    /// Set up a connection to recieve variable name property changes (actually only substitutions are used)
    /// The variable name property manager class only delivers an updated variable name (or substition) after the user has stopped typing
    QObject::connect( &variableNamePropertyManager, SIGNAL( newVariableNameProperty( QString, QString, unsigned int ) ), this, SLOT( useNewVariableNameProperty( QString, QString, unsigned int ) ) );
}

/*!
    Slot to recieve macro substitutions property changes.
*/
void ASguiFormPlugin::useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
{
    /// Set a new variable name substitutions
    /// (Variable name will not be used. A form may supply macro substitutions to it's QCa widgets but does not use a variable name itself)
    setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
}
