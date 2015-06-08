/*! 
  \class QCaObject
  \version $Revision: #10 $
  \date $DateTime: 2010/09/06 13:16:04 $
  \author anthony.owen
  \brief Provides channel access to QT.
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
 *    Anthony Owen
 *  Contact details:
 *    anthony.owen@gmail.com
 */

#ifndef QCAOBJECT_H
#define QCAOBJECT_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <QTimer>
#include <QString>
#include <QStringList>

#include <CaObject.h>
#include <QCaStateMachine.h>
#include <QCaEventFilter.h>
#include <QCaEventUpdate.h>
#include <UserMessage.h>
#include <QCaAlarmInfo.h>
#include <QCaDateTime.h>
#include <QCaConnectionInfo.h>
#include <QCaPluginLibrary_global.h>

namespace qcaobject {

  class QCAPLUGINLIBRARYSHARED_EXPORT QCaObject : public QObject, caobject::CaObject {
      Q_OBJECT

    public:
      QCaObject( const QString& recordName, QObject *eventObject );
      QCaObject( const QString& recordName, QObject *eventObject, UserMessage* userMessageIn );
      ~QCaObject();

      bool subscribe();
      bool singleShotRead();


      static void processEventStatic( QCaEventUpdate* dataUpdateEvent );

      bool dataTypeKnown();

      /// State machine access functions
      bool createChannel();
      void deleteChannel();
      bool createSubscription();
      bool getChannel();
      bool putChannel();
      bool isChannelConnected();
      void startConnectionTimer();
      void stopConnectionTimer();

      void setUserMessage( UserMessage* userMessageIn );

      void enableWriteCallbacks( bool enable );
      bool isWriteCallbacksEnabled();

      // Get database information relating to the variable
      QString getEgu();
      QStringList getEnumerations();
      unsigned int getPrecision();
      double getDisplayLimitUpper();
      double getDisplayLimitLower();
      double getAlarmLimitUpper();
      double getAlarmLimitLower();
      double getWarningLimitUpper();
      double getWarningLimitLower();
      double getControlLimitUpper();
      double getControlLimitLower();

    signals:
      void dataChanged( const QVariant& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp );
      void connectionChanged( QCaConnectionInfo& connectionInfo );

    public slots:
      bool writeData( const QVariant& value );
      void resendLastData();

    protected:
      generic::generic_types getDataType();

    private:
      void initialise( const QString& newRecordName, QObject *newEventHandler, UserMessage* userMessageIn );

      long lastEventChannelState; /// Channel state from most recent update event. This is actually of type caconnection::channel_states
      long lastEventLinkState;    /// Link state from most recent update event. This is actually of type aconnection::link_states

      QString recordName;
      QVariant writingData;

      QObject* eventHandler;                  /// Event handler
      static QMutex pendingEventsLock;        /// Used to protect access to pendingEvents list
      static QCaEventFilter eventFilter;      /// Event filter to filter in own events
      QList<QCaEventItem> pendingEvents;      /// List of pending data events
      QCaEventItem* lastDataEvent;            /// Outstanding data event
      QTimer setChannelTimer;

      bool removeEventFromPendingList( QCaEventUpdate* dataUpdateEvent );

      qcastatemachine::ConnectionQCaStateMachine *connectionMachine;
      qcastatemachine::SubscriptionQCaStateMachine *subscriptionMachine;
      qcastatemachine::ReadQCaStateMachine *readMachine;
      qcastatemachine::WriteQCaStateMachine *writeMachine;

      void signalCallback( caobject::callback_reasons reason );  /// CA callback function processed within an EPICS thread
      void processEvent( QCaEventUpdate* dataUpdateEvent );      /// Continue processing CA callback but within the contect of a Qt event
      void processData( void* newData );                         /// Process new CA data. newData is actually of type carecord::CaRecord*

      UserMessage* userMessage;

      // Last data emited
      QCaDateTime  lastTimeStamp;
      QCaAlarmInfo lastAlarmInfo;
      QVariant     lastValue;

      // Database information relating to the variable
      QString egu;
      int precision;

      double displayLimitUpper;
      double displayLimitLower;

      double alarmLimitUpper;
      double alarmLimitLower;

      double warningLimitUpper;
      double warningLimitLower;

      double controlLimitUpper;
      double controlLimitLower;

      QStringList enumerations;
      bool isStatField;


    private slots:
      void setChannelExpired();
  };

}

#endif /// QCAOBJECT_H
