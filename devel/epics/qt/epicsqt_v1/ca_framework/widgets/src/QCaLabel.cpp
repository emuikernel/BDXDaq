/*! 
  \class QCaLabel
  \version $Revision: #15 $
  \date $DateTime: 2010/09/06 11:58:56 $
  \author andrew.rhyder
  \brief CA Label Widget.
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
  This class is a CA aware label widget based on the Qt label widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include <QCaLabel.h>

/*!
    Constructor with no initialisation
*/
QCaLabel::QCaLabel( QWidget *parent ) : QLabel( parent ), QCaWidget() {
    setup();
}

/*!
    Constructor with known variable
*/
QCaLabel::QCaLabel( const QString &variableNameIn, QWidget *parent ) : QLabel( parent ), QCaWidget() {
    setup();
    setVariableName( variableNameIn, 0 );
}

/*!
    Setup common to all constructors
*/
void QCaLabel::setup() {

    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Set up default properties
    caEnabled = true;
    caVisible = true;

    // Set the initial state
    setText( "" );
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // Use label signals
    // --Currently none--
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a label a QCaObject that streams strings is required.
*/
qcaobject::QCaObject* QCaLabel::createQcaItem( unsigned int variableIndex ) {
    // Create the item as a QCaString
   return new QCaString( getSubstitutedVariableName( variableIndex ), this, &stringFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaLabel::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        QObject::connect( qca,  SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setLabelText( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
        QObject::connect( this, SIGNAL( requestResend() ),
                          qca, SLOT( resendLastData() ) );
    }
}


/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaLabel::updateToolTip( const QString& tip )
{
    setToolTip( tip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaLabel::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    /// If connected, enable the widget if the QCa enabled property is true
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        if( caEnabled )
            setEnabled( true );
    }

    /// If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        QWidget::setEnabled( false );
    }
}

/*!
    Update the label text
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QCaLabel::setLabelText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

    /// Signal a database value change to any Link widgets
   // emit dbValueChanged( text ); my:

    /// Update the text
    setText( text );
 //emit dbValueChanged( text ); // my:
    /// If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
/** my:
            updateToolTipAlarm( alarmInfo.severityName() );
            setStyleSheet( alarmInfo.style() );
            lastSeverity = alarmInfo.getSeverity();
*/
    }
 emit dbValueChanged( text ); // my:
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaLabel::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return caEnabled;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaLabel::setEnabled( bool state )
{
    /// Note the new 'enabled' state
    caEnabled = state;

    /// Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( caEnabled );
}
/*!
   Slot similar to default widget setEnabled, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaLabel::requestEnabled( const bool& state )
{
    setEnabled(state);
}

//==============================================================================
// Property convenience functions


// Access functions for variableName and variableNameSubstitutions
// variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
void QCaLabel::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex ) {
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}

// variable as tool tip
void QCaLabel::setVariableAsToolTip( bool variableAsToolTipIn )
{
    variableAsToolTip = variableAsToolTipIn;
}
bool QCaLabel::getVariableAsToolTip()
{
    return variableAsToolTip;
}

// String formatting properties

// precision
void QCaLabel::setPrecision( unsigned int precision )
{
    stringFormatting.setPrecision( precision );
    emit requestResend();
}
unsigned int QCaLabel::getPrecision()
{
    return stringFormatting.getPrecision();
}

// useDbPrecision
void QCaLabel::setUseDbPrecision( bool useDbPrecision )
{
    stringFormatting.setUseDbPrecision( useDbPrecision);
    emit requestResend();
}
bool QCaLabel::getUseDbPrecision()
{
    return stringFormatting.getUseDbPrecision();
}

// leadingZero
void QCaLabel::setLeadingZero( bool leadingZero )
{
    stringFormatting.setLeadingZero( leadingZero );
    emit requestResend();
}
bool QCaLabel::getLeadingZero()
{
    return stringFormatting.getLeadingZero();
}

// trailingZeros
void QCaLabel::setTrailingZeros( bool trailingZeros )
{
    stringFormatting.setTrailingZeros( trailingZeros );
    emit requestResend();
}
bool QCaLabel::getTrailingZeros()
{
    return stringFormatting.getTrailingZeros();
}

// addUnits
void QCaLabel::setAddUnits( bool addUnits )
{
    stringFormatting.setAddUnits( addUnits );
    emit requestResend();
}
bool QCaLabel::getAddUnits()
{
    return stringFormatting.getAddUnits();
}

// localEnumeration
void QCaLabel::setLocalEnumeration( QString localEnumeration )
{
    stringFormatting.setLocalEnumeration( localEnumeration );
    emit requestResend();
}
QString QCaLabel::getLocalEnumeration()
{
    return stringFormatting.getLocalEnumeration();
}

// format
void QCaLabel::setFormat( QCaStringFormatting::formats format )
{
    stringFormatting.setFormat( format );
    emit requestResend();
}
QCaStringFormatting::formats QCaLabel::getFormat()
{
    return stringFormatting.getFormat();
}

// radix
void QCaLabel::setRadix( unsigned int radix )
{
    stringFormatting.setRadix( radix);
    emit requestResend();
}
unsigned int QCaLabel::getRadix()
{
    return stringFormatting.getRadix();
}

// notation
void QCaLabel::setNotation( QCaStringFormatting::notations notation )
{
    stringFormatting.setNotation( notation );
    emit requestResend();
}
QCaStringFormatting::notations QCaLabel::getNotation()
{
    return stringFormatting.getNotation();
}

// visible (widget is visible outside 'Designer')
void QCaLabel::setRunVisible( bool visibleIn )
{
    // Update the property
    caVisible = visibleIn;

    // If a container profile has been defined, then this widget is being used in a real GUI and
    // should be visible or not according to the visible property. (While in Designer it can always be displayed)
    ContainerProfile profile;
    if( profile.isProfileDefined() )
    {
        QWidget::setVisible( caVisible );
    }

}
bool QCaLabel::getRunVisible()
{
    return caVisible;
}

