/*!
  \class QCaDatetime
  \version $Revision: #3 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief CA connection info manager
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

#include <CaConnection.h>
#include <QCaConnectionInfo.h>

/*!
  Construct an empty instance.
  By default the channel and link states are unknown
 */
QCaConnectionInfo::QCaConnectionInfo()
{
    channelState = caconnection::CHANNEL_UNKNOWN;
    linkState = caconnection::LINK_UNKNOWN;
}

/*!
  Construct an instance given a channel and link state
 */
QCaConnectionInfo::QCaConnectionInfo( unsigned short channelStateIn, unsigned short linkStateIn ) {
    channelState = channelStateIn;
    linkState = linkStateIn;
}

/*!
  Return true if the channel is connected
 */
bool QCaConnectionInfo::isChannelConnected() {
    return( channelState == caconnection::CONNECTED );
}

/*!
  Return true if the link is up
 */
bool QCaConnectionInfo::isLinkUp() {
    return( linkState == caconnection::LINK_UP );
}
