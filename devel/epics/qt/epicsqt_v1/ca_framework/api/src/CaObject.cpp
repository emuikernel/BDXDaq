/*! 
  \class CaObject
  \version $Revision: #11 $
  \date $DateTime: 2010/08/30 16:37:08 $
  \author anthony.owen
  \brief Provides CA to an EPICS channel.
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

#define epicsAlarmGLOBAL

#include <CaObjectPrivate.h>
#include <epicsEvent.h>
#include <epicsMutex.h>
#include <alarm.h>
#include <string.h>


static epicsEventId monitorEvent = NULL;
static epicsMutexId accessMutex = NULL;


//===============================================================================
// Initialisation and deletion
//===============================================================================

/*!
    Initialise the unique ID.
*/
int CaObject::CA_UNIQUE_OBJECT_ID = 0;

/*!
    Initialisation
*/
CaObject::CaObject() {
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = new CaObjectPrivate( this );
    priPtr = p;

    p->caConnection = new caconnection::CaConnection( this );
    initialise();
}

/*!
    Shutdown
*/
CaObject::~CaObject() {
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    shutdown();
    delete p->caConnection;
    delete p;
}

/*!
    Initialise the EPICS library by creating or attaching to a context.
*/
void CaObject::initialise() {
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    p->caConnection->establishContext( p->exceptionHandler, this);
    CA_UNIQUE_OBJECT_ID++;
    if( CA_UNIQUE_OBJECT_ID <= 1) {
        monitorEvent = epicsEventCreate( epicsEventEmpty );
        accessMutex = epicsMutexCreate();
    }
    p->caRecord.setName( "" );
    p->caRecord.setValid( false );
}

/*!
    Shutdown the EPICS library.
*/
void CaObject::shutdown() {
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    CA_UNIQUE_OBJECT_ID--;
    if( CA_UNIQUE_OBJECT_ID <= 0 ) {
        epicsMutexDestroy( accessMutex );
        epicsEventDestroy( monitorEvent );
    }
    p->caRecord.setName( "" );
    p->caRecord.setValid( false );
}

//===============================================================================
// Manage connections
//===============================================================================

/*!
    Establishes client side channel setup.
*/
caconnection::ca_responses CaObjectPrivate::setChannel( std::string channelName ) {
    caRecord.setName( channelName );
    caRecord.setValid( false );
    return caConnection->establishChannel( connectionHandler, channelName );
}

/*!
    Begins a callback subscription with the established client side channel
    setup.
*/
caconnection::ca_responses CaObjectPrivate::startSubscription() {

///printf("my: caconnection::ca_responses CaObjectPrivate::startSubscription()\n"); fflush(stdout);
    if( caRecord.getDbrType() == -1 ) {
        return caconnection::REQUEST_FAILED;
    }
    return caConnection->establishSubscription( subscriptionHandler, owner, caRecord.getDbrType() );
}

/*!
    Removes any client side channel setup and accociated subscriptions.
*/
void CaObjectPrivate::removeChannel() {
    caRecord.setName( "" );
    caRecord.setValid( false );
    caConnection->removeChannel();
}

/*!
    Cancels active subscription but does not flush callbacks.
*/
void CaObjectPrivate::cancelSubscription() {
    //NOT IMPLEMENTED
    //cerr << "CaObject::cancelSubscription()" << endl;
    //caConnection.removeSubscription();
}

//===============================================================================
// Read and write data
//===============================================================================

/*!
    Request one shot read callback for the channel.
*/
caconnection::ca_responses CaObjectPrivate::readChannel() {
///printf("my: caConnection->readChannel\n"); fflush(stdout);

    if( caRecord.getDbrType() == -1 ) {
        return caconnection::REQUEST_FAILED;
    }
    return caConnection->readChannel( readHandler, owner, caRecord.getDbrType() );


}

/*!
    Request one shot write for the channel (Generates callback).
*/
caconnection::ca_responses CaObjectPrivate::writeChannel( generic::Generic *newValue ) {

    switch( newValue->getType() ) {
        case generic::STRING :
        {
            std::string outValue = newValue->getString();
            return caConnection->writeChannel( writeHandler, owner, DBR_STRING, outValue.c_str() );
            break;
        }
        case generic::SHORT :
        {
            long outValue = newValue->getShort();
            return caConnection->writeChannel( writeHandler, owner, DBR_SHORT, &outValue );
            break;
        }
        case generic::CHAR :
        {
            char outValue = newValue->getChar();
            return caConnection->writeChannel( writeHandler, owner, DBR_CHAR, &outValue );
            break;
        }
        case generic::UNSIGNED_LONG :
        {
            unsigned long outValue = newValue->getUnsignedLong();
            return caConnection->writeChannel( writeHandler, owner, DBR_LONG, &outValue );
            break;
        }
        case generic::DOUBLE :
        {
            double outValue = newValue->getDouble();
            return caConnection->writeChannel( writeHandler, owner, DBR_DOUBLE, &outValue );
            break;
        }
        default :
        {
            return caconnection::REQUEST_FAILED;
        }
    }
    return caconnection::REQUEST_FAILED;
}

