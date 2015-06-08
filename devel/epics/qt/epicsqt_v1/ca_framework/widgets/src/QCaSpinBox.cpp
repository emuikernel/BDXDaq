/*! 
  \class QCaSpinBox
  \version $Revision: #14 $
  \date $DateTime: 2010/09/06 11:58:56 $
  \author andrew.rhyder
  \brief CA Spinbox Widget.
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

/*!
  This class is a CA aware spin box widget based on the Qt spin box widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include "QCaSpinBox.h"

/*!
    Create a CA aware spin box with no variable name yet
*/
QCaSpinBox::QCaSpinBox( QWidget *parent ) : QSpinBox( parent ), QCaWidget() {
    setup();
}

/*!
    Create a CA aware spin box with a variable name already known
*/
QCaSpinBox::QCaSpinBox( const QString &variableNameIn, QWidget *parent ) : QSpinBox( parent ), QCaWidget() {
    setVariableName( variableNameIn, 0 );

    setup();

}

/*!
    Common construction
*/
void QCaSpinBox::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Initialise the flag indicating the value is being changed programatically (not by the user)
    programaticValueChange = false;

    // Set up default properties
    localEnabled = true;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // Use spin box signals
    QObject::connect( this, SIGNAL( valueChanged( int ) ), this, SLOT( userValueChanged( int ) ) );
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a spin box a QCaObject that streams integers is required.
*/
qcaobject::QCaObject* QCaSpinBox::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaInteger
    return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaSpinBox::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        setValue( 0 );
        QObject::connect( qca,  SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setValueIfNoFocus( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaSpinBox::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaSpinBox::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    /// If connected, enable the widget if the QCa enabled property is true
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        if( localEnabled )
            QWidget::setEnabled( true );
    }

    /// If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        QWidget::setEnabled( false );
    }

    /// ??? not sure if this is right. Added as the record type was comming back as GENERIC::UNKNOWN deep in the write
    /// Start a single shot read if the channel is up (ignore channel down),
    /// This will allow initialisation of the widget using info from the database.
    /// If subscribing, then an update will occur without having to initiated one here.
    /// Note, channel up implies link up
    /// Note, even though there is nothing to do to initialise the spin box if not subscribing, an
    /// initial sing shot read is still performed to ensure we have valid information about the
    /// variable when it is time to do a write.
    if( connectionInfo.isChannelConnected() && !subscribe )
    {
        QCaInteger* qca = (QCaInteger*)getQcaItem(0);
        qca->singleShotRead();
    }
}

/*!
    Pass the update straight on to the SpinBox unless the user is changing it.
    Note, it would not be common to have a user editing a regularly updating
    value. However, this scenario should be allowed for. A reasonable reason
    for a user modified value to update on a gui is if is is written to by
    another user on another gui.
    This is the slot used to recieve data updates from a QCaObject based class.
    This is the slot used to recieve data updates from a QCaObject based class.
*/
void QCaSpinBox::setValueIfNoFocus( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

    /// If not subscribing, then do nothing.
    /// Note, This will still be called even if not subscribing as there may be initial sing shot read
    /// to ensure we have valid information about the variable when it is time to do a write.
    if( !subscribe )
        return;

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( value );

    /// Update the spin box only if the user is not interacting with the object.
    if( !hasFocus() ) {
        programaticValueChange = true;
        setValue( value );
        programaticValueChange = false;
    }

    /// If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
            updateToolTipAlarm( alarmInfo.severityName() );
            setStyleSheet( alarmInfo.style() );
            lastSeverity = alarmInfo.getSeverity();
    }
}

/*!
    The user has changed the spin box.
*/
void QCaSpinBox::userValueChanged( int value ) {
    // If the user is changing the value, write it.
    // Note, the spin box does not appear to have a signal that distinguishes between user changes and programatic changes
    if( !programaticValueChange )
    {
        /// Get the variable to write to
        QCaInteger* qca = (QCaInteger*)getQcaItem(0);

        /// If a QCa object is present (if there is a variable to write to)
        /// then write the value
        if( qca ) {
            qca->writeInteger( (long)value );
        }
    }
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaSpinBox::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return localEnabled;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaSpinBox::setEnabled( bool state )
{
    /// Note the new 'enabled' state
    localEnabled = state;

    /// Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( localEnabled );
}

/*!
   Slot similar to default widget setEnabled slot, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaSpinBox::requestEnabled( const bool& state )
{
    setEnabled(state);
}
//==============================================================================
// Property convenience functions

// Variable name and substitutions
void QCaSpinBox::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
{
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}

// subscribe
void QCaSpinBox::setSubscribe( bool subscribeIn )
{
    subscribe = subscribeIn;
}
bool QCaSpinBox::getSubscribe()
{
    return subscribe;
}

// variable as tool tip
void QCaSpinBox::setVariableAsToolTip( bool variableAsToolTipIn )
{
    variableAsToolTip = variableAsToolTipIn;
}
bool QCaSpinBox::getVariableAsToolTip()
{
    return variableAsToolTip;
}
