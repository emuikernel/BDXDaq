/*! 
  \class QCaWidget
  \version $Revision: #10 $
  \date $DateTime: 2010/09/06 13:16:04 $
  \author anthony.owen
  \brief Template for Qt-CA aware widgets.
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

#ifndef QCAWIDGET_H
#define QCAWIDGET_H

#include <QObject>
#include <QCaObject.h>
#include <VariableNameManager.h>
#include <UserMessage.h>
#include <ContainerProfile.h>
#include <QCaToolTip.h>
#include <QCaPluginLibrary_global.h>

/// #include <QWidget> /// my_

class QCAPLUGINLIBRARYSHARED_EXPORT QCaWidget : public VariableNameManager, public QCaToolTip, public ContainerProfile {///, public QWidget {

public:
    QCaWidget();                            /// Constructor
    ~QCaWidget();                           /// Destructor
    UserMessage userMessage;                /// Manager of messages to the user
    void activate();                        /// Initiate updates.
///--------------------- my_ ----------------------------------
/*
enum colMode {Default=0, Static, Alarm_Default, Alarm_Static};


    QColor thisForeColor, oldForeColor;
    QColor thisBackColor, oldBackColor;
    QPalette thisPalette;
    colMode thisColorMode;
    colMode oldColorMode;

 //   int thisPrecision;
 //   SourceMode thisPrecMode;
 //   SourceMode thisLimitsMode;

 //   int thisUnitMode;
//    QString keepText;
//    char thisFormat[20];
//    char thisFormatC[20];
//    bool d_rescaleFontOnTextChanged;
//    double thisMaximum, thisMinimum;
//    FormatType thisFormatType;
    QString thisStyle, oldStyle;

    QColor getForeground() const {return thisForeColor;}
    void setForeground(QColor c);
    QColor getBackground() const {return thisBackColor;}
    void setBackground(QColor c);

    void setColors(QColor bg, QColor fg);
*/
///-------------------------------------------------------------

protected:
    void setNumVariables( unsigned int numVariablesIn );    /// Set the number of variables that will stream data updates to the widget. Default of 1 if not called.

    bool subscribe;                 /// Flag if data updates should be requested
    bool variableAsToolTip;         /// Flag the tool tip should be set to the variable name

    qcaobject::QCaObject* createConnection( unsigned int variableIndex );    /// Create a CA connection. Return a QCaObject if successfull

    virtual void setup() = 0;                               ///???
    virtual qcaobject::QCaObject* createQcaItem( unsigned int variableIndex ) = 0; /// Function to create a appropriate superclass of QCaObject to stream data updates

    qcaobject::QCaObject* getQcaItem( unsigned int variableIndex );         /// Return a reference to one of the qCaObjects used to stream CA updates

  private:
    void deleteQcaItem( unsigned int variableIndex );       /// Delete a stream of CA updates
    unsigned int numVariables;              /// The number of process variables that will be managed for the QCa widget.
    qcaobject::QCaObject** qcaItem;          /// CA access - provides a stream of updates. One for each variable name used by the QCa widget

};

#endif // QCAWIDGET_H