//===============================================================================
// Data record interrogation
//===============================================================================

/*!
  Return true if the current data record is from the first update after connecting
 */
bool CaObject::isFirstUpdate()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.isFirstUpdate();
}

/*!
  Return a copy of the data record.
  A void* is returned although the actual data type is carecord::CaRecord*
  NOTE: The caller is responsible for deleting the record returned.
 */
void* CaObject::getRecordCopyPtr()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return (void*)(new carecord::CaRecord( p->caRecord ));
}

/*!
  Get count of enuerated strings from the current data record
 */
int CaObject::getEnumStateCount()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getEnumStateCount();
}

/*!
  Get an enuerated string from the current data record
 */
std::string CaObject::getEnumState( int position )
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getEnumState( position );
}

/*!
  Get floating point precision from the current data record
 */
int CaObject::getPrecision()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getPrecision();
}

/*!
  Get the engineering units from the current data record
 */
std::string CaObject::getUnits()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getUnits();
}

/*!
  Get the data type from the current data record
 */
generic_types CaObject::getType()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getType();
}

/*!
  Get the seconds part of the EPICS timestamp from the current record
 */
unsigned long CaObject::getTimeStampSeconds()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getTimeStampSeconds();
}

/*!
  Get the nanoseconds part of the EPICS timestamp from the current record
 */
unsigned long CaObject::getTimeStampNanoseconds()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getTimeStampNanoseconds();
}

/*!
    Returns the alarm status from the current record
*/
short CaObject::getAlarmStatus()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getStatus();
}

/*!
    Returns the alarm severity from the current record
*/
short CaObject::getAlarmSeverity()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    return p->caRecord.getAlarmSeverity();
}

/*!
    Returns the display upper limit
*/
double CaObject::getDisplayUpper()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getDisplayLimit();
    return limit.upper;
}

/*!
    Returns the display lower limit
*/
double CaObject::getDisplayLower()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getDisplayLimit();
    return limit.lower;
}

/*!
    Returns the alarm upper limit
*/
double CaObject::getAlarmUpper()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getAlarmLimit();
    return limit.upper;
}

/*!
    Returns the alarm lower limit
*/
double CaObject::getAlarmLower()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getAlarmLimit();
    return limit.lower;
}

/*!
    Returns the warning upper limit
*/
double CaObject::getWarningUpper()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getWarningLimit();
    return limit.upper;
}

/*!
    Returns the warning lower limit
*/
double CaObject::getWarningLower()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getWarningLimit();
    return limit.lower;
}

/*!
    Returns the control upper limit
*/
double CaObject::getControlUpper()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getControlLimit();
    return limit.upper;
}

/*!
    Returns the control lower limit
*/
double CaObject::getControlLower()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    carecord::ca_limit limit = p->caRecord.getControlLimit();
    return limit.lower;
}

/*!
    Returns the link status for the connection for the current record
*/
caconnection::link_states CaObjectPrivate::getLinkState() {
    return caConnection->getLinkState();
}

/*!
    Returns the link state of the channel for the current record
    The only 'good' state is channel_states::CONNECTED.
    Reads and writes should not be attempted until the channel is connected.
*/
caconnection::channel_states CaObjectPrivate::getChannelState() {
    return caConnection->getChannelState();
}

//===============================================================================
// CA callback handlers
//===============================================================================

