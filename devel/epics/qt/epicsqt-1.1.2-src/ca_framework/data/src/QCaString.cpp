/*! 
  \class QCaString
  \version $Revision: #7 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief String wrapper for QCaObject variant data.
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

#include <QCaString.h>

/*!

*/
QCaString::QCaString( QString recordName, QObject* eventObject,
                      QCaStringFormatting* newStringFormat,
                      unsigned int variableIndexIn ) : QCaObject( recordName, eventObject ) {
    initialise( newStringFormat, variableIndexIn );
}

QCaString::QCaString( QString recordName, QObject* eventObject,
                      QCaStringFormatting* newStringFormat,
                      unsigned int variableIndexIn, UserMessage* userMessageIn ) : QCaObject( recordName, eventObject, userMessageIn ) {
    initialise( newStringFormat, variableIndexIn );
}
/*!
    Stream the QCaObject data through this class to generate textual data
    updates.
*/
void QCaString::initialise( QCaStringFormatting* newStringFormat, unsigned int variableIndexIn ) {
    stringFormat = newStringFormat;
    variableIndex = variableIndexIn;

    QObject::connect( this, SIGNAL( dataChanged( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ),
                      this, SLOT( convertVariant( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ) );
}

/*!
    Take a new text value and write it to the database.
    The type of data formatted (text, floating, integer, etc) will be determined by the record data type,
    How the text is parsed will be determined by the string formatting.
    For example, assume the record is a floating point record and the text is formatted as an integer.
    The string is parsed as in integer (123 is OK, 123.456 would fail), then converted to a floating point number.
    The above example is pedantic
    if the string formatting
*/
void QCaString::writeString( const QString &data ) {
    writeData( stringFormat->formatValue( data ) );
}

/*!
    Take a new value from the database and emit a string,formatted
    as directed by the set of formatting information held by this class
*/
void QCaString::convertVariant( const QVariant& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp ) {

    // Set up variable details used by some formatting options
    stringFormat->setDbEgu( getEgu() );
    stringFormat->setDbEnumerations( getEnumerations() );
    stringFormat->setDbPrecision( getPrecision() );

    // Format the data and send it
    emit stringChanged( stringFormat->formatString( value ), alarmInfo, timeStamp, variableIndex );
}
