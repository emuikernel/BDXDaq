/*! 
  \class QCaObject
  \version $Revision: #15 $
  \date $DateTime: 2010/08/30 16:37:08 $
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

#include <QtCore>
#include <QtDebug>

#include <CaObjectPrivate.h>
#include <CaObject.h>
#include <QCaObject.h>
#include <QCaEventUpdate.h>
#include <CaRecord.h>
#include <CaConnection.h>

using namespace qcaobject;
using namespace generic;
using namespace caconnection;

//! Used to protect access to outstandingEvents list
QMutex QCaObject::pendingEventsLock;

//! An event filter for processing data updates in a Qt thread.
QCaEventFilter QCaObject::eventFilter;

/*!
   The event object can be any Qt object with an event queue.
   A filter will be inserted (and removed) by this class to catch
   events from this class and pass them back to this class for processing.
   In other words, the event object does not need to be set up in any way.
   It just need to have a suitable event loop running.
*/
QCaObject::QCaObject( const QString& newRecordName, QObject *newEventHandler ) {
    initialise( newRecordName, newEventHandler, NULL );
}

QCaObject::QCaObject( const QString& newRecordName, QObject *newEventHandler, UserMessage* userMessageIn ) {
    initialise( newRecordName, newEventHandler, userMessageIn );
}

void QCaObject::initialise( const QString& newRecordName, QObject *newEventHandler, UserMessage* userMessageIn ) {

    // Initialise variables
    precision = 0;

    displayLimitUpper = 0;
    displayLimitLower = 0;

    alarmLimitUpper = 0;
    alarmLimitLower = 0;

    warningLimitUpper = 0;
    warningLimitLower = 0;

    controlLimitUpper = 0;
    controlLimitLower = 0;

    isStatField = false;

    lastTimeStamp = QCaDateTime( QDateTime::currentDateTime() );
//    lastAlarmInfo = ???;
    lastValue = (double)0.0;

    // Setup any the mechanism to handle messages to the user, if supplied
    setUserMessage( userMessageIn );

    // Note the record required
    recordName = newRecordName;

    // Note if the field the value represents is a STAT field.
    // This is important when formatting strings as only a maximum of 16 enumerated strings
    // are available from the database yet a STAT field has 22 defined states.
    QString statName( ".STAT" );
    if( recordName.right( statName.length() ) == statName )
        isStatField = true;

    /// Set initial states of the connection and link as reported by the event system.
    lastEventChannelState = caconnection::NEVER_CONNECTED;
    lastEventLinkState = caconnection::LINK_DOWN;

    /// This object will post events to itself to transfer processing to a Qt aware thread via this filter.
    eventHandler = newEventHandler;
    eventFilter.addFilter( eventHandler );

    connectionMachine = new qcastatemachine::ConnectionQCaStateMachine( this );
    subscriptionMachine = new qcastatemachine::SubscriptionQCaStateMachine( this );
    readMachine = new qcastatemachine::ReadQCaStateMachine( this );
    writeMachine = new qcastatemachine::WriteQCaStateMachine( this );

    QObject::connect( &setChannelTimer, SIGNAL( timeout() ), this, SLOT( setChannelExpired() ) );
    setChannelTimer.stop();
    /// Start/request connecting state
    connectionMachine->process( qcastatemachine::CONNECTED );
}

/*!
    Destructor. Remove the channel and ensure that any unprocessed events posted by this obect will be ignored when
    they pop out of the event queue. Also, remove the event filter if this is the last QCaObject.
*/
QCaObject::~QCaObject() {
    /// Prevent channel access callbacks.
    /// There will be no more callbacks from CaObject after this call returns.
    /// Without this, a callback could occur while the outstanding events list contents
    /// is being marked as 'to be ignored' (below). While access to the list is thread safe, this would
    /// result in an active event in the event queue which would be cause this QCaObject to be accessed
    /// after deletion.
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;
    p->removeChannel();

    /// Protect access to pending events list
    QMutexLocker locker( &pendingEventsLock );

    /// Ensure processing of outstanding events will not access this QCaObject after deletion.
    /// If an event has been posted by a QCaObject, and the QCaObject is deleted before
    /// the event is processed, the event will still be processed if the event filter is
    /// still in place - and the filter will still be there if any other QCaObjects are using
    /// the same QObject to process events. In this case the event will reference a QCaObject
    /// which no longer exists.
    /// To manage this problem outstanding events are marked 'to be ignored'.
    for( int i = 0; i < pendingEvents.size(); i++ ) {
        pendingEvents[i].event->acceptThisEvent = false;
        pendingEvents[i].event->emitterObject = NULL;   /// Ensure a 'nice' crash if referenced in error
    }
    pendingEvents.clear();

    /// Remove the event filter for this QCaObject.
    /// Note, this only removes the filter if this is the last QCaObject to use the event loop for 'eventObject'.
    /// If this is not the last QCaObject to use the event loop for 'eventObject' any remaining events will still
    /// be processed as the event filter remains to handle events for other QCaOjects. The outstanding events are,
    /// however, now safe.
    eventFilter.deleteFilter( eventHandler );
}

