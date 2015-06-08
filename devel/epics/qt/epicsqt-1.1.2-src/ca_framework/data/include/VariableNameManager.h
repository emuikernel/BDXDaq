/*!
  \class VariableNameManager
  \version $Revision: #7 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief Variable name management.
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

#ifndef VARIABLENAMEMANAGER_H
#define VARIABLENAMEMANAGER_H

#include <QString>
#include <QList>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT VariableNameManager {

public:
    VariableNameManager();
    virtual ~VariableNameManager(){};

    void variableNameManagerInitialise( unsigned int numVariables ) ;                     /// Used to set up the number of variables required (defaults to 1 if this is not called)

    QString getOriginalVariableName( unsigned int variableIndex );          /// Return variable name prior to any macro substitutions. (eg, SR$SECTOR$V )
    QString getVariableNameSubstitutions();                                 /// Return macro substitutions used for variable names (eg, SECTOR=01,V=PRESURE)
    QString getSubstitutedVariableName(unsigned int variableIndex );        /// Return variable name after macro substitutions

    void initialiseVariableNameSubstitutions( const QString& substitutions );   /// Initialise macro substitutions. May be used on creation.

    virtual void establishConnection( unsigned int variableIndex ) = 0;     /// Create a CA connection and initiates updates if required

    void setVariableNameSubstitutionsOverride( const QString& substitutions );

    void setVariableName( const QString& variableName, unsigned int variableIndex );    /// Accept a new variable name which may include substitution keys preceeded by $
    void setVariableNameSubstitutions( const QString& substitutions );                  /// Accept a new set of macro substitutions in the form KEY1=VALUE1,KEY2=VALUE2

  private:
    QString doSubstitution( unsigned int variableIndex );
    QString standardizeSubs( const QString &subsIn );

    QString macroSubstitutions;
    QString macroSubstitutionsOverride;

    QList<QString> variableNames;
};

#endif // VARIABLENAMEMANAGER_H
