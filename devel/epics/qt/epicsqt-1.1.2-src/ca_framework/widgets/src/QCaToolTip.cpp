/*!
  \class QCaLabel
  \version $Revision: #3 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief CA Label Widget.
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

#include "QCaToolTip.h"

QCaToolTip::QCaToolTip()
{
    isConnected = false;
}

/*!
    Update the tool tip as requested by QCaWidget.
*/
void QCaToolTip::updateToolTipVariable ( const QString& variable ) {
    toolTipVariable = variable;
    displayToolTip();
}

void QCaToolTip::updateToolTipAlarm ( const QString& alarm )
{
    toolTipAlarm = alarm;
    displayToolTip();
}

void QCaToolTip::updateToolTipConnection ( bool isConnectedIn )
{
    isConnected = isConnectedIn;
    displayToolTip();
}

/*!
   Build and display the tool tip from the name and state
*/
void QCaToolTip::displayToolTip() {

    QString toolTip( toolTipVariable );

    if( toolTipAlarm.size() )
        toolTip.append( " - " ).append( toolTipAlarm );

    if( !isConnected )
        toolTip.append( " - Disconnected" );

    updateToolTip( toolTip );
}

