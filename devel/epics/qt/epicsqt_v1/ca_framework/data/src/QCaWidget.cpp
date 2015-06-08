/*! 
  \class QCaWidget
  \version $Revision: #8 $
  \date $DateTime: 2010/09/06 11:58:56 $
  \author anthony.owen
  \brief Template for Qt-CA based widgets.
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
 *    Anthony Owen
 *  Contact details:
 *    anthony.owen@gmail.com
 */

/*!
  This class is used as a base for all CA aware wigets, such as QCaLabel, QCaSpinBox, etc.
  It manages common issues including creating a source of CA data updates, handling error,
  warning and status messages, and setting tool tips based on variable names.

  Note, there is tight integration between the CA aware widget classes, this class, and its
  base classes, especially VariableNameManager and QCaToolTip.

  In particular, this class manages QCaObject classes that stream updates to the
  CA aware widget class. But this class, however, doesn't know how to format the data,
  or how the updates will be used.
  To resolve this, this class asks its parent class (such as QCaLabel) to create the
  QCaObject class in what ever flavour it wants, by calling the virtual function createQcaItem.
  A QCaLabel, for example, wants string updates so it creates a QCaString which is based on a
  QCaObject class and formats all updates as strings.

  The CA aware parent class (such as QCaLabel) defines a variable by calling VariableNameManager::setVariableName().
  The VariableNameManager class calls the establishConnection function of the CA aware parent class, such as QCaLabel
  when it has a new variable name.

  This class uses its base QCaToolTip class to format tool tips. that class in turn calls the CA aware parent class
  (such as QCaLabel) directly to make use of a new tool tip.


  After construction, a CA aware widget is activated (starts updating) by calling it's
  establishConnection() function in one of two ways:

   1) The variable name or variable name substitutions is changed by calling setVariableName
      or setVariableNameSubstitutions respectively. These functions are in the VariableNameManager class.
      The VariableNameManager calls a virtual function establishConnection() which is implemented by the CA aware widget.
      This is how a CA aware widget is activated in 'designer'. It occurs when 'designer' updates the
      variable name property or variable name substitution property.

   2) When an ASguiForm widget is created, resulting in a set of CA aware widgets being created by loading a UI file
      contining plugin definitions.
      After loading the plugin widgets, code in the ASguiForm class calls the activate() function in this class (QCaWiget).
      the activate() function calls  establishConnection() in the CA aware widget for each variable. This simulates
      what the VariableNameManager does as each variable name is entered (see 1, above, for details)

  No matter which way a CA aware widget is activated, the establishConnection() function in the CA aware widget is called
  for each variable. The establishConnection() function asks this QCaWidget base class, by calling the createConnection()
  function, to perform the tasks common to all CA aware widgets for establishing a stream of CA data.

  The createConnection() function sets up the widget 'tool tip', then immedietly calls the CA aware widget back asking it to create
  an object based on QCaObject. This object will supply a stream of CA update signals to the CA aware object in a form that
  it needs. For example a QCaLabel creates a QCaString object. The QCaString class is based on the QCaObject class and converts
  all update data to a strings which is required for updating a Qt label widget. This class stores the QCaObject based class.

  After the establishConnection() function in the CA aware widget has called createConnection(), the remaining task of the
  establishConnection() function is to connect the signals of the newly created QCaObject based classes to its own slots
  so that data updates can be used. For example, a QCaLabel connects the 'stringChanged' signal
  fromthe QCaString object to its setLabelText slot.
 */

#include <QDebug>
#include <QCaWidget.h>

/*!
    Constructor
*/
QCaWidget::QCaWidget() {

    /// Initially flag no variables array is defined.
    /// This will be corrected when the first variable is declared
    numVariables = 0;
    qcaItem = 0;

    /// Default properties
    subscribe = true;
    variableAsToolTip = true;

    // Setup an object to emit message signals according to the profile defined by whatever is creating
    // this widget (or use a default mechanism if no profile has been defined)
    // If there is a profile defining the environment containing this widget, add this widget
    // to the list of contained widgets so whatever is managing the container can activate this widget.
    //
    // Although a widget is self contained, whatever is creating the widget has the option of providing
    // a list of services and other information through a containerProfile that QCaWidgets can use.
    // For example, an application creating QCaWidgets can provide a mechanism to display error
    // messages in a manner appropriate for the application.
    // In this case, the widget is taking the oppertunity to tell its creator it exists, and also to
    // get any variable name macro substitutions offered by its creator.
    userMessage.setup( getStatusMessageConsumer(),
                       getErrorMessageConsumer(),
                       getWarningMessageConsumer() );
    if( isProfileDefined() )
    {
        addContainedWidget( this );
        setVariableNameSubstitutionsOverride( getMacroSubstitutions() );
    }
}

