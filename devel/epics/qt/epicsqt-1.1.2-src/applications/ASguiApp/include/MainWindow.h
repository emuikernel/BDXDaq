/*!
  \class
  \version $Revision: #7 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief The Australian Sysnchrotron Base Form GUI
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <ui_MainWindow.h>
#include <ASguiForm.h>
#include <UserMessage.h>
#include <ContainerProfile.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0 );
    MainWindow( QString fileName, QString path, QString substitutions, bool editEnabled, QWidget *parent = 0 );

    ~MainWindow();

private:
    void init( QString fileName, QString pathIn, QString substitutionsIn, bool enableEditIn );   // Code common to all constructors
    bool enableEdit;                                        // Enable edit menu
    Ui::MainWindowClass ui;                                 // Main window layout
    static QList<ASguiForm*> guiList;                       // Shared list of all forms being displayed in all main windows
    static QList<MainWindow*> mainWindowList;               // Shared list of all main windows
    bool usingTabs;                                         // True if using tabs to display multiple GUIs, false if displaying a single GUI

    void setSingleMode();                                   // Set up to use only a single gui
    void setTabMode();                                      // Set up to use multiple guis in tabs
    ASguiForm* createGui( QString filename );               // Create a gui
    void loadGuiIntoCurrentWindow( ASguiForm* newGui );     // Load a new gui into the current window (either single window, or tab)
    void loadGuiIntoNewTab( ASguiForm* gui );               // Load a new gui into a new tab

    void setTitle( QString fileName );                      // Set the main window title to 'AS GUI - filename'

    QTabWidget* getCentralTabs();                           // Return the central widget if it is the tab widget
    ASguiForm* getCentralGui();                             // Return the central widget if it is a single gui, else return NULL
    ASguiForm* getCurrentGui();                             // Return the current gui if any (central, or tab)
    void refresh();                                         // Reload the current gui

    void addGuiToWindowsMenu( ASguiForm* gui );             // Add a gui to the 'windows' menus
    void buildWindowsMenu();                                // Build a new 'windows' menu
    void removeGuiFromWindowsMenu( ASguiForm* gui );        // Remove a gui from the 'windows' menus
    void removeAllGuisFromWindowsMenu();                    // Remove all guis on a main window from the 'windows' menus

    QString GuiFileNameDialog( QString caption );           // Get a gui filename from the user
    QString path;                                           // Default path when looking for GUI ui files
    QString substitutions;                                  // Default substitutions when creating a new GUI form
    ContainerProfile profile;                               // Environment profile for new QCa wigets


private slots:
    void on_actionRefresh_Current_Form_triggered();             // Slot to perform 'Refresh Current Form' action
    void on_actionOpen_Current_Form_In_Designer_triggered();    // Slot to perform 'Open Current Form In Designer' action
    void on_actionDesigner_triggered();                         // Slot to perform Open Designer' action
    void on_actionNew_Window_triggered();           // Slot to perform 'New Window' action
    void on_actionNew_Tab_triggered();              // Slot to perform 'New Tab' action
    void on_actionOpen_triggered();                 // Slot to perform 'Open' action
    void on_actionClose_triggered();                // Slot to perform 'Close' action
    void onWindowMenuSelection( QAction* action );  // Slot to recieve requests to change focus to a specific gui
    void on_actionAbout_triggered();                // Slot to perform 'About' action

    void onStatusMessage( QString message );        // Slot to recieve status messages from GUIs
    void onWarningMessage( QString message );       // Slot to recieve warning messages from GUIs
    void onErrorMessage( QString message );         // Slot to recieve error messages from GUIs

    void launchGui( QString guiName, QString substitutions, ASguiForm::creationOptions creationOption );

    void tabCurrentChanged( int index );            // Slot to act on user changing tabs
    void tabCloseRequest( int index );              // Slot to act on user closing a tab
    void resizeToFitGui();                          // Resize the form to fit a GUI without scroll bars

};

#endif /// MAINWINDOW_H
