/*! 
  \class QCaPlotPlugin
  \version $Revision: #3 $
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

#ifndef QCAPLOTPLUGIN_H
#define QCAPLOTPLUGIN_H

#include <QTimer>
#include <QString>
#include <QCaPlot.h>
#include <QCaStringFormatting.h>

class QCaPlotPlugin : public QCaPlot {
    Q_OBJECT

  public:
    /// Constructors
    QCaPlotPlugin( QWidget *parent = 0 );
    QCaPlotPlugin( QString variableName, QWidget *parent = 0 );

    // Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    // A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable READ getVariableNameProperty WRITE setVariableNameProperty);
    void setVariableNameProperty( QString variableName ){ variableNamePropertyManager.setVariableNameProperty( variableName ); }
    QString getVariableNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }

    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

    Q_PROPERTY(bool subscribe READ getSubscribe WRITE setSubscribe)
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(double yMin READ getYMin WRITE setYMin)
    Q_PROPERTY(double yMax READ getYMax WRITE setYMax)
    Q_PROPERTY(bool autoScale READ getAutoScale WRITE setAutoScale)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle)
    Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QColor traceColor READ getTraceColor WRITE setTraceColor)
    Q_PROPERTY(QString traceLegend READ getTraceLegend WRITE setTraceLegend)
    Q_PROPERTY(QString xUnit READ getXUnit WRITE setXUnit)
    Q_PROPERTY(QString yUnit READ getYUnit WRITE setYUnit)
    Q_PROPERTY(double xStart READ getXStart WRITE setXStart)
    Q_PROPERTY(double xIncrement READ getXIncrement WRITE setXIncrement)
    Q_PROPERTY(unsigned int timeSpan READ getTimeSpan WRITE setTimeSpan)
    Q_PROPERTY(unsigned int tickRate READ getTickRate WRITE setTickRate)

  private:
    QCaVariableNamePropertyManager variableNamePropertyManager;

private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

};

#endif /// QCAPLOTPLUGIN_H