/*!
    Subcribe
*/
bool QCaObject::subscribe() {

    return subscriptionMachine->process( qcastatemachine::SUBSCRIBED );
}

/*!
    Initiate a single shot read
*/
bool QCaObject::singleShotRead() {

    return readMachine->process( qcastatemachine::READING );
}

/*!
   Process self generated events and only accept them if the
   originating QCaObject still exists.
   Note, this is a static method
*/
void QCaObject::processEventStatic( QCaEventUpdate* dataUpdateEvent )
{
    bool eventValid;
    eventValid = false;

    { // Limit scope of pending event list lock

        // Protect access to the pending events list
        QMutexLocker locker( &pendingEventsLock );

        // If the originating object still exists, remove the event from it's list of pending events
        if( dataUpdateEvent->acceptThisEvent == true )
        {
            // Remove the event from the list of pending events.
            eventValid = dataUpdateEvent->emitterObject->removeEventFromPendingList( dataUpdateEvent );
        }

    } // Pending event list unlocked here

    // If the originating object still exists, process the event
    if( eventValid && dataUpdateEvent->acceptThisEvent == true )
    {
        dataUpdateEvent->emitterObject->processEvent( dataUpdateEvent );
    }
}

/**
   Remove an event from the pending event list.
   If the event can't be found log an error and return false indicating 'the event is suspect - don't use it'.
   The list must be locked (using pendingEventsLock) prior to calling this method
*/
bool QCaObject::removeEventFromPendingList( QCaEventUpdate* dataUpdateEvent )
{
    /// If list is empty, something is wrong - report it
    if( pendingEvents.isEmpty() )
    {
        QString msg( recordName );
        if( userMessage )
        {
            QString msg( recordName );
            userMessage->sendErrorMessage( msg.append( " Outstanding events list is empty. It should contain at least one event"), "QCaObject::processEvent()"  );
        }
        return false;
    }

    /// If the first item in the list is not the current event, something is wrong - report it
    if( pendingEvents[0].event != dataUpdateEvent )
    {
        QString msg( recordName );
        if( userMessage )
        {
            QString msg( recordName );
            userMessage->sendErrorMessage( msg.append( " Outstanding events list is corrupt. The first event is not the event being processed" ), "QCaObject::processEvent()" );
        }
        return false;
    }

    /// Remove this event from the list
    pendingEvents.removeFirst();
    return true;
}

/*!
    Returns true if the type of data being read or to be written is known.
    The data type will be unknown until a connection is established
*/
bool QCaObject::dataTypeKnown() {
    return( ( getType() != generic::UNKNOWN ) ? true:false );
}

/*!
    Create a channel
*/
bool QCaObject::createChannel() {

    // Get the private part of this object (not visible to users of this class)
    CaObjectPrivate* p = (CaObjectPrivate*)(priPtr);

    // Try to create the channel
    caconnection::ca_responses response = p->setChannel( recordName.toStdString() );
    if( response == caconnection::REQUEST_SUCCESSFUL )
    {
        return true;
    }
    else if( userMessage )
    {
        QString msg( recordName );
        msg.append( " Create channel failed." );
        switch( response )
        {
            case CHANNEL_DISCONNECTED: msg.append( " Channel disconnected."); break;
            case REQUEST_FAILED:       msg.append( " Request failed.");       break;
            default:                   msg.append( " Unknown error");         break;
        }
        userMessage->sendErrorMessage( msg, "QCaObject::createChannel()"  );
        return false;
    }
    return false;
}

/*!
    Delete a channel
*/
void QCaObject::deleteChannel() {

    // Get the private part of this object (not visible to users of this class)
    CaObjectPrivate* p = (CaObjectPrivate*)(priPtr);

    // Delete the channel
    p->removeChannel();
}

