/*! 
  \class CaObject
  \version $Revision: #3 $
  \date $DateTime: 2010/06/23 07:49:40 $
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

#ifndef CAOBJECTPRIVATE_H_
#define CAOBJECTPRIVATE_H_

#include <cadef.h>
#include <CaConnection.h>
#include <CaRecord.h>
#include <CaObject.h>
#include <Generic.h>

using namespace caobject;
using namespace generic;

class CaObjectPrivate {
    public:
        CaObjectPrivate( CaObject* ownerIn ) { owner = ownerIn; }

        // Manage connections
        caconnection::ca_responses setChannel( std::string channelName );       // Return type actually caconnection::ca_responses
        caconnection::ca_responses startSubscription();                         // Return type actually caconnection::ca_responses
        void                       removeChannel();
        void                       cancelSubscription(); //< NOT IMPLEMENTED

        // Read and write data
        caconnection::ca_responses   readChannel();                               // Return type actually caconnection::ca_responses
        caconnection::ca_responses   writeChannel( generic::Generic *newValue );

        // Data record interrogation
        caconnection::link_states    getLinkState();                              // Return type actually caconnection::link_states
        caconnection::channel_states getChannelState();                           // Return type actually caconnection::channel_states
//        bool                         isChannelConnected(); // < NOT IMPLEMENTED

        // CA callback handlers
        bool        processChannel( struct event_handler_args args );
        static void subscriptionHandler( struct event_handler_args args );
        static void readHandler( struct event_handler_args args );
        static void writeHandler( struct event_handler_args args );
        static void exceptionHandler( struct exception_handler_args args );
        static void connectionHandler( struct connection_handler_args args );

        // CA data
        caconnection::CaConnection *caConnection;
        carecord::CaRecord caRecord;

        // CaObject of which this instance is a part of
    private:
        CaObject* owner;

};

#endif  // CAOBJECTPRIVATE_H_
