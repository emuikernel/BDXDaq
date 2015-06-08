/*!
  \class MainWindow
  \version $Revision: #12 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief Creates the main window in the Australian Synchrotron GUI
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

#include <QtGui>
#include <QString>

#include <MainWindow.h>
#include <ASguiForm.h>
#include <QMessageBox>
#include <ContainerProfile.h>

/// Shared list of all main windows
QList<MainWindow*> MainWindow::mainWindowList;

// Shared list of all GUIs being displayed in all main windows
QList<ASguiForm*> MainWindow::guiList;


//=================================================================================
// Methods for construction, destruction, initialisation
//=================================================================================

/// Constructor - No filename supplied
MainWindow::MainWindow( QWidget *parent )  : QMainWindow( parent )
{
    init( "", "", "", false );
}

/// Constructor - Filename supplied
MainWindow::MainWindow( QString fileName, QString path, QString substitutions, bool enableEditIn, QWidget *parent )  : QMainWindow( parent )
{
    init( fileName, path, substitutions, enableEditIn );
}

// Common construction
void MainWindow::init( QString fileName, QString pathIn, QString substitutionsIn, bool enableEditIn )
{

    // Save the substitutions
    substitutions = substitutionsIn;

    // Save the default path
    path = pathIn;

    // Setup the environment profile of the new form
    profile.setupProfile( this, this, this, this, path, substitutions, false );

    // Initialise
    usingTabs = false;

    // Present the main form's ui
    ui.setupUi( this );

    // Setup to allow user to change focus to a window from the 'Windows' menu
    QObject::connect( ui.menuWindows, SIGNAL( triggered( QAction* ) ), this, SLOT( onWindowMenuSelection( QAction* ) ) );

    // Save this instance of a main window in the global list of main windows
    // ??? is this list required?
    mainWindowList.append( this );

    // Set the default title
    setTitle( "" );

    // Populate the 'windows' menu to include all current guis in any main window
    buildWindowsMenu();

    // Enable the edit menu if requested
    enableEdit = enableEditIn;
    if( enableEdit )
        ui.menuEdit->setEnabled( true );

    // If no filename was supplied, open the file selection dialog
    // Do it after the creation of the main window is complete
    if( fileName.isEmpty() )
    {
        QTimer::singleShot( 0, this, SLOT(on_actionOpen_triggered()));
    }

    // If a filename was supplied, load it
    else
    {
        ASguiForm* gui = createGui( fileName );
        loadGuiIntoCurrentWindow( gui );
    }
}

/// Destructor
MainWindow::~MainWindow()
{
    // Remove the GUIs shown in this main window from the GUIs listed in the 'Windows' menus of all other main windows
    removeAllGuisFromWindowsMenu();

    // Remove this main window from the global list of main windows
    for (int i = 0; i < mainWindowList.size(); ++i)
        if( mainWindowList[i] == this )
        {
            mainWindowList.removeAt( i );
            break;
        }

    // Release the environment profile for new QCa wigets
    profile.releaseProfile();

}

//=================================================================================
// Methods for responding to user actions
//=================================================================================

/// Open a gui in a new window.
/// Present a file open dialog box and after generate the gui based on the ui file the user selects
void MainWindow::on_actionNew_Window_triggered()
{
     MainWindow* w = new MainWindow();
     w->show();
}

/// Open a gui in a new tab.
/// Present a file open dialog box and after generate the gui based on the ui file the user selects
void MainWindow::on_actionNew_Tab_triggered()
{
    // If not using tabs, start tabs and migrate any single gui to the first tab
    if( !usingTabs )
        setTabMode();

    // Create the GUI
    ASguiForm* gui = createGui( GuiFileNameDialog( "Open" ) );
    loadGuiIntoNewTab( gui );
}

/// User requested a new gui to be opened
/// Present a file open dialog box and after generate the gui based on the ui file the user selects
void MainWindow::on_actionOpen_triggered()
{
    // Create the GUI
    ASguiForm* gui = createGui( GuiFileNameDialog( "Open" ) );
    loadGuiIntoCurrentWindow( gui );
}

/// Close a gui
void MainWindow::on_actionClose_triggered()
{
    // If using tabs, delete the currenttly selected tab
    if( usingTabs )
    {
        // Delete the tab
        // The tab change code will look after changing the title to whatever gui becomes current.
        QTabWidget* tabs = getCentralTabs();
        if( tabs )
            tabCloseRequest( tabs->currentIndex() );
    }

    // Using a single window, just delete the gui
    else
    {
        ASguiForm* gui = getCentralGui();
        if( gui )
        {
            removeGuiFromWindowsMenu( gui );
            delete gui;
        }

        setTitle( "" );
    }
 }

// Raise the window selected in the 'Window' menu
void MainWindow::onWindowMenuSelection( QAction* action )
{
    // Untested
    return;


    for (int i = 0; i < guiList.size(); ++i) {
        if( guiList[i]->getASGuiTitle() == action->text() )
            guiList[i]->raise();
    }
}

/// Present the 'About' dialog
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About ASgui", "<b>ASgui</b> version 0.1");

}

// Change the current tab
void MainWindow::tabCurrentChanged( int index )
{
    // Do nothing when called while changing from single scroll area to using tabs.
    if( !usingTabs )
        return;

    // Update the main window title
    QTabWidget* tabs = getCentralTabs();
    if( tabs )
        setTitle( tabs->tabText( index ) );
}

// Delete a tab
void MainWindow::tabCloseRequest( int index )
{
    QTabWidget* tabs = getCentralTabs();
    if( !tabs )
        return;

    // Get a reference to the scroll area for the tab being deleted
    tabs->setCurrentIndex( index );
    ASguiForm* gui = (ASguiForm*)(tabs->currentWidget() );

    // Remove the tab, then delete the page (scroll area) the tab represented
    tabs->removeTab( index );
    delete gui;

    // Remove the gui from the 'windows' menus
    removeGuiFromWindowsMenu( gui );

    // If there is no need for tabs (only one GUI) stop using tabs
    if( tabs->count() == 1 )
        setSingleMode();
}

// Open designer
void MainWindow::on_actionDesigner_triggered()
{
    // Start designer
    QProcess *process = new QProcess();
    process->setWorkingDirectory( profile.getPath() );
     process->start( "designer" );
}

// Open the current form in designer
void MainWindow::on_actionOpen_Current_Form_In_Designer_triggered()
{
    // Get the gui file name (left empthy if no gui)
    QStringList guiFileName;
    ASguiForm* gui = getCurrentGui();
    if( gui )
        guiFileName.append( gui->getGuiFileName() );

    // Start designer specifying the current gui file name
    QProcess *process = new QProcess();
    process->setWorkingDirectory( profile.getPath() );
    process->start( "designer", guiFileName );
}

// Refresh the current window (reload the ui file)
void MainWindow::on_actionRefresh_Current_Form_triggered()
{
    // Get the gui file name (left empty if no gui)
    QString guiFileName;
    ASguiForm* currentGui = getCurrentGui();
    if( currentGui )
        guiFileName = currentGui->getGuiFileName();

    // Recreate the gui and load it in place of the current window
    if( guiFileName.size() )
    {
        ASguiForm* newGui = createGui( guiFileName );
        loadGuiIntoCurrentWindow( newGui );
    }
}

//=================================================================================
// Methods for managing GUI windows
//=================================================================================

/// Open a gui in a new tab
/// Either as a result of the gui user requesting a new tab, or a contained object (gui push button) requesting a new tab
void MainWindow::loadGuiIntoNewTab( ASguiForm* gui )
{
    // Do nothing if couldn't create gui
    if( !gui )
        return;

    // Add a tab
    QTabWidget* tabs = getCentralTabs();
    if( tabs )
    {
        int index = tabs->addTab( gui, gui->getASGuiTitle() );
        tabs->setCurrentIndex( index );
    }
}

/// Open a gui in the current window
/// Either as a result of the gui user requesting a new window, or a contained object (gui push button) requesting a new window
void MainWindow::loadGuiIntoCurrentWindow( ASguiForm* gui )
{
    // Do nothing if couldn't create gui
    if( !gui )
        return;

    // If using tabs, load the gui into the current tab
    if( usingTabs )
    {
        QTabWidget* tabs = getCentralTabs();
        if( tabs )
        {
            int i = tabs->currentIndex();
            tabs->removeTab( i );
            tabs->insertTab( i, gui, gui->getASGuiTitle() );
            tabs->setCurrentWidget( gui );
        }
    }

    // Using a single window, just set the gui as the central widget
    // (note, resize with a timer event to allow all widget sizes to be recalculated before we use them
    else
    {
        setCentralWidget( gui );
        QTimer::singleShot( 1, this, SLOT(resizeToFitGui())); // note 1mS rather than zero. recalculates size correctly if opening a new window from the file menu
    }


    // Set the titles (the main window title, and also the tab if in a tab)
    setTitle( gui->getASGuiTitle() );
/*    if( usingTabs )
    {
        QTabWidget* tabs = getCentralTabs();
        if( tabs )
            tabs->setTabText( tabs->currentIndex(), gui->getASGuiTitle() );
    }
 */
}

