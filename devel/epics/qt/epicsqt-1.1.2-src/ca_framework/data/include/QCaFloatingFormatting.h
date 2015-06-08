/*! 
  \class QCaFloatingFormatting
  \version $Revision: #4 $
  \date $DateTime: 2010/08/30 16:37:08 $
  \author andrew.rhyder
  \brief Provides textual formatting for QCaFloating data.
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

#ifndef QCAFLOATINGFORMATTING_H
#define QCAFLOATINGFORMATTING_H

#include <QString>
#include <QVariant>
#include <Generic.h>

class QCaFloatingFormatting {
  public:

    // Formatting enumerations
    enum formats { FORMAT_e = 'e',   // format as [-]9.9e[+|-]999
                   FORMAT_E = 'E',   // format as [-]9.9E[+|-]999
                   FORMAT_f = 'f',   // format as [-]9.9
                   FORMAT_g = 'g',   // use e or f format, whichever is the most concise
                   FORMAT_G = 'G' }; // use E or f format, whichever is the most concise


    // Construction
    QCaFloatingFormatting();

    //===============================================
    // Main functions of this class:
    //   - Format a double based on a value
    //   - Translate a double and generate a value
    //===============================================
    double formatFloating( const QVariant &value );
    QVector<double> formatFloatingArray( const QVariant &value );
    QVariant formatValue( const double &floatingValue, generic::generic_types valueType );

    // Functions to configure the formatting
    void setPrecision( unsigned int precision );
    void setFormat( formats format );

    // Functions to read the formatting configuration
    unsigned int getPrecision();
    int getFormat();

  private:
    // Private functions to read the formatting configuration
    char getFormatChar();

    // Type specific conversion functions
    double formatFromFloating( const QVariant &value );
    double formatFromInteger( const QVariant &value );
    double formatFromUnsignedInteger( const QVariant &value );
    double formatFromString( const QVariant &value );
    double formatFromTime( const QVariant &value );

    // Common functions
    double formatFloatingNonArray( const QVariant &value );

    // Error reporting
    double formatFailure( QString message );

    // Formatting configuration
    unsigned int precision;
    formats format;
};

#endif // QCAFLOATINGFORMATTING_H
