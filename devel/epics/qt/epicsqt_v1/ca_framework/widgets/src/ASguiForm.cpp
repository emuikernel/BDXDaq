/*! 
  \class ASguiForm
  \version $Revision: #12 $
  \date $DateTime: 2010/09/06 11:58:56 $
  \author andrew.rhyder
  \brief Form containing widgets, including QCa widgets, read from a UI file
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

/*! This class is used as a container for QCa widgets.

    It adds any variable name macro substitutions to the current environment profile, creates a form widget and
    reads a UI file which can contain QCa widgets.

    As QCa widgets are created, they note the current environment profile, including variable name substitutions.
    QCa widgets also register themselves with this class so this class can activate them once they are fully created.
    QCa widgets can't activate themselves. The Qt form loader creates each widget and calls the appropriate property
    functions to set it up. The widget itself does not know what properties are going to be set and when they have
    all been set. For this reason the QCa widgets don't know when to request CA data. Both variable name properties
    and variable name substitution properties must be set up to request data and other properties may need to be set
    up before udates can be used.

    This class can be used directly (within a GUI application) as the top level form, or as a base class for
    the ASguiFormPlugin class. The ASguiFormPlugin plugin class is used to store a gui form in a UI file.
*/

#include <QUiLoader>
#include <QtGui>
#include <QString>
#include <QDir>
#include <QtDebug>
#include <ASguiForm.h>
#include <ContainerProfile.h>
#include <QCaWidget.h>

/// Constructor.
/// No UI file is read. uiFileName must be set and then readUiFile() called after construction
ASguiForm::ASguiForm( QWidget* parent ) : QScrollArea( parent ) {
    commonInit( false );
}

/// Constructor.
/// UI filename is supplied and UI file is read as part of construction.
ASguiForm::ASguiForm( const QString& uiFileNameIn, QWidget* parent ) : QScrollArea( parent ) {
    commonInit( true );
    uiFileName = uiFileNameIn;
}

/// Common construction
void ASguiForm::commonInit( const bool alertIfUINoFoundIn )
{
    ui = NULL;
    alertIfUINoFound = alertIfUINoFoundIn;

    // Prepare to recieve notification that the ui file being displayed has changed
    QObject::connect( &fileMon, SIGNAL( fileChanged( const QString & ) ), this, SLOT( fileChanged( const QString & ) ) );
}

/// Destructor.
ASguiForm::~ASguiForm()
{
    // Close any existing form
    if( ui )
        ui->close();
}

// Read a UI file.
// The file read depends on the value of uiFileName
void ASguiForm::readUiFile()
{
    if (!uiFileName.isEmpty()) {

        /// Set up the environment profile for any QCa widgets created by the form
        QObject* savedGuiLaunchConsumer = NULL;

        ContainerProfile profile;
        bool localProfile;
        if( profile.isProfileDefined() )
        {
            // Flag the profile was not set up in this function (and so should not be released in this function)
            localProfile = false;

            // A profile is already defined, either by the gui application or by a higher level form.
            // Extend any variable name substitutions with this form's substitutions
            profile.addMacroSubstitutions( variableNameSubstitutions );

            // If this form is handling form launch requests from object created within it, replace any form launcher with our own
            if( handleGuiLaunchRequests )
                savedGuiLaunchConsumer = profile.replaceGuiLaunchConsumer( this );
        }
        else
        {
            // Flag the profile was set up in this function (and so should be released in this function)
            localProfile = true;

            // A profile is not already defined. This is the case if this class is the base to a form plugin loaded by 'designer'.
            // Use this form's default message handling, always handle any GUI launch requests, and add this form's substitutions
            userMessage.setup( this );
            profile.setupProfile( this, this, this, this, QDir::currentPath(), variableNameSubstitutions, true );
        }

        // Attempt to open the UI file.
        // If appropriate, warn the user if the UI file could not be opened.
        QUiLoader loader;
        QDir path( profile.getPath() );
        QFile uiFile( path.filePath( uiFileName ));
        if( !uiFile.open( QIODevice::ReadOnly ) ) {
            if( alertIfUINoFound ) {
                QString msg;
                QTextStream(&msg) << "User interface file '" << uiFile.fileName() << "' could not be opened";
                //!!!??? currently not working. profile not set up yet so user messages can't be displayed this early perhaps
                userMessage.sendWarningMessage( msg, "ASguiForm::readUiFile"  );
                qDebug() << msg;
            }
        }

        // Load the UI file if opened
        else {
            /// Ensure no other files are being monitored (belt and braces)
            QStringList monitoredPaths = fileMon.files();
            if( monitoredPaths.count())
            {
                fileMon.removePaths( monitoredPaths );
            }

            /// Monitor the opened file
            fileMon.addPath( path.filePath( uiFileName ) );

            /// Extract the file name part used for the window title
            QDir fileDir( profile.getPath() );
            QFileInfo fileInfo( fileDir, uiFile.fileName() );
            title = fileInfo.fileName();
            if( title.endsWith( ".ui" ) )
                title.chop( 3 );

            /// Load the gui
            ui = loader.load(&uiFile);
            uiFile.close();

            /// Any QCa widgets that have just been created need to be activated.
            /// Note, this is only required when QCa widgets are not loaded within a form and not directly by 'designer'.
            /// When loaded directly by 'designer' they are activated (a CA connection is established) as soon as either
            /// the variable name or variable name substitution properties are set
            QCaWidget* containedWidget;
            while( (containedWidget = profile.getNextContainedWidget()) )
                containedWidget->activate();

            // Remove the macro substitutions added for this form
            // This is irrelelvent (but harmless) if there was no profile already defined.
            profile.removeMacroSubstitutions();

            /// Present the gui to the user
            setWidget( ui );
        }

        // If the profile is not our own, restore it to how we found it
        if( !localProfile )
        {
            /// If this form is handling form launch requests from object created within it, put back any original
            /// form launcher now all objects have been created
            if ( handleGuiLaunchRequests )
                 profile.replaceGuiLaunchConsumer( savedGuiLaunchConsumer );
        }

        // The profile was created within this function. Release it so nothing created later tries to use this object's services
        else
        {
            profile.releaseProfile();
        }

    }
}