/*!
    Create a subscription
*/
bool QCaObject::createSubscription() {

    // Get the private part of this object (not visible to users of this class)
    CaObjectPrivate* p = (CaObjectPrivate*)(priPtr);

    // Try to start a subscription
    caconnection::ca_responses response = p->startSubscription();
    if( response == caconnection::REQUEST_SUCCESSFUL )
    {
        return true;
    }
    else if( userMessage )
    {
        QString msg( recordName );
        msg.append( " Create subscription failed." );
        switch( response )
        {
            case CHANNEL_DISCONNECTED: msg.append( " Channel disconnected."); break;
            case REQUEST_FAILED:       msg.append( " Request failed.");       break;
            default:                   msg.append( " Unknown error");         break;
        }
        userMessage->sendErrorMessage( msg, "QCaObject::createSubscription()"  );
        return false;
    }
    return false;
}

/*!
    ???
*/
bool QCaObject::getChannel() {

    // Get the private part of this object (not visible to users of this class)
    CaObjectPrivate* p = (CaObjectPrivate*)(priPtr);

    // ???
    caconnection::ca_responses response = p->readChannel();
    if( response == caconnection::REQUEST_SUCCESSFUL )
    {
        return true;
    }
    else if( userMessage )
    {
        QString msg( recordName );
        msg.append( " Get channel failed." );
        switch( response )
        {
            case CHANNEL_DISCONNECTED: msg.append( " Channel disconnected."); break;
            case REQUEST_FAILED:       msg.append( " Request failed.");       break;
            default:                   msg.append( " Unknown error");         break;
        }
        userMessage->sendErrorMessage( msg, "QCaObject::getChannel()"  );
        return false;
    }
    return false;
}

/*!
    ???
*/
bool QCaObject::putChannel() {

    // Generate the output data based on the type
    Generic outputData;
    switch( writingData.type() ) {
        case QVariant::Double :
            outputData.setDouble( writingData.toDouble() );
        break;
        case QVariant::LongLong :
            outputData.setUnsignedLong( writingData.toLongLong() );
        break;
        case QVariant::UInt :
        case QVariant::ULongLong :
            outputData.setUnsignedLong( writingData.toULongLong() );
        break;
        case QVariant::String :
            outputData.setString( writingData.toString().toStdString() );
        break;
        default:
            return false;
        break;
    }

    // Get the CA specific part
    CaObjectPrivate* p = (CaObjectPrivate*)(priPtr);

    // Write the data
    caconnection::ca_responses response = p->writeChannel( &outputData );
    if( response == caconnection::REQUEST_SUCCESSFUL )
    {
        return true;
    }
    else if( userMessage )
    {
        QString msg( recordName );
        msg.append( " Put channel failed." );
        switch( response )
        {
            case CHANNEL_DISCONNECTED: msg.append( " Channel disconnected."); break;
            case REQUEST_FAILED:       msg.append( " Request failed.");       break;
            default:                   msg.append( " Unknown error");         break;
        }
        userMessage->sendErrorMessage( msg, "QCaObject::putChannel()"  );
        return false;
    }
    return false;
}

/*!
    ???
*/
bool QCaObject::isChannelConnected() {
    if( connectionMachine->currentState == qcastatemachine::CONNECTED ) {
        return true;
    } else {
        return false;
    }
}

/*!
    ???
*/
void QCaObject::startConnectionTimer() {
    setChannelTimer.start( 3000 );
}

/*!
    ???
*/
void QCaObject::stopConnectionTimer() {
    setChannelTimer.stop();
}

/*!
    Returns the type of data being read or to be written.
    This will be Type::UNKNOWN if a connection has never been established.
*/
generic::generic_types QCaObject::getDataType() {
    return getType();
}

/*!
    Write a data out to channel
*/
bool QCaObject::writeData( const QVariant& newData ) {
    writingData = newData;
    return writeMachine->process( qcastatemachine::WRITING );
}