/*!
    Destruction:
    Delete all variable sources for the widgeet
*/
QCaWidget::~QCaWidget() {

    // Delete all the QCaObject instances
    for( unsigned int i = 0; i < numVariables; i++ ) {
        deleteQcaItem( i );
    }

    // Release the list
    delete[] qcaItem;
    qcaItem = NULL;
}

/*!
    Set the number of variables that will be used for this widget.
    Create an array of QCaObject based objects to suit.
    This is called by the CA aware widgets based on this class, such as a QCaLabel.
*/
void QCaWidget::setNumVariables( unsigned int numVariablesIn ) {

    /// Get the number of variables that will be used by this widget
    /// Don't accept zero or the qca array will be invalid
    if( numVariablesIn ) {
        numVariables = numVariablesIn;
    } else {
        numVariables = 1;
    }

    /// Set up the number of variables managed by the variable name manager
    variableNameManagerInitialise( numVariables );

    /// Allocate the array of QCa objects
    qcaItem = new qcaobject::QCaObject* [numVariables];
    for( unsigned int i = 0; i < numVariables; i++ ) {
        qcaItem[i] = NULL;
    }
}

/*!
   Initiate updates.
   This is only required when QCa widgets are loaded within a form and not directly by 'designer'.
   When loaded directly by 'designer' they are activated (a CA connection is established) as soon as either
   the variable name or variable name substitution properties are set
 */
void QCaWidget::activate()
{
    // For each variable, ask the CA aware widget based on this class to initiate updates and to set up
    // whatever signal/slot connections are required to make use of data updates.
    // Note, establish connection is a virtual function of the VariableNameManager class and is normally
    // called by that class when a variable name is defined or changed
    for( unsigned int i = 0; i < numVariables; i++ )
        establishConnection( i );
}


/*!
    Create a CA connection and initiates updates if required.
    This is called by the establishConnection function of CA aware widgets based on this class, such as a QCaLabel.
    If successfull it will return the QCaObject based object supplying data update signals
*/
qcaobject::QCaObject* QCaWidget::createConnection( unsigned int variableIndex ) {

    /// If the index is invalid do nothing
    /// This same test is also valid if qcaItem has never been set up yet as numVariables will be zero
    if( variableIndex >= numVariables ) {
        return NULL;
    }

    // Set the tool tip to the variable names if required
    if( variableAsToolTip ) {
        QString tip;
        for( unsigned int i = 0; i < numVariables; i++ ) {
            /// If a variable name is present, add it to the tip
            QString variableName = getSubstitutedVariableName( i );
            if( variableName.size() ) {
                /// Add a space between variable names
                if( tip.size() > 0 ) {
                    tip.append( QString( " " ) );
                }
                /// Add the variable name to the tip
                tip.append( variableName );
            }
        }

        if( tip.size() == 0 ) {
            tip = "No variables defined";
        }
        updateToolTipVariable( tip );
    }

    /// Remove any existing QCa connection
    deleteQcaItem( variableIndex );

    /// Connect to new variable.
    /// If a new variable name is present, ask the CA aware widget based on this class to create an
    /// appropriate object based on a QCaObject (by calling its createQcaItem() function).
    /// If that is successfull, supply it with a mechanism for handling errors and subscribe
    /// to the new variable if required.
    if( getSubstitutedVariableName( variableIndex ).length() > 0 ) {
        qcaItem[variableIndex] = createQcaItem( variableIndex );
        if( qcaItem[variableIndex] ) {

            qcaItem[variableIndex]->setUserMessage( &userMessage );

            if( subscribe )
                qcaItem[variableIndex]->subscribe();
        }
    }

    // Return the QCaObject, if any
    return qcaItem[variableIndex];
}

