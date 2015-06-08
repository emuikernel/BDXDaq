/*! 
  \class QCaShape
  \version $Revision: #12 $
  \date $DateTime: 2010/09/06 13:16:04 $
  \author andrew.rhyder
  \brief CA Shape Widget.
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

#ifndef QCASHAPE_H
#define QCASHAPE_H

#include <QCaInteger.h>
#include <QCaWidget.h>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QCaPluginLibrary_global.h>

//! Maximum number of variables.
#define NUM_VARIABLES 6

class QCAPLUGINLIBRARYSHARED_EXPORT QCaShape : public QWidget, public QCaWidget {
    Q_OBJECT

  public:
    QCaShape( QWidget *parent = 0 );
    QCaShape( const QString& variableName, QWidget *parent = 0 );

    enum Shape { Line, Points, Polyline, Polygon, Rect, RoundedRect, Ellipse, Arc, Chord, Pie, Path, Text, Pixmap };
    enum Animations {Width, Height, X, Y, Transperency, Rotation, ColourHue, ColourSaturation, ColourValue, ColourIndex, Penwidth};


    bool isEnabled() const;
    void setEnabled( bool state );

    // Property convenience functions

    // Variable name and substitutions
    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    // variable animations
    void setAnimation( Animations animation, const int index );
    Animations getAnimation( const int index );

    // scales
    void setScale( const double scale, const int index );
    double getScale( const int index );

    // offsets
    void setOffset( const double offset, const int index );
    double getOffset( const int index );

    // border
    void setBorder( const bool border );
    bool getBorder();

    // fill
    void setFill( const bool fill );
    bool getFill();

    // subscribe
    void setSubscribe( const bool subscribe );
    bool getSubscribe();

    // variable as tool tip
    void setVariableAsToolTip( const bool variableAsToolTip );
    bool getVariableAsToolTip();

    // shape
    void setShape( Shape shape );
    Shape getShape();


    // number of points
    void setNumPoints( const unsigned int numPoints );
    unsigned int getNumPoints();

    // Origin translation
    void setOriginTranslation( const QPoint originTranslation );
    QPoint getOriginTranslation();

    // points
    void setPoint( const QPoint point, const int index );
    QPoint getPoint(const int index);

    // colors
    void setColor( const QColor color, const int index );
    QColor getColor( const int index );

    // draw border
    void setDrawBorder( const bool drawBorder );
    bool getDrawBorder();

    // line width
    void setLineWidth( const unsigned int lineWidth );
    unsigned int getLineWidth();

    // start angle
    void setStartAngle( const double startAngle );
    double getStartAngle();

    // rotation
    void setRotation( const double rotation );
    double getRotation();

    // arc length
    void setArcLength( const double arcLength );
    double getArcLength();

    // text
    void setText( const QString text );
    QString getText();



  protected:
    QCaIntegerFormatting integerFormatting;                     /// Integer formatting options


  #define OFFSETS_SIZE NUM_VARIABLES
    double offsets[OFFSETS_SIZE];

  #define SCALES_SIZE NUM_VARIABLES
    double scales[SCALES_SIZE];

    Shape shape;
    QPoint originTranslation;

  #define POINTS_SIZE 10
    QPoint points[POINTS_SIZE];
    unsigned int numPoints;

  #define COLORS_SIZE 10
    QColor colors[COLORS_SIZE];

    Animations animations[6];

    bool fill;
    bool border;

    double startAngle;
    double arcLength;
    QString text;
    double rotation;
    unsigned int lineWidth;
    bool drawBorder;

    bool localEnabled;

    void colorChange( unsigned int index );                     /// Act on a color property change. (will update shape if using the color)
    unsigned int currentColor;                                  /// Index into colorsProperty last used when setting brush color

    void establishConnection( unsigned int variableIndex );     /// Create a CA connection and initiates updates if required

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setValue( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex );

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged1( const qlonglong& out );
    void dbValueChanged2( const qlonglong& out );
    void dbValueChanged3( const qlonglong& out );
    void dbValueChanged4( const qlonglong& out );
    void dbValueChanged5( const qlonglong& out );
    void dbValueChanged6( const qlonglong& out );

  private:
    void setup();                                               /// Initialisation common to all constructors
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    void paintEvent(QPaintEvent *event);
    QPen pen;
    QBrush brush;
    bool antialiased;
    QPixmap pixmap;

    double painterCurrentScaleX;
    double painterCurrentScaleY;
    double painterCurrentTranslateX;
    double painterCurrentTranslateY;

    QPoint scaledOriginTranslation;
};

#endif /// QCASHAPE_H
