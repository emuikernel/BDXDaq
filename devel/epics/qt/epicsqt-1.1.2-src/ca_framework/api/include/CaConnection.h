/*! 
  \class CaConnection
  \version $Revision: #4 $
  \date $DateTime: 2010/08/30 16:37:08 $
  \author glenn.jackson
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
 *    Glenn Jackson
 *  Contact details:
 *    glenn.jackson@internode.net
 */

#ifndef CACONNECTION_H_
#define CACONNECTION_H_

#include <string>
#include <cadef.h>


namespace caconnection {

  enum link_states { LINK_UP, LINK_DOWN, LINK_UNKNOWN };
  enum channel_states { NEVER_CONNECTED, PREVIOUSLY_CONNECTED, CONNECTED, CLOSED, CHANNEL_UNKNOWN };
  enum ca_responses { REQUEST_SUCCESSFUL, CHANNEL_DISCONNECTED, REQUEST_FAILED };

  //! Connection link state and properties.
  struct caLink {
      double searchTimeout;
      double readTimeout;
      double writeTimeout;
      link_states state;
  };

  //! EPICS context state information.
  struct caContext {
      bool activated;
      int creation;
      int exception;
  };

  //! Channel state and properties.
  struct caChannel {
      bool activated;
      int creation;
      int readResponse;
      int writeResponse;
      int state;
      short type;
      unsigned long elementCount;
      chid id;
      bool writeWithCallback;
  };

  //! Subscription state and properties.
  struct caSubscription {
      bool activated;
      int creation;
  };

  class CaConnection {
    public:      
      CaConnection( void* newParent = 0 );
      ~CaConnection();

      ca_responses establishContext( void (*exceptionHandler)(struct exception_handler_args), void* args );
      ca_responses establishChannel( void (*connectionHandler)(struct connection_handler_args), std::string channelName );
      ca_responses establishSubscription( void (*subscriptionHandler)(struct event_handler_args), void* args, short dbrStructType );

      void removeChannel();
      void removeSubscription(); //< NOT IMPLEMENTED

      ca_responses readChannel( void (*readHandler)(struct event_handler_args), void* args, short dbrStructType );
      ca_responses writeChannel( void (*writeHandler)(struct event_handler_args), void* args, short dbrStructType, const void* dbrValue );

      void setLinkState( link_states newLinkState );
      link_states getLinkState();

      channel_states getChannelState();
      short getChannelType();

      void* getParent() { return parent; }

      bool hasContext() { return context.activated; }
      bool activatedChannel() { return channel.activated; }
      bool isSubscribed() { return subscription.activated; }
      void setChannelElementCount();

      void setWriteWithCallback( bool writeWithCallbackIn );
      bool getWriteWithCallback();

    private:
      caLink link;
      caContext context;
      caChannel channel;
      caSubscription subscription;

      //! Unique ID assigned to each instance.
      static int CA_UNIQUE_CONNECTION_ID;
      void* parent;

      void initialise();
      void shutdown();
      void reset();
  };

}

#endif  // CACONNECTION_H_
