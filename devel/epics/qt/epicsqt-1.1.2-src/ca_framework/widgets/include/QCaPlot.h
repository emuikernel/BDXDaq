/*! 
  \class QCaPlot
  \version $Revision: #4 $
  \date $DateTime: 2010/09/06 13:16:04 $
  \author glenn.jackson
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
 *    Glenn Jackson
 *  Contact details:
 *    glenn.jackson@synchrotron.org.au
 */

#ifndef QCAPLOT_H
#define QCAPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <QCaWidget.h>
#include <QCaFloating.h>
#include <QCaFloatingFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QVector>
#include <QTimer>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaPlot : public QwtPlot, public QCaWidget {
    Q_OBJECT

  public:
    QCaPlot( QWidget *parent = 0 );
    QCaPlot( const QString &variableName, QWidget *parent = 0 );

    ~QCaPlot();

    bool isEnabled() const;
    void setEnabled( bool state );

    // Property convenience functions

    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    void setSubscribe( bool subscribe );
    bool getSubscribe();

    void setVariableAsToolTip( bool variableAsToolTip );
    bool getVariableAsToolTip();

    void setYMin( double yMin );
    double getYMin();

    void setYMax( double yMax );
    double getYMax();

    void setAutoScale( bool autoScale );
    bool getAutoScale();

    // No QCaPlot::setTitle() needed. Uses QwtPlot::setTitle()
    QString getTitle();

    void    setBackgroundColor( QColor backgroundColor );
    QColor getBackgroundColor();

    void    setTraceColor( QColor traceColor );
    QColor getTraceColor();

    void    setTraceLegend( QString traceLegend );
    QString getTraceLegend();

    void    setXUnit( QString xUnit );
    QString getXUnit();

    void    setYUnit( QString yUnit );
    QString getYUnit();

    void setXStart( double xStart );
    double getXStart();

    void setXIncrement( double xIncrement );
    double getXIncrement();

    void setTimeSpan( unsigned int timeSpan );
    unsigned int getTimeSpan();

    void setTickRate( unsigned int tickRate );
    unsigned int getTickRate();

  protected:
    QCaFloatingFormatting floatingFormatting;
    bool localEnabled;

    void establishConnection( unsigned int variableIndex );

    bool visible;               // Flag true if the widget should be visible outside 'Designer'
    void setRunVisible( bool visibleIn );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setPlotData( const QVector<double>& values, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void setPlotData( const double value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void tickTimeout();

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const double& out );
    void dbValueChanged( const QVector<double>& out );

  private:

    void setup();

    QTimer* tickTimer;          // Timer to keep strip chart scrolling
    void setPlotDataCommon();
    void setalarmInfoCommon( QCaAlarmInfo& alarmInfo );


    // General plot properties
    double yMin;
    double yMax;
    bool autoScale;

    // Trace properties
    QColor traceColor;
    QString traceLegend;
    unsigned int tickRate; //mS
    unsigned int timeSpan; // Seconds

    // Waveform properties
    double xStart;
    double xIncrement;

    // Functions common to most QCa widgets
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip( const QString& tip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
    
    // Variables and functions to manage plot data
    QDateTime baseTime;

    QVector<QCaDateTime> timeStamps;
    QVector<double> xdata;
    QVector<double> ydata;
    QwtPlotCurve* curve;

    void regenerateTickXData();

    void setCurveColor( const QColor color );
};


#endif /// QCAPLOT_H
