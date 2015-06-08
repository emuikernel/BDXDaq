/*! 
  \class QCaStringFormatting
  \version $Revision: #15 $
  \date $DateTime: 2010/08/30 16:37:08 $
  \author andrew.rhyder
  \brief Format the string for QCaString data.
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

#include <QtDebug>
#include <QCaStringFormatting.h>

/*!
    Construction
*/
QCaStringFormatting::QCaStringFormatting() {
    // Set up the stream that will perform most conversions
    stream.setString( &outStr );

    // Set up default formatting behaviour
    useDbPrecision = true;
    precision = 4;
    leadingZero = true;
    trailingZeros = true;
    format = FORMAT_DEFAULT;
    stream.setIntegerBase( 10 );
    stream.setRealNumberNotation( QTextStream::FixedNotation );
    addUnits = true;

    // Initialise database information
    dbPrecision = 0;
    dbVariableIsStatField = false;
}

/*!
    Set up the precision specified by the database.
    This precision is used when formatting floating point numbers if 'useDbPrecision' is set
*/
void QCaStringFormatting::setDbPrecision( unsigned int dbPrecisionIn )
{
    dbPrecision = dbPrecisionIn;
}

/*!
    Set up the engineering units that will be added to or removed from strings if 'addUnits' flag is set
*/
void QCaStringFormatting::setDbEgu( QString egu )
{
    dbEgu = egu;
}

/*!
    Set up the enumeration values. Thses are used if avaiable if the formatting is FORMAT_DEFAULT
*/
void QCaStringFormatting::setDbEnumerations( QStringList enumerations )
{
    dbEnumerations = enumerations;
}

/*!
    Set up a flag indicating the variable represents a STAT field.
    This is required as formatting enumerated strings for the STAT field is a special case.
*/
void QCaStringFormatting::setDbVariableIsStatField( bool isStatField )
{
    dbVariableIsStatField = isStatField;
}


/*!
    Generate a value given a string, using formatting defined within this
    class.
*/
QVariant QCaStringFormatting::formatValue( const QString &text ) {
    //??? should know the record type (passed in to QCaStringFormatting::formatValue() )

    // Strip unit if present
    QString unitlessText = text.trimmed();
    if( addUnits )
    {
        if( dbEgu == unitlessText.right( dbEgu.length() ) )
          unitlessText.chop( dbEgu.length() );
    }

    //??? interpret the string according to the formatting. for example, if
    //??? formatting says unsigned integer, fail if it looks like a floating,
    //??? negative, text, etc.
    //??? then convert to the record type and return

    /// ??? Temporarily use the variants text convertion.
    QVariant value( unitlessText );

    return value;
}