/*!
    Implemetation of virtual CA callback function.
    This code is executed by an EPICS library thread. It packages data and
    posts via an event.
*/
void QCaObject::signalCallback( caobject::callback_reasons newReason ) {
///printf("my: signalCallback\n"); fflush(stdout);
    // Initialise data package.
    // It is really of type carecord::CaRecord*
    void* dataPackage = NULL;

    /// Only case where data is processed. Package the data
    if( newReason == caobject::SUBSCRIPTION_SUCCESS || newReason == caobject::READ_SUCCESS )
    {
        dataPackage = getRecordCopyPtr();
    }

    // If the callback is a data update callback, and there is an earlier, unprocessed, data event
    // of the same type in the queue, replace the data package with this one.
    // This is better than adding events faster than they can be processed.
    bool replaced = false;  // True if data replaced in earlier event
    if( dataPackage )
    { // Limit scope of pending event list lock
        QMutexLocker locker( &pendingEventsLock );
        for( int i = 0; i < pendingEvents.count(); i++ )
        {
            QCaEventUpdate* event = pendingEvents[i].event;
            if(event->reason == newReason )
            {
                delete (carecord::CaRecord*)(event->dataPtr);
                event->dataPtr = dataPackage;
                replaced = true;
                break;
            }
        }
    }

    // This is not a data update event, or there is no earlier, unprocessed, data event in the queue,
    // create a new event and post it to the event queue
    if( !replaced )
    {
        /// Package the data to be processed within the context of a Qt thread.
        QCaEventUpdate* newDataEvent = new QCaEventUpdate( this, newReason, dataPackage );

        /// Add the event to the list of pending events.
        /// A list is maintained to allow pending events to be updated 'in transit'.
        /// This must be done before posting the event as the event is likely to be processed before
        /// the postEvent call returns.
        QCaEventItem item( newDataEvent );
        { // Limit scope of pending event list lock
            QMutexLocker locker( &pendingEventsLock );
            pendingEvents.append( item );
        }

        /// Post the data to be processed within the context of a Qt thread.
        QCoreApplication::postEvent( eventHandler, newDataEvent );

    }

    // Processing will continue within the context of a Qt thread in QCaObject::processEvent() below.
}

/*!
    Process events posted from the EPICS library thread. The event is expected
    to provide snapshot of data.
    This method completes the processing of a CA callback, started in QCaObject::signalCallback() above,
    within a Qt thread
    Note,
*/
void QCaObject::processEvent( QCaEventUpdate* dataUpdateEvent ) {
///printf("my: processEvent\n"); fflush(stdout);
    // Process the event, based on the event type
    switch( dataUpdateEvent->reason ) {
        case caobject::CONNECTION_UP :
        {
            connectionMachine->active = true;
            connectionMachine->process( qcastatemachine::CONNECTED );
            subscriptionMachine->process( subscriptionMachine->requestState );
            readMachine->process( readMachine->requestState );
            writeMachine->process( writeMachine->requestState );
            break;
        }
        case caobject::CONNECTION_DOWN :
        {
            if( userMessage )
            {
                QString msg( recordName );
                userMessage->sendWarningMessage( msg.append( " Connection down" ), "QCaObject::processEvent()"  );
            }
            connectionMachine->active = false;
            connectionMachine->process( qcastatemachine::DISCONNECTED );
            subscriptionMachine->process( subscriptionMachine->requestState );
            readMachine->process( readMachine->requestState );
            writeMachine->process( writeMachine->requestState );
            break;
        }
        case caobject::SUBSCRIPTION_SUCCESS :
        {
            if( subscriptionMachine->currentState == qcastatemachine::UNSUBSCRIBED ) {
                subscriptionMachine->active = true;
                subscriptionMachine->process( qcastatemachine::SUBSCRIBED );
            } else {
                subscriptionMachine->process( qcastatemachine::SUBSCRIBED_READ );
            }
///printf("my: subscription suceess processData\n"); fflush(stdout);
            processData( (void*)(dataUpdateEvent->dataPtr) );
            break;
        }
        case caobject::SUBSCRIPTION_FAIL :
        {
            if( userMessage )
            {
                QString msg( recordName );
                userMessage->sendWarningMessage( msg.append( " Subscription failed" ), "QCaObject::processEvent()"  );
            }

            subscriptionMachine->active = false;
            subscriptionMachine->process( qcastatemachine::SUBSCRIBE_FAIL );
            break;
        }
        case caobject::READ_SUCCESS :
        {
            readMachine->active = false;
            readMachine->process( qcastatemachine::READ_IDLE );
///printf("my: readsuccess processData\n"); fflush(stdout);
            processData( (void*)(dataUpdateEvent->dataPtr) );
            break;
        }
        case caobject::READ_FAIL :
        {
            if( userMessage )
            {
                QString msg( recordName );
                userMessage->sendWarningMessage( msg.append( " Read failed" ), "QCaObject::processEvent()"  );
            }
            readMachine->active = false;
            readMachine->process( qcastatemachine::READING_FAIL );
            break;
        }
        case caobject::WRITE_SUCCESS :
        {
            writeMachine->active = false;
            writeMachine->process( qcastatemachine::WRITE_IDLE );
            break;
        }
        case caobject::WRITE_FAIL :
        {
            if( userMessage )
            {
                QString msg( recordName );
                userMessage->sendWarningMessage( msg.append( " Write failed" ), "QCaObject::processEvent()"  );
            }

            writeMachine->active = false;
            writeMachine->process( qcastatemachine::WRITING_FAIL );
            break;
        }
        case caobject::EXCEPTION :
        {
            if( userMessage )
            {
                QString msg( recordName );
                userMessage->sendErrorMessage( msg.append( " Exception" ), "QCaObject::processEvent()"  );
            }
            break;
        }
        case caobject::CONNECTION_UNKNOWN :
        {
            if( userMessage )
            {
                QString msg( recordName );
                userMessage->sendWarningMessage( msg.append( " Unknown connection" ), "QCaObject::processEvent()"  );
            }
            break;
        }
        default :
        {
            if( userMessage )
            {
                QString msg( recordName );
                userMessage->sendErrorMessage( msg.append( " Unknown CA callback" ), "QCaObject::processEvent()"  );
            }
        }
        break;
    }

    // Assume there will be no change in the channel or link
    bool connectionChange = false;

    /// If the channel state has changed, signal if the channel is connected or not
    CaObjectPrivate* p = (CaObjectPrivate*)(priPtr);
    if( p->getChannelState() != lastEventChannelState ) {
        lastEventChannelState = p->getChannelState();
        connectionChange = true;
    }

    /// If the link state has changed, signal if the link is up or not
    if( p->getLinkState() != lastEventLinkState ) {
        lastEventLinkState = p->getLinkState();
        connectionChange = true;
    }

    /// If there is a change in the connection (link of channel), signal it
    if( connectionChange )
    {
        QCaConnectionInfo connectionInfo( lastEventChannelState, lastEventLinkState );
        emit connectionChanged( connectionInfo );
    }

}