/*!
    Return a reference to one of the qCaObjects used to stream CA data updates to the widget
    This is called by CA aware widgets based on this class, such as a QCaLabel, mainly when they
    want to connect to its signals to recieve data updates.
*/
qcaobject::QCaObject* QCaWidget::getQcaItem( unsigned int variableIndex ) {
    /// If the index is invalid return NULL.
    /// This same test is also valid if qcaItem has never been set up yet as numVariables will be zero
    if( variableIndex >= numVariables )
        return NULL;

    // Return the QCaObject used for the specified variable name
    return qcaItem[variableIndex];
}

/*!
    Remove any previous QCaObject created to supply CA data updates for a variable name
*/
void QCaWidget::deleteQcaItem( unsigned int variableIndex ) {
    /// If the index is invalid do nothing.
    /// This same test is also valid if qcaItem has never been set up yet as numVariables will be zero
    if( variableIndex >= numVariables )
        return;

    // Delete the QCaObject used for the specified variable name
    if( qcaItem[variableIndex] )
        delete qcaItem[variableIndex];

    // Remove the reference to the deleted object to prevent accidental use
    qcaItem[variableIndex] = NULL;
}
///----------------------------------- my_ --------------------------------------
/*
void QCaWidget::setBackground(QColor c)
{
    thisBackColor = c;
    setColors(thisBackColor, thisForeColor);
}
///----------------------------------
void QCaWidget::setForeground(QColor c)
{
    thisForeColor = c;
    setColors(thisBackColor, thisForeColor);
}
///----------------------------------
void QCaWidget::setColors(QColor bg, QColor fg)
{
    if((bg != oldBackColor) || (fg != oldForeColor) || (thisStyle != oldStyle) || (thisColorMode != oldColorMode)) {
        QColor defColor = QColor(255, 248, 220, thisBackColor.alpha());  // my default color, we do not use the stylesheet anymore
        QColor lc;
        QColor dc;
        thisStyle = "caTextEntry,caLineEdit {background-color: rgba(%1, %2, %3, %4); color: rgba(%5, %6, %7, %8); border-radius: 1px; }";
        thisStyle.append("caLineEdit {border: 0px; padding: 0px 0px 0px 2px;} caTextEntry { border: 2px; padding: 0px;}");
        if(thisColorMode == Default || thisColorMode == Alarm_Default) {
            thisStyle = thisStyle.arg(defColor.red()).arg(defColor.green()).arg(defColor.blue()).arg(defColor.alpha()).
                    arg(fg.red()).arg(fg.green()).arg(fg.blue()).arg(fg.alpha());

            lc = defColor.lighter();
            dc = defColor.darker();
        } else {
            thisStyle = "caTextEntry, caLineEdit {background-color: rgba(%1, %2, %3, %4); color: rgba(%5, %6, %7, %8); border-radius: 1px; }";
            thisStyle.append("caLineEdit{border: 0px; padding: 0px 0px 0px 2px;} caTextEntry { border: 2px; padding: 0px;} ");
            thisStyle = thisStyle.arg(bg.red()).arg(bg.green()).arg(bg.blue()).arg(bg.alpha()).
                    arg(fg.red()).arg(fg.green()).arg(fg.blue()).arg(fg.alpha());
            lc = bg.lighter();
            dc = bg.darker();
        }
        if(thisStyle != oldStyle || thisColorMode != oldColorMode) {
            thisStyle.append(" caTextEntry {border-style:inset; border-color: rgba(%1, %2, %3, %4) rgba(%5, %6, %7, %8)  rgba(%9, %10, %11, %12) rgba(%13, %14, %15, %16);} caTextEntry:focus {padding: 0px; border: 2px groove darkred; border-radius: 1px;} ");
            thisStyle = thisStyle.arg(dc.red()).arg(dc.green()).arg(dc.blue()).arg(dc.alpha()).
                    arg(lc.red()).arg(lc.green()).arg(lc.blue()).arg(lc.alpha()).
                    arg(lc.red()).arg(lc.green()).arg(lc.blue()).arg(lc.alpha()).
                    arg(dc.red()).arg(dc.green()).arg(dc.blue()).arg(dc.alpha());
            setStyleSheet(thisStyle);
            oldStyle = thisStyle;
        }
    }
    oldBackColor = bg;
    oldForeColor = fg;
}
*/
///------------------------------------------------------------------------------
