/*! 
  \class QCaInteger
  \version $Revision: #5 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief Integer specific wrapper for QCaObject.
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

#include <QCaInteger.h>
#include <QtDebug>

/*!
    ???
*/
QCaInteger::QCaInteger( QString recordName, QObject *eventObject,
                        QCaIntegerFormatting *integerFormattingIn,
                        unsigned int variableIndexIn ) : QCaObject( recordName, eventObject ) {
    initialise( integerFormattingIn, variableIndexIn );
}
QCaInteger::QCaInteger( QString recordName, QObject *eventObject,
                        QCaIntegerFormatting *integerFormattingIn,
                        unsigned int variableIndexIn, UserMessage* userMessageIn ) : QCaObject( recordName, eventObject, userMessageIn ) {
    initialise( integerFormattingIn, variableIndexIn );
}

/*!
    Stream the QCaObject data through this class to generate integer data updates
*/
void QCaInteger::initialise( QCaIntegerFormatting* integerFormattingIn,
                             unsigned int variableIndexIn ) {
    integerFormat = integerFormattingIn;
    variableIndex = variableIndexIn;

    QObject::connect( this, SIGNAL( dataChanged( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ),
                      this, SLOT( convertVariant( const QVariant&, QCaAlarmInfo&, QCaDateTime& ) ) );
}

/*!
    Take a new integer value and write it to the database.
    The type of data formatted (text, floating, integer, etc) will be determined by the record data type,
    How the integer is parsed will be determined by the integer formatting. For example, integer to string may require always including a sign.
*/
void QCaInteger::writeInteger( const long &data ) {
    writeData( integerFormat->formatValue( data, getDataType() ) );
}

/*!
    Slot to recieve data updates from the base QCaObject and generate integer updates.
*/
void QCaInteger::convertVariant( const QVariant &value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp ) {
    emit integerChanged( integerFormat->formatInteger( value ), alarmInfo, timeStamp, variableIndex );
}
