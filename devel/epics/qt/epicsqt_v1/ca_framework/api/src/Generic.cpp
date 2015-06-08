/*! 
  \class Generic
  \version $Revision: #4 $
  \date $DateTime: 2010/08/30 16:37:08 $
  \author anthony.owen
  \brief Provides a generic holder for different types.
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

#include <Generic.h>
#include <stdlib.h>
#include <string.h>

using namespace generic;

/*!
    Creates unknown type
*/
Generic::Generic() { 
    value = NULL;
    arrayCount = 0;
    type = UNKNOWN;
}

/*!
    Creates overloaded constructor of type string
*/
Generic::Generic( std::string newValue ) {
    value = NULL;
    arrayCount = 0;
    setString( newValue );
}

/*!
    Creates overloaded constructor of type short
*/
Generic::Generic( short newValue ) {
    value = NULL;
    arrayCount = 0;
    setShort( newValue );
}

/*!
    Creates overloaded constructor of type char
*/
Generic::Generic( char newValue ) {
    value = NULL;
    arrayCount = 0;
    setChar( newValue );
}

/*!
    Creates overloaded constructor of type unsigned long
*/
Generic::Generic( unsigned long newValue ) {
    value = NULL;
    arrayCount = 0;
    setUnsignedLong( newValue );
}

/*!
    Creates overloaded constructor of type double
*/
Generic::Generic( double newValue ) {
    value = NULL;
    arrayCount = 0;
    setDouble( newValue );
}

Generic::Generic( double* newValue, unsigned long arrayCountIn ) {
    value = NULL;
    arrayCount = 0;
    setDouble( newValue, arrayCountIn );
}

/*!
    Actively delete the stored value and set flags
*/
Generic::~Generic() {
    deleteValue();
}

/*!
    Copy constructor for deep copy
*/
Generic::Generic( Generic &param ) {
    cloneValue( &param );
}

/*!
    = Operator for assignment
*/
Generic& Generic::operator= ( Generic &param ) {
    cloneValue( &param );
    return *this;
}

/*!
    Creates and records new type string
*/
void Generic::setString( std::string newValue ) {
    deleteValue();
    value = new std::string( newValue );
    type = STRING;
}

/*!
    Creates and records new type short
*/
void Generic::setShort( short newValue ) { 
    deleteValue();
    value = new short( newValue );
    type = SHORT;
}

/*!
    Creates and records new type char
*/
void Generic::setChar( char newValue ) { 
    deleteValue();
    value = new char( newValue );
    type = CHAR;
}

/*!
    Creates and records new type unsigned long
*/
void Generic::setUnsignedLong( unsigned long newValue ) { 
    deleteValue();
    value = new unsigned long( newValue );
    type = UNSIGNED_LONG;
}

/*!
    Creates and records new type double
*/
void Generic::setDouble( double newValue ) { 
    setDouble( &newValue, 1 );
//    deleteValue();
//    value = new double( newValue );
//    arrayCount = 1;
//    type = DOUBLE;
}

/*!
    Creates and records new type double (an array larger than 1)
*/

void Generic::setDouble( double* newValueArray, unsigned long arrayCountIn ) {
    deleteValue();


//    value = new double( *newValueArray );
//    arrayCount = 1;
//    type = DOUBLE;
//    return;

    value = new double[arrayCountIn];
    memcpy( value, newValueArray, sizeof(double)*arrayCountIn );
    arrayCount = arrayCountIn;
    type = DOUBLE;
}

/*!
    Returns type string or invalid
*/
std::string Generic::getString() {
    if( getType() == STRING ) {
        return *(std::string*)value;
    }
    return "";
}

/*!
    Returns type string or short
*/
short Generic::getShort() { 
    if( getType() == SHORT ) {
        return *(short*)value;
    }
    return 0;
}

/*!
    Returns type char or invalid
*/
char Generic::getChar() { 
    if( getType() == CHAR ) {
        return *(char*)value;
    }
    return 0;
}

/*!
    Returns type unsigned long or invalid
*/
unsigned long Generic::getUnsignedLong() { 
    if( getType() == UNSIGNED_LONG ) {
        return *(unsigned long*)value;
    }
    return 0;
}

/*!
    Returns type double or invalid
*/
double Generic::getDouble() { 
    if( getType() == DOUBLE ) {
        return *(double*)value;
    }
    return 0;
}

/*!
    Returns type double array or invalid
*/
void Generic::getDouble( double** valueArray, unsigned long* arrayCountOut ) {
    if( getType() == DOUBLE ) {
        *valueArray = (double*)value;
        if( arrayCountOut )
            *arrayCountOut = arrayCount;
        return;
    }
    *valueArray = NULL;
    if( arrayCountOut )
        *arrayCountOut = 0;
}

/*!
    Returns value type
*/
generic_types Generic::getType() {
    return type;
}

/*!
    Returns array size
*/
unsigned long Generic::getArrayCount() {
    return arrayCount;
}



/*!
    Sets the value type
*/
void Generic::setType( generic_types newType ) {
    type = newType;
}

/*!
    Deletes stored value 
*/
void Generic::deleteValue() {
    if( value == NULL ) {
        type = UNKNOWN;
        return;
    }
    switch( getType() ) {
        case STRING :
            delete (std::string*)value;
        break;
        case SHORT :
            delete (short*)value;
        break;
        case CHAR :
            delete (char*)value;
        break;
        case UNSIGNED_LONG :
            delete (unsigned long*)value;
        break;
        case DOUBLE :
            delete (double*)value;
//          free( value );
        break;
        case UNKNOWN :
            value = NULL;
            return;
        break;
    }
    value = NULL;
    type = UNKNOWN;
}

/*!
    Clone from given Generic
*/
void Generic::cloneValue( Generic *param ) {
    switch( param->getType() ) {
        case STRING :
            setString( param->getString() );
        break;
        case SHORT :
            setShort( param->getShort() );
        break;
        case CHAR :
            setChar( param->getChar() );
        break;
        case UNSIGNED_LONG :
            setUnsignedLong( param->getUnsignedLong() );
        break;
        case DOUBLE :
//            setDouble( param->getDouble() );
            {
                double* paramValue;
                unsigned long paramCount;
                param->getDouble( &paramValue, &paramCount );
                setDouble( paramValue, paramCount );
            }
        break;
        case UNKNOWN :
            deleteValue();
        break;
    }
}