/*!
    Generate a string given a value, using formatting defined within this class.
*/
QString QCaStringFormatting::formatString( const QVariant &value ) {
    // Initialise
    bool errorMessage = false;      // Set if an error message is the result
    outStr.clear();

    // Set the precision
    if( useDbPrecision )
        stream.setRealNumberPrecision( dbPrecision );
    else
        stream.setRealNumberPrecision( precision );

    // Format the value as requested
    switch( format )
    {
        // Determine the format from the variant type.
        // Only the types used to store ca data are used. any other type is
        // considered a failure.
        case FORMAT_DEFAULT :
        {
            bool haveEnumeratedString = false;  // Successfully converted the value to an enumerated string
            // If a list of enumerated strings is available, attempt to get an enumerated string
            if( dbEnumerations.size() )
            {
                // Ensure the input value can be used as an index into the list of enumerated strings
                bool convertOk;
                long lValue = value.toLongLong( &convertOk );
                if( convertOk && lValue >= 0 )
                {
                    // Get the appropriate enumerated string
                    if( lValue < dbEnumerations.size() )
                    {
                        outStr = dbEnumerations[lValue];
                        haveEnumeratedString = true;
                    }
                    // If the index was higher than the available enumerated strings, handle one special case:
                    // If the field is the STAT field, and the value is between 16 and 21, then use hard coded enumerated strings.
                    // This is due to a limit in EPICS. The STAT field which has 21 enumerated values, but only 16 enumerated strings can be included in the record.
                    else if( lValue >= 16 && lValue <= 21 && dbVariableIsStatField )
                    {
                        switch( lValue )
                        {
                            case 16:
                                outStr = "BAD_SUB";
                                haveEnumeratedString = true;
                                break;

                            case 17:
                                outStr = "UDF";
                                haveEnumeratedString = true;
                                break;

                            case 18:
                                outStr = "DISABLE";
                                haveEnumeratedString = true;
                                break;

                            case 19:
                                outStr = "SIMM";
                                haveEnumeratedString = true;
                                break;

                            case 20:
                                outStr = "READ_ACCESS";
                                haveEnumeratedString = true;
                                break;

                            case 21:
                                outStr = "WRITE_ACCESS";
                                haveEnumeratedString = true;
                                break;

                        }
                    }
                }
            }

            // If no list of enumerated strings was available, or a string could not be selected,
            // convert the value based on it's type.
            if( !haveEnumeratedString )
            {
                switch( value.type() ) {
                    case QVariant::Double:
                        formatFromFloating( value );
                        break;

                    case QVariant::List :
                        {
                            const QVariantList valueArray = value.toList();
                            formatFromFloating( valueArray[0].toDouble() );
                        }
                        break;

                case QVariant::LongLong:
                        formatFromInteger( value, false );
                        break;

                    case QVariant::ULongLong:
                        formatFromUnsignedInteger( value );
                        break;

                    case QVariant::String:
                        stream << value.toString(); // No conversion requried. Stored in variant as required type
                        break;

                    default:
                        formatFailure( QString( "Bug in QCaStringFormatting::formatString(). The QVariant type was not expected" ) );
                        errorMessage = true;
                        break;
                }
            }
            break;
        }

        // Format as requested, ignoring the database type
        case FORMAT_FLOATING:
            formatFromFloating( value );
            break;

         case FORMAT_INTEGER:
            formatFromInteger( value, false );
            break;

        case FORMAT_UNSIGNEDINTEGER:
            formatFromUnsignedInteger( value );
            break;

        case FORMAT_LOCAL_ENUMERATE:
            formatFromInteger( value, true );
            break;

        case FORMAT_TIME:
            formatFromTime( value );
            break;


        // Don't know how to format.
        // This is a code error. All cases in QCaStringFormatting::formats should be catered for
        default:
            formatFailure( QString( "Bug in QCaStringFormatting::format(). The format type was not expected" ) );
            errorMessage = true;
            break;
    }

    // Add units if required, if there are any present, and if the text is not an error message
    int eguLen = dbEgu.length(); // ??? Why cant this be in the 'if' statement? If it is it never adds an egu
    if( addUnits && !errorMessage && eguLen )
        stream << " " << dbEgu;

    return outStr;
}

/*!
    Format a variant value as a string representation of a floating point
    number.
    First convert the variant value to a double. It may or may not be a
    floating point type variant. If it is - good,
    there will be no conversion problems.
    Then format it as a string using the formatting information stored in this
    class.
*/
void QCaStringFormatting::formatFromFloating( const QVariant &value ) {
    // Extract the value as a double using whatever conversion the QVariant uses.
    //
    // Note, this will not pick up if the QVariant type is not one of the types used to represent CA data.
    // This is OK as it is not absolutely nessesary to do this sort of check at this point. Also the code is more robust as it will
    // work if the range of QVariant types used expands.
    // Note, this does not give us the freedom to specify what conversions should fail or succeed. For example, does QVariant::toDouble()
    // work if the value it holds is the string 1.234 10^6, or does it work for both - 1.234 and -1.234, and should it?
    // If QVariant::toDouble() does not do exactly what is required, a switch statement for each of the types used to hold CA data
    // will need to be added and the conversions done  manually or using QVariant::toDouble() as required.
    bool convertOk;
    double dValue = value.toDouble( &convertOk );

    if( !convertOk ) {
        formatFailure( QString( "Warning from QCaStringFormatting::formatFromFloating(). A variant could not be converted to a long." ) );
        return;
    }

    // Generate the text
    stream << dValue;

    // Remove leading zero if required
    if( !leadingZero ) {
        if( outStr.left(2) == "0." )
            outStr = outStr.right( outStr.length()-1);
    }

    // Remove trailing zeros if required
    if( !trailingZeros ) {
        int numZeros = 0;
        for( int i = outStr.length()-1; i >= 0; i-- ) {
            if( outStr[i] != '0' )
                break;
            numZeros++;
        }
        outStr.chop( numZeros );
    }
}