/*!
    Process new data arrivals.
    This is called when appropriate while processing an event containing CA data
*/
void QCaObject::processData( void* newDataPtr ) {


  ///  printf("my: processData\n"); fflush(stdout);
    // Recover the data record
    carecord::CaRecord* newData = (carecord::CaRecord*)newDataPtr;

    // Sanity check. Check newData is set up. newData is only set up for some
    // callback reasons and this method is also only called for some callback reasons.
    // This test is in case the two sets of reasons are not the same.
    if( !newData )
        return;

    // On the first update, gather static information for the variable
    if( isFirstUpdate() ) {

        // Note the engineering units
        egu = QString( getUnits().c_str() );

        // Note the enumeration strings
        int numEnums = getEnumStateCount();
        for( int i = 0; i < numEnums; i++ )
        {
            std::string s = getEnumState(i);
            if( s.length() )
                enumerations.append( QString( s.c_str() ) );
            else
                enumerations.append( QString::number( i ) );
        }

        // Note the precision
        precision = CaObject::getPrecision();

        // Note the display limits
        displayLimitUpper = CaObject::getDisplayUpper();
        displayLimitLower = CaObject::getDisplayLower();

        // Note the alarm limits
        alarmLimitUpper = CaObject::getAlarmUpper();
        alarmLimitLower = CaObject::getAlarmLower();

        // Note the warning limits
        warningLimitUpper = CaObject::getWarningUpper();
        warningLimitLower = CaObject::getWarningLower();

        // Note the control limits
        controlLimitUpper = CaObject::getControlUpper();
        controlLimitLower = CaObject::getControlLower();

    }

    // Package up the CA data as a Qt variant
    QVariant value;
    unsigned long arrayCount = newData->getArrayCount();
    switch( newData->getType() ) {
        case generic::STRING :
            value = QVariant( QString::fromStdString( newData->getString() ) );
        break;
        case generic::SHORT :
            value = QVariant( newData->getShort() );
        break;
        case generic::CHAR :
            value = QVariant( newData->getChar() );
        break;
        case generic::UNSIGNED_LONG :
            value = QVariant( (qlonglong) newData->getUnsignedLong() );
        break;
        case generic::DOUBLE :
            if( arrayCount <= 1 )
            {
                value = QVariant( newData->getDouble() );
            }
            else
            {
                QVariantList values;
                double* data;
                newData->getDouble( &data );

                for( unsigned long i = 0; i < arrayCount; i++ )
                {
                    values.append( data[i] );
                }
                value = QVariant( values );
            }
        break;
        case generic::UNKNOWN :
            value = QVariant();
        break;
    }

    // Build the alarm infomation (alarm state and severity)
    QCaAlarmInfo alarmInfo( getAlarmStatus(), getAlarmSeverity() );

    // Build the time stamp (Note, a QCaDateTime is a QDateTime with nanoseconds past the millisecond)
    // Use current time if no EPICS timestamp is available
    QCaDateTime timeStamp;
    unsigned long timeStampSeconds = 0;
    if( ( timeStampSeconds = getTimeStampSeconds() ) != 0 )
    {
        QCaDateTime caTimeStamp( timeStampSeconds, getTimeStampNanoseconds() );
        timeStamp = caTimeStamp;
    }
    else
    {
        QCaDateTime localTimeStamp( QDateTime::currentDateTime() );
        timeStamp = localTimeStamp;
    }

    // Send off the new data
    emit dataChanged( value, alarmInfo, timeStamp );

    // Save the data just emited
    lastValue = value;
    lastAlarmInfo = alarmInfo;
    lastTimeStamp = timeStamp;

    // Discard the event data
    delete newData;
}

