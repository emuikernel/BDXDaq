/*! 
  \class QCaComboBox
  \version $Revision: #14 $
  \date $DateTime: 2010/09/06 13:16:04 $
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
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QCAComboBOX_H
#define QCAComboBOX_H

#include <QComboBox>
#include <QCaWidget.h>
#include <QCaInteger.h>
#include <QCaIntegerFormatting.h>
#include <QCaConnectionInfo.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaComboBox : public QComboBox, public QCaWidget {
    Q_OBJECT

  public:
    QCaComboBox( QWidget *parent = 0 );
    QCaComboBox( const QString& variableName, QWidget *parent = 0 );

    bool isEnabled() const;
    void setEnabled( bool state );

    // Property convenience functions

    // Variable Name and variable substitutions
    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    // subscribe
    void setSubscribe( bool subscribe );
    bool getSubscribe();

    // variable as tool tip
    void setVariableAsToolTip( bool variableAsToolTip );
    bool getVariableAsToolTip();

    // use database enumerations
    void setUseDbEnumerations( bool useDbEnumerations );
    bool getUseDbEnumerations();


  protected:
    QCaIntegerFormatting integerFormatting;
    bool useDbEnumerations;
    bool localEnabled;

    void establishConnection( unsigned int variableIndex );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setValueIfNoFocus( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& );
    void userValueChanged( int value );

  public slots:
    void requestEnabled( const bool& state );

signals:
    void dbValueChanged( const qlonglong& out );

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
};

#endif ///QCAComboBOX_H