//=================================================================================
// Slots for managing resizing
//=================================================================================

// Resize the form.
// This is done as a timer event once all processing has completed after creating a new gui.
void MainWindow::resizeToFitGui()
{
    // Sanity check. Do nothing if the central widget is not a scroll area (actually an ASguiForm class based on a scroll area)
    if ( QString::compare( centralWidget()->metaObject()->className(), "ASguiForm" ) )
        return;

    // Check. Do nothing if nothing is in the scroll area.
    // Unlike the check above, this condition is expected. It can happen
    // if the UI file defining the gui was not located.
    ASguiForm* sa = (ASguiForm*)centralWidget();
    if( !sa->widget() )
        return;

    // Collect relevent dimensions
    int ui_w = sa->widget()->width();
    int ui_h = sa->widget()->height();
    int main_w = width();
    int main_h = height();
    int central_w = centralWidget()->width();
    int central_h = centralWidget()->height();
    int frame_w = sa->frameWidth();

    // The size required is the size of the user interface plus the difference between the main window size and the central widget size
    this->resize( ui_w + main_w - central_w + frame_w * 2, ui_h + main_h - central_h + frame_w * 2 );
}

//=================================================================================
// Slots for presenting warnings, errors, and status messages
//=================================================================================

// Use a status message
void MainWindow::onStatusMessage( QString message )
{
    // Change the message in the status bar
    statusBar()->showMessage( message );
}

