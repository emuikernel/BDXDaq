/*! 
  \class CaConnection
  \version $Revision: #4 $
  \date $DateTime: 2010/08/30 16:37:08 $
  \author anthony.owen
  \brief Low level wrapper around the EPICS library
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

#include <CaConnection.h>

using namespace caconnection;

/*!
    Initialise the unique ID.
*/
int CaConnection::CA_UNIQUE_CONNECTION_ID = 0;

/*!
    Initialise EPICS library and setup working parent. The pointer to the
    working parent is given to the EPICS thread so that any callbacks that
    occur contain the context of the parent that created this CaConnection.
*/
CaConnection::CaConnection( void* newParent ) {
    parent = newParent;
    initialise();
    reset();
}

/*!
    Shutdown EPICS library and reset internal data to defaults.
*/
CaConnection::~CaConnection() {
    shutdown();
    reset();
}

/*!
    Creates only one EPICS context and registers an exception callback.
    Use hasContext() for feedback.
    Returns: REQUEST_SUCCESSFUL or REQUEST_FAILED
*/
ca_responses CaConnection::establishContext( void (*exceptionHandler)(struct exception_handler_args), void* args ) {
    if( context.activated == false ) {
        if( CA_UNIQUE_CONNECTION_ID <= 1) {
            context.creation = ca_context_create( ca_enable_preemptive_callback );
            context.exception = ca_add_exception_event ( exceptionHandler, args );
        }
        context.activated = true;
        switch( context.creation )  {
            case ECA_NORMAL :
                return REQUEST_SUCCESSFUL;
            break;
            default :
                return REQUEST_FAILED;
            break;
        }
    } else {
        return REQUEST_FAILED;
    }
}

/*!
    Establishes client side channel and registers a connection state change
    callback.
    Use activeChannel() for feedback.
    Returns: REQUEST_SUCCESSFUL or REQUEST_FAILED
*/
ca_responses CaConnection::establishChannel( void (*connectionHandler)(struct connection_handler_args), std::string channelName ) {
    if( context.activated == true && channel.activated == false ) {
        channel.creation = ca_create_channel( channelName.c_str(), connectionHandler, this, CA_PRIORITY_DEFAULT, &channel.id );
        ca_pend_io( link.searchTimeout );
        channel.activated = true;
        switch( channel.creation ) {
            case ECA_NORMAL :
                return REQUEST_SUCCESSFUL;
            break;
            default :
                return REQUEST_FAILED;
            break;
        }
    } else {
        return REQUEST_FAILED;
    }
}


// Set the channel element count.
// This can be done after the connection callback has been called and the connection is up
void CaConnection::setChannelElementCount()
{
    // Get the channel element count. This can be done after the connection callback has been called and the connection is up
    channel.elementCount = ca_element_count( channel.id );

    // If fail, default element count to a single element

    if( channel.elementCount < 1  )
    {
        channel.elementCount = 1;
    }
}

/*!
    Subscribes to the established channel and registers for data callbacks
    Use isSubscribed() for feedback.
*/
ca_responses CaConnection::establishSubscription( void (*subscriptionHandler)(struct event_handler_args), void* args, short dbrStructType ) {

    if( channel.activated == true && subscription.activated == false ) {
        subscription.creation = ca_create_subscription( dbrStructType, channel.elementCount, channel.id, DBE_VALUE|DBE_ALARM, subscriptionHandler, args, NULL );
        ca_pend_io( link.searchTimeout );
        subscription.activated = true;
        switch( subscription.creation ) {
            case ECA_NORMAL :
                return REQUEST_SUCCESSFUL;
            break;
            default :
                return REQUEST_FAILED;
            break;
        }
    } else {
        return REQUEST_FAILED;
    }
}

/*!
    Removes channel and associated subscription
    Use activeChannel() for feedback.
*/
void CaConnection::removeChannel() {
    if( channel.activated == true ) {
        ca_clear_channel( channel.id );
        channel.activated = false;
        channel.creation = -1;
    }
}

/*!
    Cancels channel subscription.
*/
void CaConnection::removeSubscription() {
    //NOT IMPLEMENTED
    //ca_clear_subscription( channelId );
}

