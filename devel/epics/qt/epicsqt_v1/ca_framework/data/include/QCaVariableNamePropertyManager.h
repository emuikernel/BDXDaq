/*! 
  \class QCaVariableNamePropertyManager
  \version $Revision: #3 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief Handles variable name sets.
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

#ifndef QCAVARIABLENAMEPROPERTYMANAGER_H
#define QCAVARIABLENAMEPROPERTYMANAGER_H

#include <QTimer>
#include <QString>

class QCaVariableNamePropertyManager : public QTimer {
    Q_OBJECT

  public:
    QCaVariableNamePropertyManager();

    QString getVariableNameProperty();
    void setVariableNameProperty( QString variableNamePropertyIn );

    QString getSubstitutionsProperty();
    void setSubstitutionsProperty( QString substitutionsPropertyIn );

    void setVariableIndex( unsigned int variableIndexIn );

signals:
    void newVariableNameProperty( QString variable, QString Substitutions, unsigned int variableIndex );

private slots:
    void subscribeDelayExpired();       // Called a short time after a user stops typing in 'designer'


private:
    QString variableNameProperty;
    QString substitutionsProperty;

    unsigned int variableIndex;     // Index into the list of variable names maintained by a QCa widget
    bool interactive;   // If set, there is a user typing new variable names and macro substitutions. Use timers to wait for typing to finish.
};

#endif /// QCAVARIABLENAMEPROPERTYMANAGER_H
