/*!
  \class QCaEventFilter
  \version $Revision: #5 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief Manage CA alarm and severity information
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

#ifndef QCAALARMINFO_H
#define QCAALARMINFO_H

#include <QString>

#define QCAALARMINFO_SEVERITY unsigned short

class QCaAlarmInfo
{
public:
    QCaAlarmInfo();
    QCaAlarmInfo( unsigned short statusIn, unsigned short severityIn );

    QString statusName();       // Return the name of the current alarm state
    QString severityName();     // Return the name of the current alarm severity
    bool isInAlarm();           // Return true if there is an alarm
    bool isMinor();             // Return true if there is a minor alarm
    bool isMajor();             // Return true if there is a major alarm
    bool isInvalid();           // Return true if there is an invalid alarm
    QString style();            // Return a style string to update the widget's look to reflect the current alarm state
    static QCAALARMINFO_SEVERITY getInvalidSeverity(); // Return a severity that will not match any valid severity
    QCAALARMINFO_SEVERITY getSeverity(); // Return the current severity

private:
    unsigned short status;      // Alarm state
    unsigned short severity;    // Alarm severity


};

#endif // QCAALARMINFO_H
