/*! 
  \class QCaComboBox
  \version $Revision: #16 $
  \date $DateTime: 2010/09/06 11:58:56 $
  \author andrew.rhyder
  \brief CA Combobox Widget.
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
 *    andrew.rhyder@synchrotron.org
 */

/*!
  This class is a CA aware combo box widget based on the Qt combo box widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include <QCaComboBox.h>

/*!
    Construct a combo box with no variable specified yet
*/
QCaComboBox::QCaComboBox( QWidget *parent ) : QComboBox( parent ), QCaWidget() {
    setup();
}

/*!
    Construct a combo box with a variable specified
*/
QCaComboBox::QCaComboBox( const QString &variableNameIn, QWidget *parent ) : QComboBox( parent ), QCaWidget() {
    setVariableName( variableNameIn, 0 );

    setup();

}

/*!
    Common construction
*/
void QCaComboBox::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Set up default properties
    useDbEnumerations = true;
    subscribe = false;
    localEnabled = true;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // Use line edit signals
    // Set up to write data when the user changes the value
    QObject::connect( this, SIGNAL( activated ( int ) ), this, SLOT( userValueChanged( int ) ) );
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a Combo box a QCaObject that streams integers is required.
*/
qcaobject::QCaObject* QCaComboBox::createQcaItem( unsigned int variableIndex ) {

    // Create the item as a QCaInteger
    return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaComboBox::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        setCurrentIndex( 0 );
        QObject::connect( qca,  SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setValueIfNoFocus( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
   Act on a connection change.
   Change how the label looks and change the tool tip.
   This is the slot used to recieve connection updates from a QCaObject based class.

   Perform initialisation that can only be completed once data from the variable has been read.
   Specifically, set up the combo box entries to match the enumerated types if required.
   This function is called when the channel is first established to the data. It will also be called if the channel fails
   and recovers. Subsequent calls will do nothing as the combo box is already populated.
*/
void QCaComboBox::connectionChanged( QCaConnectionInfo& connectionInfo )
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

    /// Start a single shot read if the channel is up (ignore channel down),
    /// This will allow initialisation of the widget using info from the database.
    /// If the combo box is already populated, then it has been set up at design time, or this is a subsequent 'channel up'
    /// If subscribing, then an update will occur without having to initiated one here.
    /// Note, channel up implies link up
    if( connectionInfo.isChannelConnected() && count() == 0 && !subscribe )
    {
        QCaInteger* qca = (QCaInteger*)getQcaItem(0);
        qca->singleShotRead();
    }
}

/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaComboBox::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Pass the update straight on to the ComboBox unless the user is changing it.
    Note, it would not be common to have a user editing a regularly updating
    value. However, this scenario should be allowed for. A reasonable reason
    for a user modified value to update on a gui is if is is written to by
    another user on another gui.

    Note, this will still be called once if not subscribing to set up enumeration values.
    See  QCaComboBox::dynamicSetup() for details.
*/

void QCaComboBox::setValueIfNoFocus( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

    // If the combo box is not populated, setup the enumerations if any.
    // If not subscribing, there will still be an initial update to get enumeration values.

    //char tmp[100];
    //strcpy(tmp,value.toAscii());
   // printf("my: QCaComboBox %ld %d\n", value, subscribe);
   // fflush(stdout);

    if( count() == 0 )
    {
        QCaInteger* qca = (QCaInteger*)getQcaItem(0);
        QStringList enumerations = qca->getEnumerations();
        if( useDbEnumerations && enumerations.size() )
        {
            insertItems( 0,enumerations );
        }
    }

    /// If not subscribing, then do nothing.
    /// Note, This will still be called even if not subscribing as there may be initial sing shot read
    /// to ensure we have valid information about the variable when it is time to do a write.
    if( !subscribe ){
//printf("my===== return: QCaComboBox %ld\n", value);
        return;
    }

    // Signal a database value change to any Link widgets
 ///   emit dbValueChanged( value ); my:

    /// Update the text if appropriate
    /// If the user is editing the object then updates will be inapropriate
    if( hasFocus() == false )
    {  // printf("my==: QCaComboBox %ld\n", value);
        setCurrentIndex( value );
    }
emit dbValueChanged( value ); /// my: 
    /// If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
            updateToolTipAlarm( alarmInfo.severityName() );
            setStyleSheet( alarmInfo.style() );
            lastSeverity = alarmInfo.getSeverity();
    }
}

/*!
    The user has changed the Combo box.
*/
void QCaComboBox::userValueChanged( int value ) {

    /// Get the variable to write to
    QCaInteger* qca = (QCaInteger*)getQcaItem(0);

    /// If a QCa object is present (if there is a variable to write to)
    /// then write the value
    if( qca ) {
        qca->writeInteger( (long)value );
    }
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaComboBox::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return localEnabled;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaComboBox::setEnabled( bool state )
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
void QCaComboBox::requestEnabled( const bool& state )
{
    setEnabled(state);
}

//==============================================================================
// Property convenience functions

// Variable Name and substitutions
void QCaComboBox::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
{
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}

// subscribe
void QCaComboBox::setSubscribe( bool subscribeIn )
{
    subscribe = subscribeIn;
}
bool QCaComboBox::getSubscribe()
{
    return subscribe;
}

// variable as tool tip
void QCaComboBox::setVariableAsToolTip( bool variableAsToolTipIn )
{
    variableAsToolTip = variableAsToolTipIn;
}
bool QCaComboBox::getVariableAsToolTip()
{
    return variableAsToolTip;
}

// use database enumerations
void QCaComboBox::setUseDbEnumerations( bool useDbEnumerationsIn )
{
    useDbEnumerations = useDbEnumerationsIn;
}
bool QCaComboBox::getUseDbEnumerations()
{
    return useDbEnumerations;
}