/*!
    Connecting timeout
*/
void QCaObject::setChannelExpired() {

    // Signal a connection change.
    // (This is done with some licence. There isn't really a connection change.
    //  The connection has gone from 'no connection' to 'there never is going to be a connection')
    QCaConnectionInfo connectionInfo( caconnection::NEVER_CONNECTED, caconnection::LINK_DOWN );
    emit connectionChanged( connectionInfo );

    // Generate a user message
    if( userMessage )
    {
        QString msg( recordName );
        userMessage->sendWarningMessage( msg.append( " Channel expired" ), "QCaObject::setChannelExpired()"  );
    }

    // Update the current state
    connectionMachine->expired = true;
    connectionMachine->process( qcastatemachine::CONNECTION_EXPIRED );
}

/*!
  Setup the user message mechanism.
  After calling this method the QCaObject can report errors to the user.
  */
void QCaObject::setUserMessage( UserMessage* userMessageIn )
{
    userMessage = userMessageIn;
}

/*!
  Re-emit the last data emited, if any
  */
void QCaObject::resendLastData()
{
    emit dataChanged( lastValue, lastAlarmInfo, lastTimeStamp );
}

/*!
 Return the engineering units, if any
*/
QString QCaObject::getEgu() {
    return egu;
}

/*!
 Return the enumerations strings, if any
*/
QStringList QCaObject::getEnumerations() {
    return enumerations;
}

/*!
 Return the precision, if any
*/
unsigned int QCaObject::getPrecision()
{
    return precision;
}

/*!
 Return the display upper limit, if any
*/
double QCaObject::getDisplayLimitUpper()
{
    return displayLimitUpper;
}

/*!
 Return the display lower limit, if any
*/
double QCaObject::getDisplayLimitLower()
{
    return displayLimitLower;
}

/*!
 Return the alarm upper limit, if any
*/
double QCaObject::getAlarmLimitUpper()
{
    return alarmLimitUpper;
}

/*!
 Return the alarm lower limit, if any
*/
double QCaObject::getAlarmLimitLower()
{
    return alarmLimitLower;
}

/*!
 Return the warning upper limit, if any
*/
double QCaObject::getWarningLimitUpper()
{
    return warningLimitUpper;
}

/*!
 Return the warning lower limit, if any
*/
double QCaObject::getWarningLimitLower()
{
    return warningLimitLower;
}

/*!
 Return the control upper limit, if any
*/
double QCaObject::getControlLimitUpper()
{
    return controlLimitUpper;
}

/*!
 Return the control lower limit, if any
*/
double QCaObject::getControlLimitLower()
{
    return controlLimitLower;
}

/*!
  Set if callbacks are required on write completion. (default is write with no callback)
  Note, this is not just for better write status, if affects the behaviour of the write as follows:
  When using write with callback, then record will finish processing before accepting next write.
  Writing with callback may be required when writing code that is tightly integrated with record
  processing and code nneds to know processing has completed.
  Writing with no callback is more desirable when a detachement from record processing is required, for
  example in a GUI after issuing a motor record move a motor stop command will take effect immedietly
  if writing without callback, but will only take affect after the move has finished if writing with callback.
  */
void QCaObject::enableWriteCallbacks( bool enable )
{
    setWriteWithCallback( enable );
}

/*!
  Determine if callbacks are required on write completion.
*/
bool QCaObject::isWriteCallbacksEnabled()
{
    return getWriteWithCallback();
}

