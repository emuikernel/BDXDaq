/*! 
  \class ASguiForm
  \version $Revision: #10 $
  \date $DateTime: 2010/09/06 13:16:04 $
  \author andrew.rhyder
  \brief A base form GUI
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

#ifndef ASGUIFORM_H
#define ASGUIFORM_H

#include <QScrollArea>
#include <QWidget>
#include <UserMessage.h>
#include <QString>
#include <QFileSystemWatcher>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT ASguiForm : public QScrollArea
{
    Q_OBJECT
    public:

        ASguiForm( QWidget* parent = 0 );
        ASguiForm( const QString& uifileNameIn, QWidget* parent = 0 );
        void commonInit( const bool alertIfUINoFoundIn );

        ~ASguiForm();

        QString getASGuiTitle();        // Get the title to be used as the window or form title.
        QString getGuiFileName();       // Get the UI file name used to build the gui

        enum creationOptions { CREATION_OPTION_OPEN, CREATION_OPTION_NEW_TAB, CREATION_OPTION_NEW_WINDOW };

        // Property convenience functions

        // Variable name related properties (for a form no variable name will be set, only the substitutions that will be passed on to widgets within the form)
        void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

        // UI file name
        void    setUiFileName( QString uiFile );
        QString getUiFileName();

        // Flag indicating form should handle gui form launch requests
        void setHandleGuiLaunchRequests( bool handleGuiLaunchRequests );
        bool getHandleGuiLaunchRequests();

    public slots:
        void requestEnabled( const bool& state );
        void readUiFile();
        void onGeneralMessage( QString message );

    private slots:
        void onGuiLaunch( QString guiName, QString substitutions, creationOptions createOption );
        void fileChanged ( const QString & path );


    protected:
        QString uiFileName;
        void setVariableNameSubstitutions( QString variableNameSubstitutionsIn );
        bool handleGuiLaunchRequests;

    private:
        QString title;
        QWidget* ui;
        bool alertIfUINoFound;      // True if the UI file could not be read. No alert is required, for example, when a partial UI file name is being typed in Designer
        QFileSystemWatcher fileMon;

        QString variableNameSubstitutions;
        UserMessage userMessage;
        void generalMessage( QString message );
};

#endif // ASGUIFORM_H
