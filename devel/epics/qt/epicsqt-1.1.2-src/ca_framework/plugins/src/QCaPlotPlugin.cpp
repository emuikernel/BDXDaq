/*! 
  \class QCaPlotPlugin
  \version $Revision: #2 $
  \date $DateTime: 2010/09/06 11:58:56 $
  \author glenn.jackson
  \brief CA Plot Widget Plugin for designer.
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
 *    Glenn Jackson
 *  Contact details:
 *    glenn.jackson@synchrotron.org.au
 */

#include <QtCore>
#include <QTimer>
#include <QCaPlotPlugin.h>

/*!
    ???
*/
QCaPlotPlugin::QCaPlotPlugin( QWidget* parent ) : QCaPlot( parent ) {





/*
    int id = qRegisterMetaType<localEnumerationList>();
    qDebug() << "qRegisterMetaType ID: " << id;
    qRegisterMetaTypeStreamOperators<localEnumerationList>("localEnumerationList");

//    QVariant v;
*/





/*xxx required for a label - remove permenantly if no equivelent function required for a strip chart
// Set some default text to give the label visibility and size as the default label has no border and the background colour is the same as the form

*/
    setMinimumSize(400,200);

    /// Set up a connection to recieve variable name property changes
    /// The variable name property manager class only delivers an updated variable name after the user has stopped typing
    QObject::connect( &variableNamePropertyManager, SIGNAL( newVariableNameProperty( QString, QString, unsigned int ) ), this, SLOT( useNewVariableNameProperty( QString, QString, unsigned int) ) );
}

/*!
    Slot to recieve variable name and macro substitutions property changes.
*/
void QCaPlotPlugin::useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
{
    setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
}
