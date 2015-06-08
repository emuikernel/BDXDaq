/*! 
  \class ContainerProfile
  \version $Revision: #4 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief Defines attributes of the containing window (form, dialog, etc) within which QCa widgets are being created.
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

#ifndef CONTAINERPROFILE_H
#define CONTAINERPROFILE_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <QDebug>
#include <QCaPluginLibrary_global.h>

class QCaWidget;

class WidgetRef
{
    public:
        WidgetRef( QCaWidget* refIn ) { ref = refIn; }
        ~WidgetRef() {}
        QCaWidget* getRef() { return ref; }

    private:
        QCaWidget* ref;
};

class QCAPLUGINLIBRARYSHARED_EXPORT ContainerProfile
{
public:
    ContainerProfile();    // Construction. Sets up local copies of any profile defined by setupProfile() in an earlier instance
    ~ContainerProfile();   // Destruction
    void init();

    void setupProfile( QObject* statusMessageConsumerIn,
                       QObject* errorMessageConsumerIn,
                       QObject* warningMessageConsumerIn,
                       QObject* guiLaunchConsumerIn,
                       QString pathIn,
                       QString macroSubstitutionsIn,
                       bool interactiveIn );      // Setup an environmental profile for all QcaWidgets to use on creation
    QObject* replaceGuiLaunchConsumer( QObject* newGuiLaunchConsumerIn );  // Override the current GUI launch consumer

    void addMacroSubstitutions( QString macroSubstitutionsIn ); // Add another set of macro substitutions to those setup by setupProfile(). Used as sub forms are created
    void removeMacroSubstitutions();                            // Remove the last set of macro substitutions added by addMacroSubstitutions(). Used after sub forms are created

    QObject* getStatusMessageConsumer();   // Get the local copy of the object that will recieve status message events
    QObject* getWarningMessageConsumer();  // Get the local copy of the object that will recieve warning message events
    QObject* getErrorMessageConsumer();    // Get the local copy of the object that will recieve error message events
    QObject* getGuiLaunchConsumer();       // Get the local copy of the object that will recieve GUI launch requests
    QString getPath();                     // Get the local copy of the path used for file operations
    QString getMacroSubstitutions();       // Get the local copy of the variable name macro substitutions
    bool isProfileDefined();               // Returns true if a profile has been setup by setupProfile()
    bool isInteractive();                  // Returns true if the profile was set up by an application where a user is interacting with properties such as variable names

    void addContainedWidget( QCaWidget* containedWidget );  // Adds a reference to the list of QCa widgets created with this profile
    QCaWidget* getNextContainedWidget();                    // Returns a reference to the next QCa widgets in the list of QCa widgets created with this profile

    void releaseProfile();                                  // Clears the context setup by setupProfile(). Local data in all instances is still valid

private:
    static QObject* publishedStatusMessageConsumer;     // Object to send status message event to
    static QObject* publishedErrorMessageConsumer;      // Object to send error message event to
    static QObject* publishedWarningMessageConsumer;    // Object to send warning message event to
    static QObject* publishedGuiLaunchConsumer;         // Object to send GUI launch requests to
    static QString publishedPath;                       // Path used for file operations
    static QList<QString> publishedMacroSubstitutions;  // list of variable name macro substitution strings. Extended by each sub form created
    static bool publishedInteractive;                    // Flag true if the profile was set up by an application where a user is interacting with properties such as variable names

    static QList<WidgetRef> containedWidgets;           // List of QCa widgets created with this profile

    static bool profileDefined;                         // Flag true if a profile has been setup. Set between calling setupProfile() and releaseProfile()

    QObject* statusMessageConsumer;  // Local copy of status message consumer. Still valid after the profile has been released by releaseProfile()
    QObject* errorMessageConsumer;   // Local copy of error message consumer. Still valid after the profile has been released by releaseProfile()
    QObject* warningMessageConsumer; // Local copy of warning message consumer. Still valid after the profile has been released by releaseProfile()
    QObject* guiLaunchConsumer;      // Local copy of GUI launch consumer. Still valid after the profile has been released by releaseProfile()
    QString path;                    // Local copy of path used for file operations
    QString macroSubstitutions;      // Local copy of macro substitutions (converted to a single string) Still valid after the profile has been released by releaseProfile()
    bool interactive;              // Local copy of 'is interactive' flag. Still valid after the profile has been released by releaseProfile()

};

#endif // CONTAINERPROFILE_H
