/*! 
  \class QCaPlot
  \version $Revision: #3 $
  \date $DateTime: 2010/09/06 11:58:56 $
  \author glenn.jackson
  \brief CA Plot Widget.
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

/*!
  This class is a CA aware Plot widget based on the Qwt QwtPlot widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
 */

#include <qwt_legend.h>
#include <QCaPlot.h>

/*!
    Constructor with no initialisation
*/
QCaPlot::QCaPlot( QWidget *parent ) : QwtPlot( parent ), QCaWidget() {
    setup();
}

/*!
    Constructor with known variable
*/
QCaPlot::QCaPlot( const QString &variableNameIn, QWidget *parent ) : QwtPlot( parent ), QCaWidget() {
    setup();
    setVariableName( variableNameIn, 0 );
}

/*!
    Setup common to all constructors
*/
void QCaPlot::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(1);

    // Set up default properties
    visible = true;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    // General plot properties
    yMin = 0.0;
    yMax = 0.0;
    autoScale = true;
    //setLabelOrientation (Qt::Orientation)Qt::Vertical

    // Trace properties
    traceColor = Qt::black;

    // Thread to use when running as a strip chart
    tickTimer = NULL;

    // Waveform properties
    xStart = 0.0;
    xIncrement = 1.0;

    // Initially no curve
    curve = NULL;
//    current_x = 0.0;  // temporary to plot an x axis.

    tickRate = 50;
    timeSpan = 59;

    // Use QwtPlot signals
    // --Currently none--
}

