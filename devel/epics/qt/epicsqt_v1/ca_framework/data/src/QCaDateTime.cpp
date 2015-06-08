/*!
  \class QCaDatetime
  \version $Revision: #4 $
  \date $DateTime: 2010/08/30 16:37:08 $
  \author andrew.rhyder
  \brief CA Date Time manager
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

#include <QString>
#include <QTextStream>
#include <QCaDateTime.h>
#include <QDebug>

/*!
  Construct an empty QCa date time
 */
QCaDateTime::QCaDateTime() {
    nSec = 0;
}

/*!
  Construct a QCa date time set to the same date/time as a conventional QDateTime
 */
QCaDateTime::QCaDateTime( QDateTime dt ) : QDateTime( dt ) {
    nSec = 0;
}

/*!
  Construct a QCa date time set to the same date/time as an EPICS time stamp
 */
QCaDateTime::QCaDateTime( unsigned long seconds, unsigned long nanoseconds ) {
    // Static count of the number of seconds between the Qt base time used when specifying
    // seconds since a base time (1/1/1970) and the EPICS base time (1/1/1990).
    static unsigned long EPICSQtEpocOffset = QDate( 1970, 1, 1 ).daysTo( QDate( 1990, 1, 1 ) ) *60 *60 *24;

    // Set the time to the second
    // Note, although the EPICS time stamp is in seconds since a base, the
    // method which takes seconds since a base time uses a different base, so an offset is added.
    setTime_t( seconds + EPICSQtEpocOffset );

    // Add the nanoseconds. Down to the millisecond goes in the Qt structure,
    // the remaining nanoseconds are saved in this class
    addMSecs( nanoseconds / 1000000 );
    nSec = nanoseconds % 1000000;
}

/*!
  Copy a QCaDateTime from another
 */
void QCaDateTime::operator=( const QCaDateTime& other )
{
    setDate( other.date() );
    setTime( other.time() );
    nSec = other.nSec;
}

/*!
  Returns a string which represents the date and time
 */
QString QCaDateTime::text() {

    // Format the date and time to millisecond resolution
    QString out;
    out = toString( QString( "yyyy-MM-dd hh:mm:ss.zzz" ));

    // Add down to nanosecond resolution
    QTextStream s( &out );
    s.setFieldAlignment( QTextStream::AlignRight );
    s.setPadChar( '0' );
    s.setFieldWidth( 6 );
    s << nSec;

    return out;
}

/*!
  Returns a double which represents the date and time in seconds (to mS resolution) to the base time
 */
double QCaDateTime::floating( QDateTime base ) {

    int days = base.date().daysTo( date() );
    int mSecs = base.time().msecsTo( time() );

    return (double)days + (double)mSecs / 1000;
}
