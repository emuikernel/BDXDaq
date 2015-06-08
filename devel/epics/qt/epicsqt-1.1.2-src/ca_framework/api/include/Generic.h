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

#ifndef GENERIC_H_
#define GENERIC_H_

#include <string>


namespace generic {

  enum generic_types { STRING, SHORT, CHAR, UNSIGNED_LONG, DOUBLE, UNKNOWN };

  class Generic {
    public:
      Generic();
      Generic( std::string newValue );
      Generic( short newValue );
      Generic( char newValue );
      Generic( unsigned long newValue );
      Generic( double newValue );
      Generic( double* newValueArray, unsigned long countIn );
      ~Generic();

      Generic( Generic &param );
      Generic& operator= ( Generic &param );

      void setString( std::string newValue );
      void setShort( short newValue );
      void setChar( char newValue );
      void setUnsignedLong( unsigned long newValue );
      void setDouble( double newValue );
      void setDouble( double* newValueArray, unsigned long countIn );

      std::string getString();
      short getShort();
      char getChar();
      unsigned long getUnsignedLong();
      double getDouble();
      void getDouble( double** valueArray, unsigned long* countOut = NULL );
      unsigned long getArrayCount();

      generic_types getType();

    protected:
      void cloneValue( Generic *param );

    private:
      unsigned long arrayCount;
      generic_types type;
      void* value;

      void setType( generic_types newType );
      void deleteValue();
  };

}

#endif  // GENERIC_H_
