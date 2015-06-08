/*! 
  \class Monitor
  \version $Revision: #5 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief Qt-CA based version of CA Monitor.
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

#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>
#include <QCaString.h>

/*!
  Class to monitor a PV. All data updats and messages are timestamped and written to std out
  */
class monitor : public QObject
{
      Q_OBJECT

public:
    monitor( QString pvIn );

private:
    QCaString* source;
    UserMessage messages;
    QCaStringFormatting formatting;
    QString pv;
    QTextStream* stream;

private slots:
    void connectionChanged( QCaConnectionInfo& );
    void log( const QString& data, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp, const unsigned int & );
    void message( const QString& );

};

#endif // MONITOR_H