/*!
    Read channel once and register an event handler.
*/
ca_responses CaConnection::readChannel( void (*readHandler)(struct event_handler_args), void* args, short dbrStructType ) {
    if( channel.activated == true ) {
        channel.readResponse = ca_array_get_callback( dbrStructType, channel.elementCount, channel.id, readHandler, args);
        ca_pend_io( link.readTimeout );
        switch( channel.readResponse ) {
            case ECA_NORMAL :
                return REQUEST_SUCCESSFUL;
            break;
            case ECA_DISCONN :
                return CHANNEL_DISCONNECTED;
            break;
            default :
                return REQUEST_FAILED;
            break;
        }
    } else {
        return CHANNEL_DISCONNECTED;
    }
}

/*!
    Write to channel once and register a write handler.
*/
ca_responses CaConnection::writeChannel( void (*writeHandler)(struct event_handler_args), void* args, short dbrStructType, const void* newDbrValue ) {
    if( channel.activated == true ) {
        if( channel.writeWithCallback )
            channel.writeResponse = ca_put_callback( dbrStructType, channel.id, newDbrValue, writeHandler, args);
        else
            channel.writeResponse = ca_put( dbrStructType, channel.id, newDbrValue);

        ca_pend_io( link.readTimeout );
        switch( channel.writeResponse ) {
            case ECA_NORMAL :
                return REQUEST_SUCCESSFUL;
            break;
            case ECA_DISCONN :
                return CHANNEL_DISCONNECTED;
            break;
            default :
                return REQUEST_FAILED;
            break;
        }
    } else {
        return CHANNEL_DISCONNECTED;
    }
}

/*!
    Set the write callback mode.
    Write with no callback using ca_put() (default)
    or write with callback using ca_put_callback()
    When using write with callback, then record will finish processing before accepting next write.
    Writing with callback may be required when writing code that is tightly integrated with record
    processing and code nneds to know processing has completed.
    Writing with no callback is more desirable when a detachement from record processing is required, for
    example in a GUI after issuing a motor record move a motor stop command will take effect immedietly
    if writing without callback, but will only take affect after the move has finished if writing with callback.
*/
void CaConnection::setWriteWithCallback( bool writeWithCallbackIn )
{
    channel.writeWithCallback = writeWithCallbackIn;
}

/*!
    Get the write callback mode.
*/
bool CaConnection::getWriteWithCallback()
{
    return channel.writeWithCallback;
}

/*!
    Record the connection link status.
*/
void CaConnection::setLinkState( link_states newLinkState ) {
    link.state = newLinkState;
}

/*!
    Retrieve the connection status.
*/
link_states CaConnection::getLinkState() {
    return link.state;
}

/*!
    Retrieve the channel connection state. There is no set connection state
    because this is handled by the EPICS library.
*/
channel_states CaConnection::getChannelState() {
    channel.state = ca_state( channel.id );
    switch( channel.state ) {
        case cs_never_conn :
            return NEVER_CONNECTED;
        break;
        case cs_prev_conn :
            return PREVIOUSLY_CONNECTED;
        break;
        case cs_conn :
            return CONNECTED;
        break;
        case cs_closed :
            return CLOSED; 
        break;
        default:
            return CHANNEL_UNKNOWN;
        break;
    }
}

/*!
    Retrieve the channel's database type.
*/
short CaConnection::getChannelType() {
    channel.type = ca_field_type( channel.id );
    return channel.type;
}

/*!
    Initialise with unique ID and state information
*/
void CaConnection::initialise() {
    CA_UNIQUE_CONNECTION_ID++;
}

/*!
    Shutdown and free context if last.
*/
void CaConnection::shutdown() {
    CA_UNIQUE_CONNECTION_ID--;

    if( channel.activated == true ) {
        ca_clear_channel( channel.id );
    }
    if( context.activated == true ) {
        if( CA_UNIQUE_CONNECTION_ID <= 0 ) {
            ca_context_destroy();
        }
    }
}

/*!
    Set internal data to startup conditions
*/
void CaConnection::reset() {
    link.searchTimeout = 3.0;
    link.readTimeout = 2.0;
    link.writeTimeout = 2.0;
    link.state = LINK_DOWN;

    context.activated = false;
    context.creation = -1;
    context.exception = -1;

    channel.activated = false;
    channel.creation = -1;
    channel.readResponse = -1;
    channel.writeResponse = -1;
    channel.state = cs_never_conn;
    channel.type = -1;
    channel.id = NULL;
    channel.writeWithCallback = false;

    subscription.activated = false;
    subscription.creation = false;
}