/*!
    Format a variant value as a string representation of a signed integer.
    This method was written to convert a QVariant of type LongLong, but should cope with a variant of any type.
    First convert the variant value to a long. It may or may not be a longlong type variant. If it is - good,
    there will be no conversion problems.
    Then format it as a string using the formatting information stored in this class.
*/
void QCaStringFormatting::formatFromInteger( const QVariant &value, const bool doLocalEnumeration ) {
    // Extract the value as a long using whatever conversion the QVariant uses.
    //
    // Note, this will not pick up if the QVariant type is not one of the types used to represent CA data.
    // This is OK as it is not absolutely nessesary to do this sort of check at this point. Also the code is more robust as it will
    // work if the range of QVariant types used expands.
    // Note, this does not give us the freedom to specify what conversions should fail or succeed. For example, does QVariant::toLongLong()
    // work if the value it holds is the string 1.000 and should it?
    // If QVariant::toLongLong() does not do exactly what is required, a switch statement for each of the types used to hold CA data
    // will need to be added and the conversions done  manually or using QVariant::toLongLong() as required.
    bool convertOk;
    long lValue = value.toLongLong( &convertOk );

    if( !convertOk )
    {
        formatFailure( QString( "Warning from QCaStringFormatting::formatFromInteger(). A variant could not be converted to a long." ) );
        return;
    }


    // Perform any required local enumeration
    if( doLocalEnumeration )
    {
        // Search for a matching value in the list of local enumerated strings
        int i;
        for( i = 0; i < localEnumeration.size(); i++ )
        {
            // Determine if the value matches an enumeration
            bool match = false;
            switch( localEnumeration[i].op )
            {
                case localEnumerationItem::LESS:
                    if( lValue < localEnumeration[i].value )
                        match = true;
                    break;

                case localEnumerationItem::LESS_EQUAL:
                    if( lValue <= localEnumeration[i].value )
                        match = true;
                    break;

                case localEnumerationItem::EQUAL:
                    if( lValue == localEnumeration[i].value )
                        match = true;
                    break;

                case localEnumerationItem::NOT_EQUAL:
                    if( lValue != localEnumeration[i].value )
                        match = true;
                    break;

                case localEnumerationItem::GREATER_EQUAL:
                    if( lValue >= localEnumeration[i].value )
                        match = true;
                    break;

                case localEnumerationItem::GREATER:
                    if( lValue > localEnumeration[i].value )
                        match = true;
                    break;

                case localEnumerationItem::ALWAYS:
                    match = true;
                    break;

                default:
                    match = false;
                    break;

            }

            // If the value does match, use the enumeration value
            if( match )
            {
                stream << localEnumeration[i].text;
                break;
            }
        }

        // If no match was found, generate the text directly from the value
        if( i >= localEnumeration.size() )
            stream << lValue;
    }

    // No local enumeration required, so generate the text directly from the value
    else
    {
        stream << lValue;
    }
}