// Use a warning message
void MainWindow::onWarningMessage( QString message )
{
    statusBar()->showMessage( message );

/* Message box option
    QMessageBox msgBox;
    msgBox.setText( "Warning" );
    msgBox.setInformativeText( message );
    msgBox.exec();
*/
}

// Use an error message
void MainWindow::onErrorMessage( QString message )
{
    statusBar()->showMessage( message );


/* Message box option
    QMessageBox msgBox;
    msgBox.setText( "Error" );
    msgBox.setInformativeText( message );
    msgBox.exec();
*/
}

//=================================================================================
// Slots and methods for launching new GUIs on behalf of objects in the gui (typically buttons)
//=================================================================================

// Slot for launching a new gui from a contained object.
void MainWindow::launchGui( QString guiName, QString substitutions, ASguiForm::creationOptions createOption )
{
    // Load the new gui as required
    switch( createOption )
    {
        // Open the specified gui in the current window
        case ASguiForm::CREATION_OPTION_OPEN:
            {
                profile.addMacroSubstitutions( substitutions );
                ASguiForm* gui = createGui( guiName );
                loadGuiIntoCurrentWindow( gui );
                profile.removeMacroSubstitutions();
            }
            break;

        // Open the specified gui in a new tab
        case ASguiForm::CREATION_OPTION_NEW_TAB:
            {
                // If not using tabs, start tabs and migrate any single gui to the first tab
                if( !usingTabs )
                    setTabMode();

                // Create the gui and load it into a new tab
                profile.addMacroSubstitutions( substitutions );
                ASguiForm* gui = createGui( guiName );
                loadGuiIntoNewTab( gui );
                profile.removeMacroSubstitutions();
            }
            break;

        // Open the specified gui in a new window
        case ASguiForm::CREATION_OPTION_NEW_WINDOW:
            {
                profile.addMacroSubstitutions( substitutions );
                MainWindow* w = new MainWindow( guiName, path, profile.getMacroSubstitutions(), enableEdit );
                w->show();
                profile.removeMacroSubstitutions();
            }
            break;

        default:
            qDebug() << "MainWindow::launchGui() Unexpected gui creation option: " << createOption;
            break;
    }
}

