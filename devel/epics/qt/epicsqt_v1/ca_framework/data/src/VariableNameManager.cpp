/*! 
  \class VariableNameManager
  \version $Revision: #4 $
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

/*
 * Description:
 *
 * A class to manage changes to one or more variable names and the macro 
 * substitutions applying to the variable names. This class calls the virtual
 * function establishConnection() whenever a translated variable name is updated.
 *
 */

#include <VariableNameManager.h>
#include <ContainerProfile.h>
#include <QStringList>
#include <QtDebug>

/*!
    Assume one variable name.
*/
VariableNameManager::VariableNameManager() {

    // Assume one variable name.
    variableNameManagerInitialise( 1 );
}

/*!
    Define the required number of variables to manage.
    The list of variable names is initially conatins a single variable name so this
    need only be called if more than one variable name is required.
*/
void VariableNameManager::variableNameManagerInitialise( unsigned int numVariables ) {

    // Sanity check. List must contain at least one variable name
    if( numVariables < 1 )
        return;

    /// Clear out any existing variables
    variableNames.clear();

    // Create the required number of variables
    for( unsigned int i = 0; i < numVariables; i++ ) {
        variableNames.append( QString() );
    }
}

/*!
    Get the current variable name.
    Supply a variable index if this class is managing more than one variable
    name.
*/
QString VariableNameManager::getOriginalVariableName( unsigned int variableIndex ) {

    // Sanity check
    if( variableIndex >= (unsigned int )variableNames.size() )
        return "";

    // Return the original variable name
    return variableNames[variableIndex];
}

/*!
    Get the current variable name substitutions.
    Note the substitutions for the first variable are always returned as
    the same substitutions are used for every entry in the variableNames list.
*/
QString VariableNameManager::getVariableNameSubstitutions() {

    return macroSubstitutions;
}

/*!
    Get the current variable name with substitutions applied.
*/
QString VariableNameManager::getSubstitutedVariableName( unsigned int variableIndex ) {

    // Sanity check
    if( variableIndex >= (unsigned int )variableNames.size() )
        return "";

    // Perform the substitution
    return doSubstitution( variableIndex );
}

/*!
    Override variable name substitutions.
    This is called when any macro substitutions set by default are overridden by the creator.
*/
void VariableNameManager::setVariableNameSubstitutionsOverride( const QString& macroSubstitutionsOverrideIn ) {

    macroSubstitutionsOverride = macroSubstitutionsOverrideIn;
}

/*!
    Set the variable name.
    Macro substitution will be performed.
    A new connection is established.
*/
void VariableNameManager::setVariableName( const QString& variableNameIn, unsigned int variableIndex ) {

    // Sanity check
    if( variableIndex >= (unsigned int )variableNames.size() )
        return;

    /// Save the variable name and request the variableName data if updates are required
    variableNames[variableIndex] = variableNameIn;
}

/*!
    Set the variable name substitutions.
    Note, if there is more than one variable name in the list, the same
    substitutions are used for every entry in the variableNames list.
    Macro substitution will be performed.
    A new connection is established.
*/
void VariableNameManager::setVariableNameSubstitutions( const QString& macroSubstitutionsIn ) {

    macroSubstitutions = macroSubstitutionsIn;
}

/*!
    Perform a set of substitutions throughout the string.
    Replace $MACRO1 with VALUE1, $MACRO2 with VALUE2, etc wherever they appear in the string.
*/
QString VariableNameManager::doSubstitution( unsigned int variableIndex ) {

    // Sanity check
    if( variableIndex >= (unsigned int )variableNames.size() )
        return "";

    /// Start with the initial string
    QString result = variableNames[variableIndex];

    /// Generate a list where each item in the list is a single substitution in the form MACRO1=VALUE1
    QString subs;
    subs.append( macroSubstitutionsOverride ).append( " " ).append( macroSubstitutions );
    subs = standardizeSubs( subs );
    QStringList subList = subs.split( "," );

    /// Perform all substitutions
    QStringList::const_iterator subsIndex;
    for ( subsIndex = subList.constBegin(); subsIndex != subList.constEnd(); ++subsIndex ) {
        /// Get the key to search for and the value to replace it with.
        /// If both a key and value are present (and nothing more) perform the substitution.
        /// Note, the key will only be replaced if it is prefixed with a $.
        QStringList subList = ( *subsIndex ).split( "=" );
        if( subList.size() == 2 ) {
            QString before( "$(" );
            before.append( subList[0] ).append( ")" );

            QString after = subList[1];

            result = result.replace( before, after );
        }
    }
    return result;
}

/*!
    Standardize the substitution string to make parsing it easier.
    Remove all white space except white space seperating key/value pairs.
    Replace any white space between white space seperating key/value pairs with a comma.
*/
QString VariableNameManager::standardizeSubs( const QString &subsIn ) {
    /// Standardise the string to the form $MACRO1=VALUE1,$MACRO2=VALUE2
    /// (no spaces, each substitution seperated by a comma)
    /// This allows the following forms:
    ///     $MACRO1=VALUE1,$MACRO2=VALUE2
    ///     $MACRO1=VALUE1 $MACRO2=VALUE2
    ///     $MACRO1=VALUE1 , $MACRO2=VALUE2
    ///     $MACRO1 = VALUE1,$MACRO2 = VALUE2
    QString std = subsIn.simplified();
    std = std.replace( " =", "=" );
    std = std.replace( "= ", "=" );
    std = std.replace( " ,", "," );
    std = std.replace( ", ", "," );
    std = std.replace( " ", "," );

    return std;
}