// Get the form title
QString ASguiForm::getASGuiTitle(){
    return title;
}

// Get the UI file name used to build the gui
QString ASguiForm::getGuiFileName(){
    return uiFileName;
}

/// Set the variable name substitutions used by all QCa widgets wihtin the form
void ASguiForm::setVariableNameSubstitutions( QString variableNameSubstitutionsIn )
{
    variableNameSubstitutions = variableNameSubstitutionsIn;

    // The macro substitutions have changed. Reload the form to pick up new substitutions.
    // NOTE an alternative to this would be to find all QCa widgets contained in the form and it's descentand forms, modify the macro substitutions and reconnect.
    // This is a realistic option since contained widgets now register themselves with the form on creation so the fomr can activate them once all properties have been set up
    if( ui )
    {
        ui->close();
        readUiFile();
    }
}

// Slot for reloading the file if it has changed.
// It doesn't matter if it has been deleted, a reload attempt will still tell the user what they need to know - that the file has gone.
void ASguiForm::fileChanged ( const QString & /*path*/ )
{
    // Ensure we arn't monitoring files any more
    QStringList monitoredPaths = fileMon.files();
    fileMon.removePaths( monitoredPaths );

    // Close any existing form
    if( ui )
        ui->close();

    // Re-open file
    readUiFile();

}

// Slot for presenting messages to the user.
// Normally a gui will have provided it's own message and error handling.
// This is only used if no environment profile has been set up when a form is created. This is the case if created within 'designer'
// Use a general message
void ASguiForm::onGeneralMessage( QString message )
{
    QMessageBox msgBox;
    msgBox.setText( "Message" );
    msgBox.setInformativeText( message );
    msgBox.exec();
}

// Slot for launching another form.
// Normally a gui will have provided it's own GUI launch mechanism.
// This is only used if no environment profile has been set up when a form is created. This is the case if created within 'designer'
// Launch a GUI.
// Note, creation options are ignored as the guiForm has no application wide context to know
// what 'creating a new tab', etc, means. A new window is always created.
 void ASguiForm::onGuiLaunch( QString guiName, QString /*variableNameSubstitutions*/, creationOptions )
 {
     // Use variable name substitutions???
     // Build the gui
    ASguiForm* gui = new ASguiForm( guiName );
    if( gui )
        gui->readUiFile();
}

// Slot same as default widget setEnabled slot, but renamed to match other QCa widgets where requestEnabled() will use our own setEnabled
// which will allow alarm states to override current enabled state
void ASguiForm::requestEnabled( const bool& state )
{
    setEnabled(state);
}

//==============================================================================
// Property convenience functions

// Access functions for variableName and variableNameSubstitutions
// variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
void ASguiForm::setVariableNameAndSubstitutions( QString, QString variableNameSubstitutionsIn, unsigned int ) {

    /// Set new variable name substitutions
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
}

// UI file name
void    ASguiForm::setUiFileName( QString uiFileNameIn )
{
    uiFileName = uiFileNameIn;
    readUiFile();
}
QString ASguiForm::getUiFileName()
{
    return uiFileName;
}

// Flag indicating form should handle gui form launch requests
void ASguiForm::setHandleGuiLaunchRequests( bool handleGuiLaunchRequestsIn )
{
    handleGuiLaunchRequests = handleGuiLaunchRequestsIn;
}
bool ASguiForm::getHandleGuiLaunchRequests()
{
    return handleGuiLaunchRequests;
}
