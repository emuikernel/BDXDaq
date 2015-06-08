/*! 
  \class CaObject
  \version $Revision: #5 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author anthony.owen
  \brief Provides CA to an EPICS channel.
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

#ifndef CAOBJECT_H_
#define CAOBJECT_H_

#include <string>
#include <Generic.h>
using namespace generic;

namespace caobject {

  enum callback_reasons { SUBSCRIPTION_SUCCESS, SUBSCRIPTION_FAIL,
                          READ_SUCCESS, READ_FAIL,
                          WRITE_SUCCESS, WRITE_FAIL,
                          EXCEPTION,
                          CONNECTION_UP, CONNECTION_DOWN, CONNECTION_UNKNOWN };

    class CaObject {

            // Initialisation and deletion
        public:
            CaObject();
            virtual ~CaObject();

            void setWriteWithCallback( bool writeWithCallback);
            bool getWriteWithCallback();

        private:
            void initialise();
            void shutdown();

        protected:
            // Data record interrogation
            bool          isFirstUpdate();
            void*         getRecordCopyPtr(); // Return a copy of the record as a void*. Caller is responsible for deleting.
            int           getEnumStateCount();
            std::string   getEnumState( int position );
            int           getPrecision();
            double        getDisplayUpper();
            double        getDisplayLower();
            double        getAlarmUpper();
            double        getAlarmLower();
            double        getWarningUpper();
            double        getWarningLower();
            double        getControlUpper();
            double        getControlLower();
            std::string   getUnits();
            generic_types getType();
            unsigned long getTimeStampSeconds();
            unsigned long getTimeStampNanoseconds();
            short         getAlarmStatus();
            short         getAlarmSeverity();

        public:
            virtual void  signalCallback( callback_reasons ) = 0;

            // CA specific data private to the CaObject code
            // To allow application to include the QCaFramework without requiring
            // EPICS include files, this is obscured by casting the CA specific data as a void*
        public:
            void*        priPtr;     // Actually of type CaObjectPrivate*
        protected:
            static int CA_UNIQUE_OBJECT_ID;            //! Unique ID assigned to each instance.

    };

}

#endif  // CAOBJECT_H_
