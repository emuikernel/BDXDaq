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

/*
 * Description:
 *
 * This class provides a communication mechanism from the code creating QCa widgets to the QCa widgets.
 *
 * When QCa widgets, such as QCaLabel, are created, they need to know environmental
 * information such as what macro substitutions to apply, or where to signal error messages.
 * Also, the code creating the QCa widgets may require a reference to all the created QCa widgets.
 * In some cases this information cannot be passed during construction or set up post construction
 * via a method. For example, when the object is being created from a UI file by Qt. In this case
 * the application code asks Qt to generate objects from a UI file and has no idea what
 * QCa widgets if any have been created.
 *
 * To use this class, an instance of this class is instantiated prior to creating the QCa widgets.
 * Information to be communicated such as message handlers and macro substitutions is set up within this class.
 * Then the QCa widgets are created using a mechanism such as the QUiLoader class.
 *
 * As each QCa widget is created it also instantiates an instance of the ContainerProfile class.
 * If any information has been provided, it can then be used.
 * Note, as a local copy of the environment profile is saved within the container object it is available
 * for as long as the environment container instantiated. The original container defining the environment
 * may only be transitory, but as long ass other contaners were created while the original existed, they
 * will hold a complete record of the environment.
 */

#include <ContainerProfile.h>
#include <QCaWidget.h>
#include <QtDebug>


/// Static variables used to pass information from the creator of QCa widgets to the QCa widgets themselves.
QObject* ContainerProfile::publishedStatusMessageConsumer = NULL;
QObject* ContainerProfile::publishedErrorMessageConsumer = NULL;
QObject* ContainerProfile::publishedWarningMessageConsumer = NULL;

QObject* ContainerProfile::publishedGuiLaunchConsumer = NULL;

QList<QString>   ContainerProfile::publishedMacroSubstitutions;
QList<WidgetRef> ContainerProfile::containedWidgets;
QString          ContainerProfile::publishedPath;

bool ContainerProfile::publishedInteractive = false;

bool ContainerProfile::profileDefined = false;

// Constructor.
// A local copy of the defined profile (if any) is made.
// Note, this does not define a profile. A profile is defined only when ContainerProfile::setupProfile() is called
ContainerProfile::ContainerProfile()
{
    init();
}

// Destructor
// Note, if the profile has been defined (ContainerProfile::setupProfile() has been
// called) this does not release the profile. A profile is released only when
// ContainerProfile::releaseProfile() is called.
ContainerProfile::~ContainerProfile()
{
}

/**
  Setup the environmental profile prior to creating some QCa widgets.
  The new widgets will use this profile to determine their external environment.

  This method locks access to the envionmental profile. ReleaseProfile() must be
  called to release the lock once all QCa widgets have been created.
  */
void ContainerProfile::setupProfile( QObject* publishedStatusMessageConsumerIn,
                                     QObject* publishedErrorMessageConsumerIn,
                                     QObject* publishedWarningMessageConsumerIn,
                                     QObject* guiLaunchConsumerIn,
                                     QString publishedPathIn,
                                     QString publishedMacroSubstitutionsIn,
                                     bool interactiveIn )
{
    publishedStatusMessageConsumer = publishedStatusMessageConsumerIn;
    publishedErrorMessageConsumer = publishedErrorMessageConsumerIn;
    publishedWarningMessageConsumer = publishedWarningMessageConsumerIn;

    publishedGuiLaunchConsumer = guiLaunchConsumerIn;

    publishedPath = publishedPathIn;

    publishedMacroSubstitutions.clear();
    publishedMacroSubstitutions.append( publishedMacroSubstitutionsIn );

    publishedInteractive = interactiveIn;

    profileDefined = true;

    init();
}

