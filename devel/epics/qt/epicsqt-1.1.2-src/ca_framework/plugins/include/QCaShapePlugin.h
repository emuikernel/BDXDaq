/*! 
  \class QCaShapePlugin
  \version $Revision: #7 $
  \date $DateTime: 2010/09/06 11:58:56 $
  \author andrew.rhyder
  \brief CA Shape Widget Plugin.
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

#ifndef QCASHAPEPLUGIN_H
#define QCASHAPEPLUGIN_H

#include <QTimer>
#include <QString>
#include <QCaShape.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaStringFormatting.h>

//! ???
#define VARIABLE_NAME_PROPERTY_MANAGERS_SIZE 6

class QCaShapePlugin : public QCaShape {
    Q_OBJECT

  public:
    /// Constructors
    QCaShapePlugin( QWidget *parent = 0 );
    QCaShapePlugin( QString variableName, QWidget *parent = 0 );


    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable1 READ getVariableName1Property WRITE setVariableName1Property)
    void    setVariableName1Property( QString variable1Name ){ variableNamePropertyManagers[0].setVariableNameProperty( variable1Name ); }
    QString getVariableName1Property(){ return variableNamePropertyManagers[0].getVariableNameProperty(); }

    Q_PROPERTY(QString variable2 READ getVariableName2Property WRITE setVariableName2Property)
    void    setVariableName2Property( QString variable2Name ){ variableNamePropertyManagers[1].setVariableNameProperty( variable2Name ); }
    QString getVariableName2Property(){ return variableNamePropertyManagers[1].getVariableNameProperty(); }

    Q_PROPERTY(QString variable3 READ getVariableName3Property WRITE setVariableName3Property)
    void    setVariableName3Property( QString variable3Name ){ variableNamePropertyManagers[2].setVariableNameProperty( variable3Name ); }
    QString getVariableName3Property(){ return variableNamePropertyManagers[2].getVariableNameProperty(); }

    Q_PROPERTY(QString variable4 READ getVariableName4Property WRITE setVariableName4Property)
    void    setVariableName4Property( QString variable4Name ){ variableNamePropertyManagers[3].setVariableNameProperty( variable4Name ); }
    QString getVariableName4Property(){ return variableNamePropertyManagers[3].getVariableNameProperty(); }

    Q_PROPERTY(QString variable5 READ getVariableName5Property WRITE setVariableName5Property)
    void    setVariableName5Property( QString variable5Name ){ variableNamePropertyManagers[4].setVariableNameProperty( variable5Name ); }
    QString getVariableName5Property(){ return variableNamePropertyManagers[4].getVariableNameProperty(); }

    Q_PROPERTY(QString variable6 READ getVariableName6Property WRITE setVariableName6Property)
    void    setVariableName6Property( QString variable6Name ){ variableNamePropertyManagers[5].setVariableNameProperty( variable6Name ); }
    QString getVariableName6Property(){ return variableNamePropertyManagers[5].getVariableNameProperty(); }


    Q_ENUMS(animationOptions)
    enum animationOptions {Width = QCaShape::Width,
                           Height = QCaShape::Height,
                           X = QCaShape::X,
                           Y = QCaShape::Y,
                           Transperency = QCaShape::Transperency,
                           Rotation = QCaShape::Rotation,
                           ColourHue = QCaShape::ColourHue,
                           ColourSaturation = QCaShape::ColourSaturation,
                           ColourValue = QCaShape::ColourValue,
                           ColourIndex = QCaShape::ColourIndex,
                           Penwidth = QCaShape::Penwidth};

    Q_PROPERTY(animationOptions animation1 READ getAnimation1Property WRITE setAnimation1Property)
    void setAnimation1Property( animationOptions animation ){ setAnimation( (QCaShape::Animations)animation, 0 ); }
    animationOptions getAnimation1Property(){ return (animationOptions)getAnimation( 0 ); }

    Q_PROPERTY(animationOptions animation2 READ getAnimation2Property WRITE setAnimation2Property)
    void setAnimation2Property( animationOptions animation ){ setAnimation( (QCaShape::Animations)animation, 1 ); }
    animationOptions getAnimation2Property(){ return (animationOptions)getAnimation( 1 ); }

    Q_PROPERTY(animationOptions animation3 READ getAnimation3Property WRITE setAnimation3Property)
    void setAnimation3Property( animationOptions animation ){ setAnimation( (QCaShape::Animations)animation, 2 ); }
    animationOptions getAnimation3Property(){ return (animationOptions)getAnimation( 2 ); }

    Q_PROPERTY(animationOptions animation4 READ getAnimation4Property WRITE setAnimation4Property)
    void setAnimation4Property( animationOptions animation ){ setAnimation( (QCaShape::Animations)animation, 3 ); }
    animationOptions getAnimation4Property(){ return (animationOptions)getAnimation( 3 ); }

    Q_PROPERTY(animationOptions animation5 READ getAnimation5Property WRITE setAnimation5Property)
    void setAnimation5Property( animationOptions animation ){ setAnimation( (QCaShape::Animations)animation, 4 ); }
    animationOptions getAnimation5Property(){ return (animationOptions)getAnimation( 4 ); }

    Q_PROPERTY(animationOptions animation6 READ getAnimation6Property WRITE setAnimation6Property)
    void setAnimation6Property( animationOptions animation ){ setAnimation( (QCaShape::Animations)animation, 5 ); }
    animationOptions getAnimation6Property(){ return (animationOptions)getAnimation( 5 ); }

    Q_PROPERTY(double scale1 READ getScale1Property WRITE setScale1Property)
    void setScale1Property( double scale ){ setScale( scale, 0 ); }
    double getScale1Property(){ return getScale( 0 ); }

    Q_PROPERTY(double scale2 READ getScale2Property WRITE setScale2Property)
    void setScale2Property( double scale ){ setScale( scale, 1 ); }
    double getScale2Property(){ return getScale( 1 ); }

    Q_PROPERTY(double scale3 READ getScale3Property WRITE setScale3Property)
    void setScale3Property( double scale ){ setScale( scale, 2 ); }
    double getScale3Property(){ return getScale( 2 ); }

    Q_PROPERTY(double scale4 READ getScale4Property WRITE setScale4Property)
    void setScale4Property( double scale ){ setScale( scale, 3 ); }
    double getScale4Property(){ return getScale( 3 ); }

    Q_PROPERTY(double scale5 READ getScale5Property WRITE setScale5Property)
    void setScale5Property( double scale ){ setScale( scale, 4 ); }
    double getScale5Property(){ return getScale( 4 ); }

    Q_PROPERTY(double scale6 READ getScale6Property WRITE setScale6Property)
    void setScale6Property( double scale ){ setScale( scale, 5 ); }
    double getScale6Property(){ return getScale( 5 ); }


    Q_PROPERTY(double offset1 READ getOffset1Property WRITE setOffset1Property)
    void setOffset1Property( double offset ){ setOffset( offset, 0 ); }
    double getOffset1Property(){ return getOffset( 0 ); }

    Q_PROPERTY(double offset2 READ getOffset2Property WRITE setOffset2Property)
    void setOffset2Property( double offset ){ setOffset( offset, 1 ); }
    double getOffset2Property(){ return getOffset( 1 ); }

    Q_PROPERTY(double offset3 READ getOffset3Property WRITE setOffset3Property)
    void setOffset3Property( double offset ){ setOffset( offset, 2 ); }
    double getOffset3Property(){ return getOffset( 2 ); }

    Q_PROPERTY(double offset4 READ getOffset4Property WRITE setOffset4Property)
    void setOffset4Property( double offset ){ setOffset( offset, 3 ); }
    double getOffset4Property(){ return getOffset( 3 ); }

    Q_PROPERTY(double offset5 READ getOffset5Property WRITE setOffset5Property)
    void setOffset5Property( double offset ){ setOffset( offset, 4 ); }
    double getOffset5Property(){ return getOffset( 4 ); }

    Q_PROPERTY(double offset6 READ getOffset6Property WRITE setOffset6Property)
    void setOffset6Property( double offset ){ setOffset( offset, 5 ); }
    double getOffset6Property(){ return getOffset( 5 ); }

    Q_PROPERTY(bool border READ getBorder WRITE setBorder)
    Q_PROPERTY(bool fill READ getFill WRITE setFill)

    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ for( int i = 0; i < VARIABLE_NAME_PROPERTY_MANAGERS_SIZE; i++ ) variableNamePropertyManagers[i].setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManagers[0].getSubstitutionsProperty(); }

    Q_PROPERTY(bool subscribe READ getSubscribe WRITE setSubscribe)
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)

    Q_ENUMS(shapeOptions)
    Q_PROPERTY(shapeOptions shape READ getShapeProperty WRITE setShapeProperty)
    enum shapeOptions { Line = QCaShape::Line,
                  Points = QCaShape::Points,
                  Polyline = QCaShape::Polyline,
                  Polygon = QCaShape::Polygon,
                  Rect = QCaShape::Rect,
                  RoundedRect = QCaShape::RoundedRect,
                  Ellipse = QCaShape::Ellipse,
                  Arc = QCaShape::Arc,
                  Chord = QCaShape::Chord,
                  Pie = QCaShape::Pie,
                  Path = QCaShape::Path,
                  Text = QCaShape::Text,
                  Pixmap = QCaShape::Pixmap };
    void setShapeProperty( shapeOptions shape ){ setShape(( QCaShape::Shape)shape); }
    shapeOptions getShapeProperty(){ return (shapeOptions)getShape(); }


    Q_PROPERTY(unsigned int numPoints READ getNumPoints WRITE setNumPoints )
    Q_PROPERTY(QPoint originTranslation READ getOriginTranslation WRITE setOriginTranslation)

    Q_PROPERTY(QPoint point1 READ getPoint1Property WRITE setPoint1Property)
    void setPoint1Property( QPoint point ){ setPoint( point, 0 ); }
    QPoint getPoint1Property(){ return getPoint( 0 ); }

    Q_PROPERTY(QPoint point2 READ getPoint2Property WRITE setPoint2Property)
    void setPoint2Property( QPoint point ){ setPoint( point, 1 ); }
    QPoint getPoint2Property(){ return getPoint( 1 ); }

    Q_PROPERTY(QPoint point3 READ getPoint3Property WRITE setPoint3Property)
    void setPoint3Property( QPoint point ){ setPoint( point, 2 ); }
    QPoint getPoint3Property(){ return getPoint( 2 ); }

    Q_PROPERTY(QPoint point4 READ getPoint4Property WRITE setPoint4Property)
    void setPoint4Property( QPoint point ){ setPoint( point, 3 ); }
    QPoint getPoint4Property(){ return getPoint( 3 ); }

    Q_PROPERTY(QPoint point5 READ getPoint5Property WRITE setPoint5Property)
    void setPoint5Property( QPoint point ){ setPoint( point, 4 ); }
    QPoint getPoint5Property(){ return getPoint( 4 ); }

    Q_PROPERTY(QPoint point6 READ getPoint6Property WRITE setPoint6Property)
    void setPoint6Property( QPoint point ){ setPoint( point, 5 ); }
    QPoint getPoint6Property(){ return getPoint( 5 ); }

    Q_PROPERTY(QPoint point7 READ getPoint7Property WRITE setPoint7Property)
    void setPoint7Property( QPoint point ){ setPoint( point, 6 ); }
    QPoint getPoint7Property(){ return getPoint( 6 ); }

    Q_PROPERTY(QPoint point8 READ getPoint8Property WRITE setPoint8Property)
    void setPoint8Property( QPoint point ){ setPoint( point, 7 ); }
    QPoint getPoint8Property(){ return getPoint( 7 ); }

    Q_PROPERTY(QPoint point9 READ getPoint9Property WRITE setPoint9Property)
    void setPoint9Property( QPoint point ){ setPoint( point, 8 ); }
    QPoint getPoint9Property(){ return getPoint( 8 ); }

    Q_PROPERTY(QPoint point10 READ getPoint10Property WRITE setPoint10Property)
    void setPoint10Property( QPoint point ){ setPoint( point, 9 ); }
    QPoint getPoint10Property(){ return getPoint( 9 ); }


    Q_PROPERTY(QColor color1 READ getColor1Property WRITE setColor1Property)
    void setColor1Property( QColor color ){ setColor( color, 0 ); }
    QColor getColor1Property(){ return getColor( 0 ); }

    Q_PROPERTY(QColor color2 READ getColor2Property WRITE setColor2Property)
    void setColor2Property( QColor color ){ setColor( color, 1 ); }
    QColor getColor2Property(){ return getColor( 1 ); }

    Q_PROPERTY(QColor color3 READ getColor3Property WRITE setColor3Property)
    void setColor3Property( QColor color ){ setColor( color, 2 ); }
    QColor getColor3Property(){ return getColor( 2 ); }

    Q_PROPERTY(QColor color4 READ getColor4Property WRITE setColor4Property)
    void setColor4Property( QColor color ){ setColor( color, 3 ); }
    QColor getColor4Property(){ return getColor( 3 ); }

    Q_PROPERTY(QColor color5 READ getColor5Property WRITE setColor5Property)
    void setColor5Property( QColor color ){ setColor( color, 4 ); }
    QColor getColor5Property(){ return getColor( 4 ); }

    Q_PROPERTY(QColor color6 READ getColor6Property WRITE setColor6Property)
    void setColor6Property( QColor color ){ setColor( color, 5 ); }
    QColor getColor6Property(){ return getColor( 5 ); }

    Q_PROPERTY(QColor color7 READ getColor7Property WRITE setColor7Property)
    void setColor7Property( QColor color ){ setColor( color, 6 ); }
    QColor getColor7Property(){ return getColor( 6 ); }

    Q_PROPERTY(QColor color8 READ getColor8Property WRITE setColor8Property)
    void setColor8Property( QColor color ){ setColor( color, 7 ); }
    QColor getColor8Property(){ return getColor( 7 ); }

    Q_PROPERTY(QColor color9 READ getColor9Property WRITE setColor9Property)
    void setColor9Property( QColor color ){ setColor( color, 8 ); }
    QColor getColor9Property(){ return getColor( 8 ); }

    Q_PROPERTY(QColor color10 READ getColor10Property WRITE setColor10Property)
    void setColor10Property( QColor color ){ setColor( color, 9 ); }
    QColor getColor10Property(){ return getColor( 9 ); }


    Q_PROPERTY(bool drawBorder READ getDrawBorder WRITE setDrawBorder)
    Q_PROPERTY(unsigned int lineWidth READ getLineWidth WRITE setLineWidth)
    Q_PROPERTY(double startAngle READ getStartAngle WRITE setStartAngle)
    Q_PROPERTY(double rotation READ getRotation WRITE setRotation)
    Q_PROPERTY(double arcLength READ getArcLength WRITE setArcLength)
    Q_PROPERTY(QString text READ getText WRITE setText)

  private:
    QCaVariableNamePropertyManager variableNamePropertyManagers[VARIABLE_NAME_PROPERTY_MANAGERS_SIZE];

  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );
};

#endif /// QCASHAPEPLUGIN_H
