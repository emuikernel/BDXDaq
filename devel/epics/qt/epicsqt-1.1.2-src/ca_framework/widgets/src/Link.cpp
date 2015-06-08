/*! 
  \class Link
  \version $Revision: #7 $
  \date $DateTime: 2010/09/06 11:58:56 $
  \author andrew.rhyder
  \brief Link Widget.
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

#include <Link.h>
#include <ContainerProfile.h>
#include <QVariant>
#include <QString>
#include <QDebug>

/*!
    Constructor with no initialisation
*/
Link::Link( QWidget *parent ) : QLabel( parent ) {

    // If a container profile has been defined, then this widget is being used in a real GUI and
    // should be visible or not according to the visible property. (While in Designer it can always be displayed)
    ContainerProfile profile;
    if( profile.isProfileDefined() )
        setVisible( visible );

    // Set default properties
    setText( "Link" );

    signalFalse = true;
    signalTrue = true;

    visible = false;

    condition = CONDITION_EQ;
}

// Common comparison. Macro to evaluate the 'in' signal value.
// Determine if the 'in' signal value matches the condition
// If match and signaling on a match, then send a signal
// If not a match and signaling on no match, then send a signal
#define EVAL_CONDITION                                              \
                                                                    \
    bool match = false;                                             \
    switch( condition )                                     \
    {                                                               \
        case CONDITION_EQ: if( in == val ) match = true; break;     \
        case CONDITION_NE: if( in != val ) match = true; break;     \
        case CONDITION_GT: if( in >  val ) match = true; break;     \
        case CONDITION_GE: if( in >= val ) match = true; break;     \
        case CONDITION_LT: if( in <  val ) match = true; break;     \
        case CONDITION_LE: if( in <= val ) match = true; break;     \
    }                                                               \
                                                                    \
    sendValue( match );

// Slot to perform a comparison on a bool
void Link::in( const bool& in )
{
    bool val = comparisonValue.toBool();
    EVAL_CONDITION;
}

// Slot to perform a comparison on an integer
void Link::in( const qlonglong& in )
{
    qlonglong val = comparisonValue.toLongLong();
    EVAL_CONDITION;
}

// Slot to perform a comparison on a floating point number
void Link::in( const double& in )
{
    double val = comparisonValue.toDouble();
    EVAL_CONDITION;
}

// Slot to perform a comparison on a string
void Link::in( const QString& in )
{
    bool stringIsNum = false;

    // If the string is a valid number, compare it as a number

    QStringList inList = in.split(" ", QString::SkipEmptyParts);
    if( inList.size() )
    {
        double inDouble = inList[0].toDouble( &stringIsNum );
        if( stringIsNum )
        {
            this->in( inDouble );
        }
    }

    // If the string is not a valid number, do a string comparison
    if( !stringIsNum )
    {
        QString val = comparisonValue.toString();
        EVAL_CONDITION;
    }
}

// Generate appropriate signals following a comparison of an input value
void Link::sendValue( bool match )
{
    // If input comparison matched, emit the appropriate value if required
    if( match )
    {
        if( signalTrue )
            emitValue( outTrueValue );
    }

    // If input comparison did not match, emit the appropriate value if required
    else
    {
        if( signalFalse )
            emitValue( outFalseValue );
    }
}

// Emit signals required when input value matches or fails to match
void Link::emitValue( QVariant value )
{
    emit out( value.toBool() );
    emit out( value.toLongLong() );
    emit out( value.toDouble() );
    emit out( value.toString() );
}

// Slot to allow signal/slot manipulation of the auto fill background attribute of the base label class
void Link::autoFillBackground( const bool& enable )
{
    setAutoFillBackground( enable );
}

//==============================================================================
// Property convenience functions

// condition
void Link::setCondition( conditions conditionIn )
{
    condition = conditionIn;
}
Link::conditions Link::getCondition()
{
    return condition;
}

// comparisonValue Value to compare input signals to
void    Link::setComparisonValue( QString comparisonValueIn )
{
    comparisonValue = QVariant(comparisonValueIn);
}
QString Link::getComparisonValue()
{
    return comparisonValue.toString();
}

// signalTrue (Signal if condition is met)
void Link::setSignalTrue( bool signalTrueIn )
{
    signalTrue = signalTrueIn;
}
bool Link::getSignalTrue()
{
    return signalTrue;
}

// signalFalse (Signal if condition not met)
void Link::setSignalFalse( bool signalFalseIn )
{
    signalFalse = signalFalseIn;
}
bool Link::getSignalFalse()
{
    return signalFalse;
}

// outTrueValue Value to emit if condition is met
void    Link::setOutTrueValue( QString outTrueValueIn )
{
    outTrueValue = QVariant(outTrueValueIn);
}
QString Link::getOutTrueValue()
{
    return outTrueValue.toString();
}

// outFalseValue Value to emit if condition is not met
void    Link::setOutFalseValue( QString outFalseValueIn )
{
    outFalseValue = QVariant(outFalseValueIn);
}
QString Link::getOutFalseValue()
{
    return outFalseValue.toString();
}

// visible (widget is visible outside 'Designer')
void Link::setRunVisible( bool visibleIn )
{
    // Update the property
    visible = visibleIn;

    // If a container profile has been defined, then this widget is being used in a real GUI and
    // should be visible or not according to the visible property. (While in Designer it can always be displayed)
    ContainerProfile profile;
    if( profile.isProfileDefined() )
    {
        setVisible( visible );
    }

}
bool Link::getRunVisible()
{
    return visible;
}
