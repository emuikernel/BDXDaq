/*! 
  \class QCaString
  \version $Revision: #6 $
  \date $DateTime: 2010/09/06 13:16:04 $
  \author andrew.rhyder
  \brief String wrapper for QCaObject variant data.
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

#ifndef QCASTRING_H
#define QCASTRING_H

#include <QtDebug>
#include <QVariant>
#include <QCaObject.h>
#include <QCaStringFormatting.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaString : public qcaobject::QCaObject {
    Q_OBJECT

  public:
    QCaString( QString recordName, QObject *eventObject, QCaStringFormatting *stringFormattingIn, unsigned int variableIndexIn );
    QCaString( QString recordName, QObject *eventObject, QCaStringFormatting *stringFormattingIn, unsigned int variableIndexIn, UserMessage* userMessageIn );

  signals:
    void stringChanged( const QString& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp, const unsigned int &variableIndex );

  public slots:
    void writeString( const QString &data );

  private:
    void initialise( QCaStringFormatting* newStringFormat, unsigned int variableIndexIn );
    QCaStringFormatting *stringFormat;
    unsigned int variableIndex;

  private slots:
    void convertVariant( const QVariant& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp );
};

#endif /// QCASTRING_H
