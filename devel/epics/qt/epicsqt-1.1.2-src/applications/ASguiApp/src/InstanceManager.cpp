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
/*! This class is used to manage maintaining only a single instance of ASgui when required.

    On creation it attempts to connect to a server on an already running ASgui.
    If it can't connect, it assumes it is the only version of ASgui and starts the server itself.

    When ASgui starts with a -s parameter, indicating that only a single instance of the application is required,
    It passes all the startup parameters to the handball() method of this class.

    If this class has connected to a server, it passes the startup parameters to the server and then stops. The ASgui
    application running the server then starts a new main window based on the handballed parameters.
    If this class is not connected to a server, the handball() method returns indicating that it was unable to handball
    the parameters and this instance of ASgui should start a new window regardless of the -s parameter.
*/

#include "InstanceManager.h"
#include <MainWindow.h>


#define ASGUISERVERNAME "ASguiInstance"

instanceManager::instanceManager( QObject *parent ) : QObject( parent )
{
    // Create a socket
    socket = new QLocalSocket(this);
    socket->connectToServer( ASGUISERVERNAME, QIODevice::WriteOnly );

    // If no other instance is found, discard the socket  and start a server for future instances
    // (no socket will be used to indicate no other instance)
    if( !socket->waitForConnected( 1000 ) )
    {
        delete socket;
        socket = 0;

        // Kill any other server.
        // This is required if an eariler instance has crashed
        QLocalServer::removeServer( ASGUISERVERNAME );

        // Start a server to listen for other instances of ASgui starting
        server = new QLocalServer( this );
        connect( server, SIGNAL(newConnection()), this, SLOT(connected()));
        if( !server->listen( ASGUISERVERNAME ))
            qDebug() << "Couldn't start server";
    }
}

instanceManager::~instanceManager()
{
}

bool instanceManager::handball( startupParams* params )
{
    // If no other instance, do nothing
    if( !socket )
        return false;

    // Build a serial copy of the parameters
    QByteArray ba;
    params->setSharedParams( ba );

    // Send it to the other instance
    socket->write( ba );

    // Wait until it is gone.
    // The application will be closing down almost immedietly and we don't
    // wait here it doesn't get written
    socket->waitForBytesWritten ( 10000 );

    // Return indicating startup request has been hand-balled to another instance of ASgui
    return true;
}

void instanceManager::connected()
{
    client = server->nextPendingConnection();
    connect( client, SIGNAL(readyRead ()), this, SLOT(readParams()));
}

void instanceManager::readParams()
{
    QByteArray ba(client->readAll());
    startupParams params;
    params.getSharedParams( ba );
    newWindow( params );
}

// Create a new main window
void instanceManager::newWindow( const startupParams& params )
{
    MainWindow* mw = new MainWindow( params.filename, params.path, params.substitutions, params.enableEdit );
    mw->show();
//    setActivationWindow( mw );
}
