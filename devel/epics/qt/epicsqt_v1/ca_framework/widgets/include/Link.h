/*! 
  \class Link
  \version $Revision: #8 $
  \date $DateTime: 2010/09/06 13:16:04 $
  \author andrew.rhyder
  \brief Manage updating tool tip with variable name, alarm state and connected state
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

#ifndef QLINK_H
#define QLINK_H

#include <QLabel>
#include <QVariant>
#include <QString>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT Link : public QLabel {
   Q_OBJECT

  public:
    Link( QWidget *parent = 0 );

    enum conditions { CONDITION_EQ, CONDITION_NE, CONDITION_GT, CONDITION_GE, CONDITION_LT, CONDITION_LE };

    // Property convenience functions

    // condition
    void setCondition( conditions conditionIn );
    conditions getCondition();

    // comparisonValue Value to compare input signals to
    void    setComparisonValue( QString comparisonValue );
    QString getComparisonValue();

    // signalTrue (Signal if condition is met)
    void setSignalTrue( bool signalTrue );
    bool getSignalTrue();

    // signalFalse (Signal if condition not met)
    void setSignalFalse( bool signalFalse );
    bool getSignalFalse();

    // outTrueValue Value to emit if condition is met
    void    setOutTrueValue( QString outTrueValue );
    QString getOutTrueValue();

    // outFalseValue Value to emit if condition is not met
    void    setOutFalseValue( QString outFalseValue );
    QString getOutFalseValue();

    // visible (widget is visible outside 'Designer')
    void setRunVisible( bool visibleIn );
    bool getRunVisible();


  protected:
    conditions condition;       // Rule for comparing incoming signal
    QVariant comparisonValue;    // Value to compare incoming signal against

    bool signalTrue;            // Flag true if a signal should be emmited if the condition matches
    bool signalFalse;           // Flag true if a signal should be emmited if the condition does not match
    QVariant outTrueValue;      // Value to send in signal if condition matches
    QVariant outFalseValue;     // Value to send in signal if condition does not match

    bool visible;               // Flag true if the widget should be visible outside 'Designer'

  public slots:
    void in( const bool& in );
    void in( const qlonglong& in );
    void in( const double& in );
    void in( const QString& in );

    void autoFillBackground( const bool& enable );

  signals:
    void out( const bool& out );
    void out( const qlonglong& out );
    void out( const double& out );
    void out( const QString& out );


  private:
    void sendValue( bool match );
    void emitValue( QVariant value );

};

#endif /// QLINK_H
