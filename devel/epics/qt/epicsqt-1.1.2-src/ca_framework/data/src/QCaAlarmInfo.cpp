/*!
  \class QCaDatetime
  \version $Revision: #6 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief CA alarm info manager
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

#include <alarm.h>
#include <QCaAlarmInfo.h>

/*!
  Construct an empty instance.
  By default there is no alarm present.
 */
QCaAlarmInfo::QCaAlarmInfo()
{
    status = NO_ALARM;
    severity = NO_ALARM;
}

/*!
  Construct an instance given an alarm state and severity
 */
QCaAlarmInfo::QCaAlarmInfo( unsigned short statusIn, unsigned short severityIn ) {
    status = statusIn;
    severity = severityIn;
}

/*!
  Return a string identifying the alarm state
 */
QString QCaAlarmInfo::statusName() {

    if( status <= lastEpicsAlarmCond )
        return QString( epicsAlarmConditionStrings[status] );
    else
        return QString( "" );
}

/*!
  Return a string identifying the alarm severity
 */
QString QCaAlarmInfo::severityName() {

    if( severity <= lastEpicsAlarmSev )
        return QString( epicsAlarmSeverityStrings[severity] );
    else
        return QString( "" );
}

/*!
  Return true if there is an alarm
 */
bool QCaAlarmInfo::isInAlarm() {
    return( status != NO_ALARM );
}

/*!
  Return true if there is a minor alarm
 */
bool QCaAlarmInfo::isMinor() {
    return( severity == MINOR_ALARM );
}

/*!
  Return true if there is a major alarm
 */
bool QCaAlarmInfo::isMajor() {
    return( severity == MAJOR_ALARM );
}

/*!
  Return true if there is an invalid alarm
 */
bool QCaAlarmInfo::isInvalid() {
    return( severity == INVALID_ALARM );
}

/*!
  Return a style string to update the widget's look to reflect the current alarm state
 */
QString QCaAlarmInfo::style()
{
    switch( severity )
    {
        case NO_ALARM:      return "";
        case MINOR_ALARM:   return "QWidget { background-color: #ffff80; }";
        case MAJOR_ALARM:   return "QWidget { background-color: #ff8080; }";
        case INVALID_ALARM: return "QWidget { background-color: #ffffff; }";
        default:            return "";
    }
}

/*!
  Return a severity that will not match any valid severity
 */
QCAALARMINFO_SEVERITY QCaAlarmInfo::getInvalidSeverity()
{
    return ALARM_NSEV;
}

/*!
  Return the severity
  The caller is only expected to compare this to
 */
QCAALARMINFO_SEVERITY QCaAlarmInfo::getSeverity()
{
    return severity;
}