/*!
    Format a variant value as a string representation of an unsigned integer.
    This method was written to convert a QVariant of type ULongLong, but should cope with a variant of any type.
    First convert the variant value to an unsigned long. It may or may not be a ulonglong type variant. If it is - good,
    there will be no conversion problems.
    Then format it as a string using the formatting information stored in this class.
*/
void QCaStringFormatting::formatFromUnsignedInteger( const QVariant &value ) {
    // Extract the value as an unsigned long using whatever conversion the QVariant uses.
    //
    // Note, this will not pick up if the QVariant type is not one of the types used to represent CA data.
    // This is OK as it is not absolutely nessesary to do this sort of check at this point. Also the code is more robust as it will
    // work if the range of QVariant types used expands.
    // Note, this does not give us the freedom to specify what conversions should fail or succeed. For example, does QVariant::toULongLong()
    // work if the value it holds is the string 1.000 and should it?
    // If QVariant::toULongLong() does not do exactly what is required, a switch statement for each of the types used to hold CA data
    // will need to be added and the conversions done  manually or using QVariant::toULongLong() as required.
    bool convertOk;
    unsigned long ulValue = value.toULongLong( &convertOk );

    if( !convertOk )
    {
        formatFailure( QString( "Warning from QCaStringFormatting::formatFromUnsignedInteger(). A variant could not be converted to an unsigned long." ) );
        return;
    }

    // Generate the text
    stream << ulValue;
}

/*!
    Format a variant value as a string representation of time.
    This method was written to convert a QVariant of type ??? (the type used to represent times in CA),
    but should cope with a variant of any type.
*/
void QCaStringFormatting::formatFromTime( const QVariant &value ) {
    if( value.type() == QVariant::String )
        stream << value.toString();
    else
        stream << QString( "notImplemented" ); //??? to do
}

/*!
    Do something with the fact that the value could not be formatted as
    requested.
*/
void QCaStringFormatting::formatFailure( QString message ) {
    // Log the format failure if required.
    //???
    qDebug() << message;

    // Return whatever is required for a formatting falure.
    stream << "---";
}

//========================================================================================
// 'Set' formatting configuration methods

/*!
    Set the precision - the number of significant digits displayed when
    formatting a floating point number.
    Relevent when formatting the string as a floating point number.
    Note, this will only be used if 'useDbPrecision' is false.
*/
void QCaStringFormatting::setPrecision( unsigned int precisionIn ) {
    precision = precisionIn;
}

/*!
    Set or clear a flag to ignore the precision held by this class and use the
    precision read from the database record.
    The precision is the number of significant digits displayed when formatting
    a floating point number.
    Relevent when formatting the string as a floating point number.
*/
void QCaStringFormatting::setUseDbPrecision( bool useDbPrecisionIn ) {
    useDbPrecision = useDbPrecisionIn;
}

/*!
    Set or clear a flag to display a leading zero before a decimal point for
    floating point numbers between 1 and -1
    Relevent when formatting the string as a floating point number.
*/
void QCaStringFormatting::setLeadingZero( bool leadingZeroIn ) {
    leadingZero = leadingZeroIn;
}

/*!
    Set or clear a flag to display trailing zeros at the end up the precision for
    floating point
    Relevent when formatting the string as a floating point number.
*/
void QCaStringFormatting::setTrailingZeros( bool trailingZerosIn ) {
    trailingZeros = trailingZerosIn;
}

/*!
    Set the type of information being displayed (floating point number,
    date/time, etc).
*/
void QCaStringFormatting::setFormat( formats formatIn ) {
    format = formatIn;
}

/*!
    Set the numer system base.
    Relevent when formatting the string as an interger of floating point
    number.
    Any radix of 2 or more is accepted. Check the conversion code that uses
    this number to see what values are expected.
    At the time of writing (16/2/9) it is anticipated that floating point
    numbers will always be base 10 and integer numbers will
    be base 2, 8, 10, or 16.
*/
void QCaStringFormatting::setRadix( unsigned int radix ) {
    if( radix >= 2 )
        stream.setIntegerBase( radix );
}