/*!
    Processes EPICS callbacks, rebuilds returned data into a CaRecord.
    Database types that EPICS can return, fall into the groups:
    Basic, Status, Time, Graphic and Control.
    /note Time and Graphic database groups not implemented.
*/
bool CaObjectPrivate::processChannel( struct event_handler_args args ) {

    switch( args.type ) {
        case DBR_STS_STRING :
        {
            struct dbr_sts_string incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setString( incommingData.value );
            break;
        }
        case DBR_STS_SHORT :
        {
            struct dbr_sts_short incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_STS_FLOAT :
        {
            struct dbr_sts_float incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setDouble( incommingData.value );
            break;
        }
        case DBR_STS_ENUM :
        {
            struct dbr_sts_enum incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_STS_CHAR :
        {
            struct dbr_sts_char incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setRiscAlignment( incommingData.RISC_pad );
            caRecord.setChar( incommingData.value );
            break;
        }
        case DBR_STS_LONG :
        {
            struct dbr_sts_long incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_STS_DOUBLE :
        {
            struct dbr_sts_double incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setDouble( incommingData.value );
            break;
        }
        case DBR_CTRL_STRING : //< Same as dbr_sts_string
        {
            struct dbr_sts_string incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setString( incommingData.value );
            break;
        }
        case DBR_CTRL_SHORT :
        {
            struct dbr_ctrl_int incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setUnits( std::string( incommingData.units ) );
            caRecord.setDisplayLimit( incommingData.upper_disp_limit, incommingData.lower_disp_limit );
            caRecord.setAlarmLimit( incommingData.upper_alarm_limit, incommingData.lower_alarm_limit );
            caRecord.setWarningLimit( incommingData.upper_warning_limit, incommingData.lower_warning_limit );
            caRecord.setControlLimit( incommingData.upper_ctrl_limit, incommingData.lower_ctrl_limit );
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_CTRL_FLOAT :
        {
            struct dbr_ctrl_float incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setPrecision( incommingData.precision );
            caRecord.setUnits( std::string( incommingData.units ) );
            caRecord.setRiscAlignment( incommingData.RISC_pad );
            caRecord.setDisplayLimit( incommingData.upper_disp_limit, incommingData.lower_disp_limit );
            caRecord.setAlarmLimit( incommingData.upper_alarm_limit, incommingData.lower_alarm_limit );
            caRecord.setWarningLimit( incommingData.upper_warning_limit, incommingData.lower_warning_limit );
            caRecord.setControlLimit( incommingData.upper_ctrl_limit, incommingData.lower_ctrl_limit );
            caRecord.setDouble( incommingData.value );
            break;
        }
        case DBR_CTRL_ENUM :
        {
            struct dbr_ctrl_enum incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            for( int i = 0; i < incommingData.no_str; i++ ) {
                caRecord.addEnumState( std::string( incommingData.strs[i] ) );
            }
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_CTRL_CHAR :
        {
            struct dbr_ctrl_char incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setRiscAlignment( incommingData.RISC_pad );
            caRecord.setDisplayLimit( incommingData.upper_disp_limit, incommingData.lower_disp_limit );
            caRecord.setAlarmLimit( incommingData.upper_alarm_limit, incommingData.lower_alarm_limit );
            caRecord.setWarningLimit( incommingData.upper_warning_limit, incommingData.lower_warning_limit );
            caRecord.setControlLimit( incommingData.upper_ctrl_limit, incommingData.lower_ctrl_limit );
            caRecord.setChar( incommingData.value );
            break;
        }
        case DBR_CTRL_LONG :
        {
            struct dbr_ctrl_long incommingData;
            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData.status );
            caRecord.setAlarmSeverity( incommingData.severity );
            caRecord.setUnits( std::string( incommingData.units ) );
            caRecord.setDisplayLimit( incommingData.upper_disp_limit, incommingData.lower_disp_limit );
            caRecord.setAlarmLimit( incommingData.upper_alarm_limit, incommingData.lower_alarm_limit );
            caRecord.setWarningLimit( incommingData.upper_warning_limit, incommingData.lower_warning_limit );
            caRecord.setControlLimit( incommingData.upper_ctrl_limit, incommingData.lower_ctrl_limit );
            caRecord.setUnsignedLong( incommingData.value );
            break;
        }
        case DBR_CTRL_DOUBLE :
        {
//            struct dbr_ctrl_double incommingData;
//            memcpy( &incommingData, args.dbr, dbr_size_n(args.type, args.count) );
            struct dbr_ctrl_double* incommingData = (dbr_ctrl_double*)(args.dbr);
            caRecord.setName( ca_name( args.chid ) );
            caRecord.setValid( true );
            caRecord.updateProcessState();
            caRecord.setStatus( incommingData->status );
            caRecord.setAlarmSeverity( incommingData->severity );
            caRecord.setPrecision( incommingData->precision );
            caRecord.setUnits( std::string( incommingData->units ) );
            caRecord.setRiscAlignment( incommingData->RISC_pad0 );
            caRecord.setDisplayLimit( incommingData->upper_disp_limit, incommingData->lower_disp_limit );
            caRecord.setAlarmLimit( incommingData->upper_alarm_limit, incommingData->lower_alarm_limit );
            caRecord.setWarningLimit( incommingData->upper_warning_limit, incommingData->lower_warning_limit );
            caRecord.setControlLimit( incommingData->upper_ctrl_limit, incommingData->lower_ctrl_limit );
  //          caRecord.setDouble( incommingData->value );
            caRecord.setDouble( &incommingData->value, args.count );
            break;
        }
        default :
        {
            //cerr << "CaObject::processChannel(): " << ca_name( args.chid )
            //     << ", type not implemented: " << args.type << endl;
        }
    }
    return 0;
}

/*!
    Subscription handler callback.
*/
void CaObjectPrivate::subscriptionHandler( struct event_handler_args args ) {

    //!!!??? WARNING callbacks can occur AFTER a connection has been closed. Handle this! AJR
///printf("my: subscriptionHandler\n"); fflush(stdout);
    epicsMutexLock( accessMutex );
    CaObject* context = (CaObject*)args.usr;

    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)(context->priPtr);

    switch( args.status ) {
        case ECA_NORMAL :
            p->processChannel( args );
            context->signalCallback( SUBSCRIPTION_SUCCESS );
        break;
        default :
            context->signalCallback( SUBSCRIPTION_FAIL );
        break;
    }
    epicsEventSignal( monitorEvent );
    epicsMutexUnlock( accessMutex );
}

/*!
    Read data handler callback.
*/
void CaObjectPrivate::readHandler( struct event_handler_args args ) {

    //!!!??? WARNING callbacks can occur AFTER a connection has been closed. Handle this! AJR
///printf("my: readHandler\n"); fflush(stdout);
    epicsMutexLock( accessMutex );
    CaObject* context = (CaObject*)args.usr;

    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)(context->priPtr);

    switch( args.status ) {
        case ECA_NORMAL :
            p->processChannel( args );
            context->signalCallback( READ_SUCCESS );
        break;
        default :
            context->signalCallback( READ_FAIL );
        break;
    }
    epicsEventSignal( monitorEvent );
    epicsMutexUnlock( accessMutex );
}

/*!
    Write data handler callback.
*/
void CaObjectPrivate::writeHandler( struct event_handler_args args ) {

    //!!!??? WARNING callbacks can occur AFTER a connection has been closed. Handle this! AJR

    epicsMutexLock( accessMutex );
    CaObject* context = (CaObject*)args.usr;

    switch( args.status ) {
        case ECA_NORMAL :
            context->signalCallback( WRITE_SUCCESS );
        break;
        default :
            context->signalCallback( WRITE_FAIL );
        break;
    }
    epicsMutexUnlock( accessMutex );
}

/*!
    EPICS Exception handler callback.
*/
void CaObjectPrivate::exceptionHandler( struct exception_handler_args args ) {

    //!!!??? WARNING callbacks can occur AFTER a connection has been closed. Handle this! AJR

    epicsMutexLock( accessMutex );
    CaObject* context = (CaObject*)args.usr;

    switch( args.stat ) {
        case ECA_NORMAL :
            context->signalCallback( EXCEPTION );
        break;
        default :
//!!!??? crash occurs here on occasion when opening new GUI, perhaps because callbacks can occur AFTER a connection has been closed. See warning above
//       Also crash here when ioc is shutdown.
//            context->signalCallback( EXCEPTION );
        break;
    }
    epicsMutexUnlock( accessMutex );
}

/*!
    Connection handler callback.
    This is called with CaObject out of context, it is recovered in:
    "args" -> "parent" -> "grandParent".
*/
void CaObjectPrivate::connectionHandler( struct connection_handler_args args ) {

    //!!!??? WARNING callbacks can occur AFTER a connection has been closed. Handle this! AJR

    epicsMutexLock( accessMutex );
    caconnection::CaConnection* parent = (caconnection::CaConnection*)ca_puser( args.chid );

    CaObject* grandParent = (CaObject*)parent->getParent();
    switch( args.op ) {
        case CA_OP_CONN_UP :
            {
                CaObjectPrivate* grandParentPri = (CaObjectPrivate*)(grandParent->priPtr);
                grandParentPri->caRecord.setDbrType( parent->getChannelType() );
            }
            parent->setChannelElementCount();
            parent->setLinkState( caconnection::LINK_UP );
            grandParent->signalCallback( CONNECTION_UP );
        break;
        case CA_OP_CONN_DOWN :
            parent->setLinkState( caconnection::LINK_DOWN );
            grandParent->signalCallback( CONNECTION_DOWN );
        break;
        default :
            parent->setLinkState( caconnection::LINK_UNKNOWN );
            grandParent->signalCallback( CONNECTION_UNKNOWN );
        break;
    }   
    epicsMutexUnlock( accessMutex );
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
void CaObject::setWriteWithCallback( bool writeWithCallbackIn )
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    // Set the write callback requirements
    p->caConnection->setWriteWithCallback( writeWithCallbackIn );
}

/*!
  Determine if callbacks are delivered on write completion.
  */
bool CaObject::getWriteWithCallback()
{
    // Get the parts not shared with the non CA world
    CaObjectPrivate* p = (CaObjectPrivate*)priPtr;

    // return the write callback requirements
    return p->caConnection->getWriteWithCallback();
}

