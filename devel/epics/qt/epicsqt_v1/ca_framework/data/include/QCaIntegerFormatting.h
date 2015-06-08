/*! 
  \class QCaIntegerFormatting
  \version $Revision: #4 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief Provides textual formatting for QCaInteger data.
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

#ifndef QCAINTEGERFORMATTING_H
#define QCAINTEGERFORMATTING_H

#include <QString>
#include <QVariant>
#include <Generic.h>

class QCaIntegerFormatting {

public:

    // Construction
    QCaIntegerFormatting();

    //===============================================
    // Main functions of this class:
    //   - Format a double based on a value
    //   - Translate a double and generate a value
    //===============================================
    long formatInteger( const QVariant &value );
    QVariant formatValue( const long &integerValue, generic::generic_types valueType );

    // Functions to configure the formatting
    void setRadix( unsigned int radix );

    // Functions to read the formatting configuration
    unsigned int getPrecision();
    unsigned int getRadix();

  private:
    // Type specific conversion functions
    long formatFromFloating( const QVariant &value );
    long formatFromInteger( const QVariant &value );
    long formatFromUnsignedInteger( const QVariant &value );
    long formatFromString( const QVariant &value );
    long formatFromTime( const QVariant &value );

    // Error reporting
    long formatFailure( QString message );

    // Formatting configuration
    int radix; /// Positional base system to display data.
};

#endif // QCAINTEGERFORMATTING_H