/*!
    Set the notation (floating, scientific, or automatic)
    Note, the notation enumerations match the QTextStream RealNumberNotation values
*/
void QCaStringFormatting::setNotation( notations notation ) {
    stream.setRealNumberNotation( (QTextStream::RealNumberNotation)notation );
}

/*!
    Set or clear a flag to include the engineering units in a string
*/
void QCaStringFormatting::setAddUnits( bool AddUnitsIn ) {
    addUnits = AddUnitsIn;
}

/*!
    Set the string used to specify local enumeration.
    This is used when a value is to be enumerated and the value is either not the VAL field,
    or the database does not provide any enumeration, or the database enumeration is not appropriate
*/
void QCaStringFormatting::setLocalEnumeration( QString/*localEnumerationList*/ localEnumerationIn ) {

    // Save the original local enumeration string.
    // This is returned when the enumeration is requested as a property.
    localEnumerationString = localEnumerationIn;


    // Parse the local enumeration string.
    //
    // Format is:
    //
    //  [[<|<=|=|!=|>=|>]value1|*] : string1 , [[<|<=|=|!=|>=|>]value2|*] : string2 , [[<|<=|=|!=|>=|>]value3|*] : string3 , ...
    //
    // Where:
    //   <  Less than
    //   <= Less than or equal
    //   =  Equal (default if no operator specified)
    //   >= Greather than or equal
    //   >  Greater than
    //   *  Always match (used to specify default text)
    //
    // Values do not have to be in any order, but first match wins
    // Consecutive values do not have to be present.
    // Operator is assumed to be equality if not present.
    // White space is ignored except within quoted strings.
    // \n may be included in a string to indicate a line break
    //
    // Examples are:
    //
    // 0:Off,1:On
    // 0 : "Pump Running", 1 : "Pump not running"
    // 0:"", 1:"Warning!\nAlarm"
    // <2:"Value is less than two", =2:"Value is equal to two", >2:"Value is grater than 2"
    // 3:"Beamline Available", *:""
    //
    // The data value is converted to a string if no enumeration for that value is available.
    // For example, if the local enumeration is '0:off,1:on', and a value of 10 is processed, the text generated is '10'.
    // If a blank string is required, this should be explicit. for example, '0:off,1:on,10:""'

    // A range of numbers can be covered by a pair of values as in the following example: >=4:"Between 4 and 8",<=8:"Between 4 and 8"

    localEnumerationItem item;

    enum states { STATE_START, STATE_OPERATOR, STATE_VALUE, STATE_DELIMITER, STATE_START_QUOTE, STATE_UNQUOTED_TEXT, STATE_QUOTED_TEXT, STATE_END_QUOTE, STATE_COMMA, STATE_END };

    int start = 0;                          // Index into enumeration text of current item of interest.
    int len = 0;                            // Length of current item of interest
    int state = STATE_OPERATOR;             // Current state of finite state table
    int size = localEnumerationIn.size();   // Length of local enumeration string to be processed

    // Start with no enumerations
    localEnumeration.clear();

    // Process the enumeration text using a finite state table.
    while( start+len <= size )
    {
        switch( state )
        {
            // Reading the optional operator before the value and it's enumeration string.  For example, the '=' in '0=on,1=off'
            case STATE_OPERATOR:
                // If nothing left, finish
                if( start >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( len == 0 && localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If more operator characters, continue
                if( localEnumerationIn[start+len] == '>' ||
                    localEnumerationIn[start+len] == '<' ||
                    localEnumerationIn[start+len] == '=' ||
                    localEnumerationIn[start+len] == '!' ||
                    localEnumerationIn[start+len] == '*' )
                {
                    len++;
                    break;
                }

                // If an operator is found, use it
                if( len )
                {
                    // Assume operation will not be valid
                    item.op = localEnumerationItem::UNKNOWN;

                    // Interpret operator
                    switch( len )
                    {
                        case 1: // single character operator <, >, =
                            switch( localEnumerationIn[start].toAscii() )
                            {
                                case '<': item.op = localEnumerationItem::LESS;    break;
                                case '>': item.op = localEnumerationItem::GREATER; break;
                                case '=': item.op = localEnumerationItem::EQUAL;   break;
                                case '*': item.op = localEnumerationItem::ALWAYS;   break;
                            }
                            break;

                        case 2: // double character operator (always ending in '=') <=, >=, !=
                            if( localEnumerationIn[start+1] == '=' )
                            {
                                switch( localEnumerationIn[start].toAscii() )
                                {
                                    case '<': item.op = localEnumerationItem::LESS_EQUAL;    break;
                                    case '>': item.op = localEnumerationItem::GREATER_EQUAL; break;
                                    case '!': item.op = localEnumerationItem::NOT_EQUAL;     break;
                                }
                            }
                            break;

                        default:
                            // Error do no more
                            state = STATE_END;
                            break;
                    }

                    // Step over operator onto the value if a comparison is to be made, or the text is there is no value expected
                    start += len;
                    len = 0;
                    if( item.op == localEnumerationItem::ALWAYS )
                        state = STATE_DELIMITER;
                    else
                        state = STATE_VALUE;
                    break;
                }

                // No operator - assume equality
                item.op = localEnumerationItem::EQUAL;
                state = STATE_VALUE;
                break;

                // Reading a value. For example, the '0' in '0=on,1=off'
            case STATE_VALUE:
                // If nothing left, finish
                if( start+len >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( len == 0 && localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If more numeric characters, continue
                if( localEnumerationIn[start+len] >= '0' && localEnumerationIn[start+len] <= '9' )
                {
                    len++;
                    break;
                }

                // If have a value, save it
                if( len )
                {
                    item.value = localEnumerationIn.mid( start, len ).toInt();
                    start += len;
                    len = 0;
                    state = STATE_DELIMITER;
                    break;
                }

                // Error do no more
                state = STATE_END;
                break;

            // Reading the delimiter between value and text.
            case STATE_DELIMITER:
                // If nothing left, finish
                if( start+len >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( len == 0 && localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If delimiter found, use it
                if( localEnumerationIn[start] == ':' )
                {
                    start++;
                    len = 0;
                    state = STATE_START_QUOTE;
                    break;
                }

                // Error do no more
                state = STATE_END;
                break;

            // Where an enumerations string is quoted, handle the opening quotation mark.
            // For example, the first quote in 0=off,1="pump on"
            case STATE_START_QUOTE:
                // If nothing left, finish
                if( start >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If found '"' use it
                if( localEnumerationIn[start] == '"' )
                {
                    start++;
                    len = 0;
                    state = STATE_QUOTED_TEXT;
                    break;
                }

                // No quote found, assume unquoted text instead
                state = STATE_UNQUOTED_TEXT;
                break;

            // Where an enumerations string is quoted, extract the string within quotation marks.
            // For example, the string 'pump on' in in 0=off,1="pump on"
            case STATE_QUOTED_TEXT:
                // If nothing left, finish
                if( start+len >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If have all text, save it
                if( localEnumerationIn[start+len] == '"' )
                {
                    item.text = localEnumerationIn.mid( start, len );
                    start += len;
                    len = 0;
                    localEnumeration.append( item );
                    item.value = 0;
                    item.op = localEnumerationItem::UNKNOWN;
                    item.text.clear();
                    state = STATE_END_QUOTE;
                    break;
                }

                // Extend the text
                len++;
                break;

            // Where an enumerations string is not quoted, extract the string.
            // For example, the string 'off' in in 0=off,1="pump on"
            case STATE_UNQUOTED_TEXT:
                // If nothing left, finish
                if( start+len >= size )
                {
                    // if reached the end, use what ever we have
                    if( len )
                    {
                        item.text = localEnumerationIn.mid( start, len );
                        localEnumeration.append( item );
                    }
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( len == 0 && localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If have started, finish text when white space, comma
                if( len != 0 && ( localEnumerationIn[start+len] == ' ' ||  localEnumerationIn[start+len] == ',' ) )
                {
                    item.text = localEnumerationIn.mid( start, len );
                    start += len;
                    len = 0;
                    localEnumeration.append( item );
                    item.value = 0;
                    item.text.clear();
                    state = STATE_COMMA;
                    break;
                }

                // Extend the text
                len++;
                break;

            // Where an enumerations string is quoted, handle the closing quotation mark.
            // For example, the second quote in 0=off,1="pump on"
            case STATE_END_QUOTE:
                // If nothing left, finish
                if( start >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If found '"' use it
                if( localEnumerationIn[start] == '"' )
                {
                    start++;
                    len = 0;
                    state = STATE_COMMA;
                    break;
                }

                // Error do no more
                state = STATE_END;
                break;

            // Reading the ',' between each value and string pair.  For example, the ',' in '0:on,1:off'
            case STATE_COMMA:
                // If nothing left, finish
                if( start >= size )
                {
                    state = STATE_END;
                    break;
                }

                // If haven't started yet, skip white space
                if( localEnumerationIn[start] == ' ' )
                {
                    start++;
                    break;
                }

                // If found ',' use it
                if( localEnumerationIn[start] == ',' )
                {
                    start++;
                    len = 0;
                    state = STATE_OPERATOR;
                    break;
                }

                // Error do no more
                state = STATE_END;
                break;

            // finish. Re-initialise for safety
            case STATE_END:
                start = size+1;
                len = 0;
                break;
        }
    }

    // Replace any \n strings with a real new line character
    for( int i = 0; i < localEnumeration.size(); i++ )
    {
        localEnumeration[i].text.replace( "\\n", "\n" );

    }
}


//========================================================================================
// 'Get' formatting configuration methods

/*!
    Get the precision. See setPrecision() for the use of 'precision'.
*/
unsigned int QCaStringFormatting::getPrecision() {
    return precision;
}

/*!
    Get the 'use DB precision' flag. See setUseDbPrecision() for the use of the
    'use DB precision' flag.
*/
bool QCaStringFormatting::getUseDbPrecision() {
    return useDbPrecision;
}

/*!
    Get the 'leading zero' flag. See setLeadingZero() for the use of the
    'leading zero' flag.
*/
bool QCaStringFormatting::getLeadingZero() {
    return leadingZero;
}

/*!
    Get the 'trailing zeros' flag. See setTrailingZeros() for the use of the
    'trailing zeros' flag.
*/
bool QCaStringFormatting::getTrailingZeros() {
    return trailingZeros;
}

/*!
    Get the type of information being formatted. See setFormat() for the use of
    the format type.
*/
QCaStringFormatting::formats QCaStringFormatting::getFormat() {
    return format;
}

/*!
    Get the numerical base. See setRadix() for the use of 'radix'.
*/
unsigned int QCaStringFormatting::getRadix() {
    return stream.integerBase();
}

/*!
    Return the floating point notation
*/
QCaStringFormatting::notations QCaStringFormatting::getNotation() {
    return (QCaStringFormatting::notations)stream.realNumberNotation();
}

/*!
    Get the 'include engineering units' flag.
*/
bool QCaStringFormatting::getAddUnits() {
    return addUnits;
}

/*!
    Get the local enumeration strings. See setLocalEnumeration() for the use of 'localEnumeration'.
*/
QString/*localEnumerationList*/ QCaStringFormatting::getLocalEnumeration() {

    return localEnumerationString;

/* was returning regenerated localEumeration string
    QString s;
    int i;
    for( i = 0; i < localEnumeration.size(); i++ )
    {
        s.append( localEnumeration[i].value ).append( "=" );
        if( localEnumeration[i].text.contains( ' ' ) )
        {
            s.append( "\"" ).append( localEnumeration[i].text ).append( "\"" );
        }
        else
        {
            s.append( localEnumeration[i].text );
        }
        if( i != localEnumeration.size()+1 )
        {
            s.append( "," );
        }
    }
    return s;
*/
}

