/*!
  \class
  \version $Revision: #9 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief
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

#include <QtGui/QApplication>
#include <MainWindow.h>
#include <StartupParams.h>
#include <InstanceManager.h>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Get the startup parameters from the command line arguments
    startupParams params;
    QStringList args = QCoreApplication::arguments();
    params.getStartupParams( args );

    // If only a single instance has been requested,
    // and if there is already another instance of ASgui
    // and it takes the parameters, do no more
    instanceManager instance( &app );
    if( params.singleApp && instance.handball( &params ) )
        return 0;

    // Start the main application window
    instance.newWindow( params );
    return app.exec();
}
