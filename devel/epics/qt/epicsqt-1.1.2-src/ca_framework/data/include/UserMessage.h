/*! 
  \class UserMessage
  \version $Revision: #4 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief User message manager.
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

#ifndef UserMessage_H
#define UserMessage_H

#include <QObject>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT UserMessage : public QObject
{
    Q_OBJECT

    public:
        UserMessage();
        ~UserMessage();

    void setup( QObject* statusMessageConsumer,                 // Setup the signal / slot connections required to present messages to the user
                QObject* warningMessageConsumer,
                QObject* errorMessageConsumer );

    void setup( QObject* generalMessageConsumer );              // Setup the signal / slot connections required to present messages to the user

    void sendStatusMessage( QString message );                  // Send a status message to the user
    void sendStatusMessage( QString message, QString source );  // Send a status message to the user including a string identifying the source of the message

    void sendWarningMessage( QString message );                 // Send a warning message to the user
    void sendWarningMessage( QString message, QString source ); // Send a warning message to the user including a string identifying the source of the message

    void sendErrorMessage( QString message );                   // Send an error message to the user
    void sendErrorMessage( QString message, QString source );   // Send an error message to the user including a string identifying the source of the message

signals:
    void statusMessage( QString message );                      // Signal a status message
    void warningMessage( QString message );                     // Signal a warning message
    void errorMessage( QString message );                       // Signal an error message

    void generalMessage( QString message );                     // Signal a message (emitted for status, warning, and error
  };

#endif // UserMessage_H