//=================================================================================
// Methods for common support tasks
//=================================================================================

// Set up to use only a single gui
// This is the case when the main window is started or the number of tabs has reduced to one
void MainWindow::setSingleMode()
{
    // Sanity check
    if( !usingTabs )
        return;

    // Get the tabs widget
    QTabWidget* tabs = getCentralTabs();
    if( !tabs )
        return;

    // Move the gui from the first (and only) tab to be the central widget
    ASguiForm* gui = (ASguiForm*)( tabs->currentWidget() );
    if( gui )
    {
        // Make the gui the central widget.
        // Note, ownership of widgets set as the central widget is claimed by the main window.
        // So when a widget is set as the central widget, any previous central widget is deleted by the main window.
        // As the previous central widget was the tab widget, and gui we are about to set as the central
        // widget is part of the tab widget's hierarchy, there is a risk that the gui may be deleted before
        // it is reparented as part of being set as the central widget. In practice, this does not happen.
        // It appears it is removed from the tab widget hierarchy before the tab widget is deleted.
        // If this proves not be the case, it may be nessesary to reparent the gui widget manually before
        // calling setCentralWidget() here.
        setCentralWidget( gui );

        // Must 'show' the gui following the reparenting inherent in the above invocation of setCentralWidget
        // where the gui is reparented from from the tab widget to the main window.
        gui->show();
    }

    // Flag tabs are no longer in use
    usingTabs = false;
}

// Set up to use multiple guis in tabs
// This is the case when a single scroll area is in use and a new tab has been requested
void MainWindow::setTabMode()
{
    // Sanity check
    if( usingTabs )
        return;

    // Create the tab widget
    QTabWidget* tabs = new QTabWidget;
    tabs->setTabsClosable( true );
    QObject::connect( tabs, SIGNAL( tabCloseRequested ( int ) ), this, SLOT( tabCloseRequest( int ) ) );
    QObject::connect( tabs, SIGNAL( currentChanged ( int ) ), this, SLOT( tabCurrentChanged( int ) ) );

    // If there was a single gui present, move it to the first tab
    ASguiForm* gui = getCentralGui();
    if( gui )
        tabs->addTab( gui, gui->getASGuiTitle() );

    // Start using tabs as the main area of the main window
    setCentralWidget( tabs );
    usingTabs = true;
}

// Get a gui filename from the user
// Although this is only a single line, it standardises the filters and file types used
QString MainWindow::GuiFileNameDialog( QString caption )
{
    // Get the filename
    return QFileDialog::getOpenFileName( this, caption, path, "*.ui" );
}

// Create a gui
// Performs gui opening tasks generic to new guis, including opening a new tab,
// replacing a gui in a tab, replacing a single gui in the main window,
// or creating a gui in a new main window.
ASguiForm* MainWindow::createGui( QString fileName )
{
    // Don't do anything if no filename was supplied
     if (fileName.isEmpty())
        return NULL;

    // Inform user
    onStatusMessage( QString( "Opening %1 in new window " ).arg( fileName ) );

    // Setup the environment profile of the new form
//    ContainerProfile profile;
//    profile.setupProfile( this, this, this, this, path, substitutions, false );

    // Build the gui
    ASguiForm* gui = new ASguiForm( fileName );
    if( gui )
        gui->readUiFile();

//    // Release the environment profile for new QCa wigets
//    profile.releaseProfile();

    // Add the form to the 'windows' menu
    if ( gui )
        addGuiToWindowsMenu( gui );

    // Return the created gui if any
    return gui;
 }