QCaPlot::~QCaPlot()
{
    if( tickTimer )
    {
        tickTimer->stop();
        delete tickTimer;
    }

    if( curve )
    {
        delete curve;
        curve = NULL;
    }

}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a strip chart a QCaObject that streams floating point data is required.
*/
qcaobject::QCaObject* QCaPlot::createQcaItem( unsigned int variableIndex ) {

   // Create the item as a QCaFloating
   return new QCaFloating( getSubstitutedVariableName( variableIndex ), this, &floatingFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaPlot::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        QObject::connect( qca,  SIGNAL( floatingArrayChanged( const QVector<double>&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setPlotData( const QVector<double>&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( floatingChanged( const double, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                          this, SLOT( setPlotData( const double, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}


/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaPlot::updateToolTip( const QString& tip )
{
    setToolTip( tip );
}

/*!
    Act on a connection change.
    Change how the strip chart looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaPlot::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    /// If connected, enable the widget if the QCa enabled property is true
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );
    }

    /// If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );
    }
}

/*!
    Update the plotted data with a new single value
    This is a slot used to recieve data updates from a QCaObject based class.
 */

void QCaPlot::setPlotData( const double value, QCaAlarmInfo& alarmInfo, QCaDateTime& timestamp, const unsigned int& ) {

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( value );

    // Setup to do if this is the first data
    if( xdata.count() == 0 )
    {

        baseTime = (QDateTime)timestamp;

        tickTimer = new QTimer(this);
        connect(tickTimer, SIGNAL(timeout()), this, SLOT(tickTimeout()));
        tickTimer->start( tickRate );
    }
    // !!!??? This above timer is never stopped
    // !!!??? Stop it if this switches over to an array input

    // Add the new data point
    timeStamps.append( timestamp );
    ydata.append(value);
    xdata.append( 0.0 ); // keep x and y arrays the same size
    regenerateTickXData();

    // Remove any old data
    QDateTime oldest = QDateTime::currentDateTime();
    oldest = oldest.addSecs( -timeSpan );
    while( timeStamps.count() > 1 )
    {
        if( timeStamps[1] < oldest )
        {
            timeStamps.remove(0);
            xdata.remove(0);
            ydata.remove(0);
        }
        else
        {
            break;
        }
    }

    // Fix the X for a strip chart
    setAxisScale( xBottom, -(double)timeSpan, 0.0 );

    // The data is now ready to plot
    setPlotDataCommon();
    setalarmInfoCommon( alarmInfo );
}

/*!
    Update the plotted data with a new array of values
    This is a slot used to recieve data updates from a QCaObject based class.
 */
void QCaPlot::setPlotData( const QVector<double>& values, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& ) {

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( values );

    // Clear any previous data
    xdata.clear();
    ydata.clear();
    timeStamps.clear();

    // If no increment was supplied, use 1 by default
    double inc;
    xIncrement == 0.0 ? inc = 1.0 : inc = xIncrement;

    for( int i = 0; i < values.count(); i++ )
    {
        xdata.append( xStart + ((double)i * inc ) );
        ydata.append( values[i] );
    }

    // Autoscale X for a waveform
    setAxisAutoScale( xBottom );

    // The data is now ready to plot
    setPlotDataCommon();
    setalarmInfoCommon( alarmInfo );

}

// Update the plot with new data.
// The new data may be due to a new value being added to the current values (stripchart)
// or the new data may be due to a new waveform
void QCaPlot::setPlotDataCommon()
{
    // Create the curve if it does not exist
    if( !curve )
    {
        curve = new QwtPlotCurve( traceLegend );
        setCurveColor( traceColor );
        curve->setRenderHint( QwtPlotItem::RenderAntialiased );
        curve->attach(this);
    }

    // Set the curve data
    curve->setData(xdata, ydata);

    // Update the plot
    replot();
}

void QCaPlot::setalarmInfoCommon( QCaAlarmInfo& alarmInfo )
{
/// If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
            updateToolTipAlarm( alarmInfo.severityName() );
            setStyleSheet( alarmInfo.style() );
            lastSeverity = alarmInfo.getSeverity();
    }
}

/*!
  For strip chart functionality
  Recalculate the x value as time goes by
 */
void QCaPlot::regenerateTickXData()
{
    QDateTime now = QDateTime::currentDateTime();
    for( int i = 0; i < xdata.count(); i++)
    {
        xdata[i] = timeStamps[i].floating( now );
    }
}

// Update the chart if it is a strip chart
void QCaPlot::tickTimeout()
{
    regenerateTickXData();
    setPlotDataCommon();
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaPlot::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return localEnabled;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaPlot::setEnabled( bool state )
{
    /// Note the new 'enabled' state
    localEnabled = state;

    /// Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( localEnabled );
}

/*!
   Slot similar to default widget setEnabled, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaPlot::requestEnabled( const bool& state )
{
    setEnabled(state);
}


/*!
  Manage property to set widget visible or not
 */
void QCaPlot::setRunVisible( bool visibleIn )
{
    // Update the property
    visible = visibleIn;

    // If a container profile has been defined, then this widget is being used in a real GUI and
    // should be visible or not according to the visible property. (While in Designer it can always be displayed)
    ContainerProfile profile;
    if( profile.isProfileDefined() )
    {
        setVisible( visible );
    }

}

// Update the color of the trace
void QCaPlot::setCurveColor( const QColor color )
{
    if( curve )
    {
        curve->setPen (color);
    }
}

//==============================================================================
// Property convenience functions


// Access functions for variableName and variableNameSubstitutions
// variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
void QCaPlot::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
{
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}

// Access functions for subscribe
void QCaPlot::setSubscribe( bool subscribeIn )
{
    subscribe = subscribeIn;
}
bool QCaPlot::getSubscribe(){
    return subscribe;
}

// Access functions for variableAsToolTip
void QCaPlot::setVariableAsToolTip( bool variableAsToolTipIn )
{
    variableAsToolTip = variableAsToolTipIn;
}
bool QCaPlot::getVariableAsToolTip()
{
    return variableAsToolTip;
}

// Access functions for YMin
void QCaPlot::setYMin( double yMinIn )
{
    yMin = yMinIn;
    if( !autoScale )
    {
        setAxisScale( yLeft, yMin, yMax );
    }
}
double QCaPlot::getYMin()
{
    return yMin;
}

// Access functions for yMax
void QCaPlot::setYMax( double yMaxIn )
{
    yMax = yMaxIn;
    if( !autoScale )
    {
        setAxisScale( yLeft, yMin, yMax );
    }
}
double QCaPlot::getYMax()
{
    return yMax;
}

// Access functions for qutoScale
void QCaPlot::setAutoScale( bool autoScaleIn )
{
    autoScale = autoScaleIn;
    
    // Set auto scale if requested, or if manual scale values are invalid
    if( autoScale || yMin == yMax )
    {
        setAxisAutoScale( yLeft );
    }
    else
    {
        setAxisScale( yLeft, yMin, yMax );
    }
}
bool QCaPlot::getAutoScale()
{
    return autoScale;
}

// Access functions for title
// No QCaPlot::setTitle() needed. Uses QwtPlot::setTitle()
QString QCaPlot::getTitle()
{
    return title().text();
}

// Access functions for backgroundColor
void    QCaPlot::setBackgroundColor( QColor backgroundColor )
{
    setCanvasBackground( backgroundColor );
}
QColor QCaPlot::getBackgroundColor()
{
    return canvasBackground();
}

// Access functions for traceColor
void    QCaPlot::setTraceColor( QColor traceColorIn )
{
    traceColor = traceColorIn;
    setCurveColor( traceColor );
}

QColor QCaPlot::getTraceColor()
{
    return traceColor;
}

// Access functions for traceLegend
void QCaPlot::setTraceLegend( QString traceLegendIn ){

    traceLegend = traceLegendIn;
    if( traceLegend.count() )
    {
        insertLegend( new QwtLegend(), QwtPlot::RightLegend );
    }
    else
    {
        insertLegend( NULL, QwtPlot::RightLegend );
    }

    if( curve )
    {
        curve->setTitle( traceLegend );
    }
}
QString QCaPlot::getTraceLegend()
{
    return traceLegend;
}

// Access functions for xUnit
void    QCaPlot::setXUnit( QString xUnit )
{
    setAxisTitle(xBottom, xUnit);
}
QString QCaPlot::getXUnit()
{
    return axisTitle( xBottom ).text();
}

// Access functions for yUnit
void    QCaPlot::setYUnit( QString yUnit )
{
    setAxisTitle( yLeft, yUnit );
}
QString QCaPlot::getYUnit()
{
    return axisTitle( yLeft ).text();
}

// Access functions for xStart
void QCaPlot::setXStart( double xStartIn )
{
    xStart = xStartIn;
}
double QCaPlot::getXStart()
{
    return xStart;
}

// Access functions for xIncrement
void QCaPlot::setXIncrement( double xIncrementIn )
{
    xIncrement = xIncrementIn;
}
double QCaPlot::getXIncrement()
{
    return xIncrement;
}

// Access functions for timeSpan
void QCaPlot::setTimeSpan( unsigned int timeSpanIn )
{
    timeSpan = timeSpanIn;
}
unsigned int QCaPlot::getTimeSpan()
{
    return timeSpan;
}

// Access functions for tickRate
void QCaPlot::setTickRate( unsigned int tickRateIn )
{
    tickRate = tickRateIn;
    if( tickTimer )
    {
        tickTimer->stop();
        tickTimer->start( tickRate );
    }
}
unsigned int QCaPlot::getTickRate()
{
    return tickRate;
}