void ContainerProfile::init()
{

    // Get a local copy of the current environment profile. This local copy will endure for as long as the creator of this intance keeps the object.
    statusMessageConsumer = publishedStatusMessageConsumer;
    errorMessageConsumer = publishedErrorMessageConsumer;
    warningMessageConsumer = publishedWarningMessageConsumer;

    guiLaunchConsumer = publishedWarningMessageConsumer;

    interactive = publishedInteractive;

    for( int i = 0; i < publishedMacroSubstitutions.size(); i++ ) {
        macroSubstitutions.append( " " );
        macroSubstitutions.append( publishedMacroSubstitutions[i] );
    }

    path = publishedPath;

}


/**
  Extend the macro substitutions currently being used by all new QCaWidgets.
  This is used when a form is created. This allow a form to pass on macro substitutions to the QCa widgets it contains.
  Since it adds to the end of the existing macro substitutions, any substitutions already added by the originating
  container or higher forms take precedence.
  */

void ContainerProfile::addMacroSubstitutions( QString macroSubstitutionsIn )
{
    if( profileDefined  )
        publishedMacroSubstitutions.prepend( macroSubstitutionsIn );
}

/**
  Reduce the macro substitutions currently being used by all new QCaWidgets.
  This is used after a form is created. Any macro substitutions passed on by the form being created are no longer relevent.
  */

void ContainerProfile::removeMacroSubstitutions()
{
    if( profileDefined && !publishedMacroSubstitutions.isEmpty() )
        publishedMacroSubstitutions.removeFirst();
}

/**
  Clears any profile context. Must be called by any code that calls setupProfile() once the profile should no longer be used
  */
void ContainerProfile::releaseProfile()
{
    publishedStatusMessageConsumer = NULL;
    publishedErrorMessageConsumer = NULL;
    publishedWarningMessageConsumer = NULL;

    publishedGuiLaunchConsumer = NULL;

    publishedMacroSubstitutions.clear();
    publishedPath.clear();

    containedWidgets.clear();

    publishedInteractive = false;

    profileDefined = false;
}

/**
  Return the object to emit status message signals to.
  If NULL, there is no object available.
  */
QObject* ContainerProfile::getStatusMessageConsumer()
{
    return statusMessageConsumer;
}

/**
  Return the object to emit error message signals to.
  If NULL, there is no object available.
  */
QObject* ContainerProfile::getErrorMessageConsumer()
{
    return errorMessageConsumer;
}

/**
  Return the object to emit warning message signals to.
  If NULL, there is no object available.
  */
QObject* ContainerProfile::getWarningMessageConsumer()
{
    return warningMessageConsumer;
}

/**
  Return the object to emit GUI launch request signals to.
  If NULL, there is no object available.
  */
QObject* ContainerProfile::getGuiLaunchConsumer()
{
    return guiLaunchConsumer;
}

/**
  Return the path to use for file operations.
  */
QString ContainerProfile::getPath()
{
    return path;
}

/**
  Return the object to emit warning message signals to.
  If NULL, there is no object available.
  */
QString ContainerProfile::getMacroSubstitutions()
{
    return macroSubstitutions;
}

bool ContainerProfile::isProfileDefined()
{
    return profileDefined;
}

void ContainerProfile::addContainedWidget( QCaWidget* containedWidget )
{
    containedWidgets.append( WidgetRef( containedWidget ) );
}

QCaWidget* ContainerProfile::getNextContainedWidget()
{
    if( !containedWidgets.isEmpty() )
        return containedWidgets.takeFirst().getRef();
    else
        return NULL;
}

bool ContainerProfile::isInteractive()
{
    return interactive;
}

QObject* ContainerProfile::replaceGuiLaunchConsumer( QObject* newGuiLaunchConsumerIn )
{
    QObject* savedGuiLaunchConsumer = guiLaunchConsumer;
    publishedGuiLaunchConsumer = newGuiLaunchConsumerIn;
    guiLaunchConsumer = publishedGuiLaunchConsumer;

    return savedGuiLaunchConsumer;
}