// Set the main window title to 'AS GUI - filename'
void MainWindow::setTitle( QString fileName )
{
    QString title( "AS GUI" );
    if( !fileName.isEmpty() )
        title.append(  " - ").append( fileName );
    setWindowTitle( title );
}

// Return the central widget if it is the tab widget, else return NULL
QTabWidget* MainWindow::getCentralTabs()
{
    QWidget* w = centralWidget();
    if( !w || QString( "QTabWidget").compare( w->metaObject()->className() ) )
        return NULL;
    else
        return (QTabWidget*)w;
}

// Return the central widget if it is a single gui, else return NULL
// Note, originally ASguiForm class did not implement QOBJECT so className() returned it's base class which was QScrollArea.
ASguiForm* MainWindow::getCentralGui()
{
    QWidget* w = centralWidget();
    if( !w || QString( "ASguiForm").compare( w->metaObject()->className() ) )
        return NULL;
    else
        return (ASguiForm*)w;
}

// Get the current gui if any
ASguiForm* MainWindow::getCurrentGui()
{
    // If using tabs, return the current tab if any
    if( usingTabs )
    {
        QTabWidget* tabs = getCentralTabs();
        if( tabs )
            return (ASguiForm*)(tabs->currentWidget());
    }

    // Using a single window, return the gui
    else
    {
        ASguiForm* gui = getCentralGui();
        if( gui )
            return gui;
    }

    // No gui present
    return NULL;
 }

//=================================================================================
// Methods to manage the application wide 'Windows' menu
//=================================================================================
//??? To be tested and debugged. Currently crashing regularly


// Add a gui to the 'windows' menus
// Used when creating a new gui
void MainWindow::addGuiToWindowsMenu( ASguiForm* gui )
{
    //??? To be tested
    return;

    // Add the gui to the list of guis
    guiList.append( gui );
    for( int i = 0; i < mainWindowList.size(); i++ )
    {
        mainWindowList[i]->ui.menuWindows->addAction( gui->getASGuiTitle() );
    }
}

// Build new 'windows' menu
// Used when creating a new main window and there are already other main windows present with GUIs
void MainWindow::buildWindowsMenu()
{
    //??? To be tested
    return;

    // Add all current guis to the 'windows' menu
    for( int i = 0; i < guiList.size(); i++ )
        ui.menuWindows->addAction( guiList[i]->getASGuiTitle() );
}

// Remove a gui from the 'windows' menus
// Used when deleting a single gui
void MainWindow::removeGuiFromWindowsMenu( ASguiForm* gui )
{
    //??? To be tested
    return;

    for( int i = 0; i < mainWindowList.size(); i++ )
    {
        for( int j = 0; j < guiList.size(); j++ )
        {
            if( guiList[j] == gui )
            {
                guiList.removeAt( j );

                QList<QAction*> actions = mainWindowList[i]->ui.menuWindows->actions();
                mainWindowList[i]->ui.menuWindows->removeAction( actions[j] );
                break;
            }
        }
    }
}

// Remove all guis on a main window from the 'windows' menus of all main windows
// Used when deleting a main window
void MainWindow::removeAllGuisFromWindowsMenu()
{
    //??? To be tested
    return;

    // If using tabs, delete the 'windows' menu references for each gui displayed in the tabs
    if( usingTabs )
    {
        QTabWidget* tabs = getCentralTabs();
        if( tabs )
        {
            QList<QObject*> children = tabs->children();
            for( int i = 0; i < children.size(); i++ )
            {
                ASguiForm* gui = (ASguiForm*)children[i];
                    removeGuiFromWindowsMenu( gui );
            }
        }
    }

    // Not using tabs, so just remove the entry for the single gui displayed, if any
    else
    {
        ASguiForm* gui = getCentralGui();
        if( gui )
            removeGuiFromWindowsMenu( gui );
    }
}
