//******************************************************************************
// Copyright (c) 2012 Paul Scherrer Institut PSI), Villigen, Switzerland
// Disclaimer: neither  PSI, nor any of their employees makes any warranty
// or assumes any legal liability or responsibility for the use of this software
//******************************************************************************
//******************************************************************************
//
//     Author : Anton Chr. Mezger
//
//******************************************************************************

#ifndef CACARTESIANPLOT_H
#define CACARTESIANPLOT_H
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_symbol.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_marker.h>
#include <QMouseEvent>
#include <QVarLengthArray>
#include <qtcontrols_global.h>

class QTCON_EXPORT caCartesianPlot : public QwtPlot
{
    Q_OBJECT

#if QWT_VERSION >= 0x060100
    // suppress theese properties for the designer
    Q_PROPERTY( QBrush canvasBackground READ canvasBackground WRITE setCanvasBackground DESIGNABLE false)
    Q_PROPERTY( bool autoReplot READ autoReplot WRITE setAutoReplot DESIGNABLE false)
#endif

    Q_ENUMS(curvSymbol)
    Q_ENUMS(curvStyle)
    Q_ENUMS(axisScaling)
    Q_ENUMS(plMode)
    Q_ENUMS(eraMode)

    Q_PROPERTY(QString Title READ getTitlePlot WRITE setTitlePlot)
    Q_PROPERTY(QString TitleX READ getTitleX WRITE setTitleX)
    Q_PROPERTY(QString TitleY READ getTitleY WRITE setTitleY)

    Q_PROPERTY(QString channels_1 READ getPV_1 WRITE setPV_1)
    Q_PROPERTY(curvStyle Style_1 READ getStyle_1 WRITE setStyle_1)
    Q_PROPERTY(curvSymbol symbol_1 READ getSymbol_1 WRITE setSymbol_1)
    Q_PROPERTY(QColor color_1 READ getColor_1 WRITE setColor_1)

    Q_PROPERTY(QString channels_2 READ getPV_2 WRITE setPV_2)
    Q_PROPERTY(curvStyle Style_2 READ getStyle_2 WRITE setStyle_2)
    Q_PROPERTY(curvSymbol symbol_2 READ getSymbol_2 WRITE setSymbol_2)
    Q_PROPERTY(QColor color_2 READ getColor_2 WRITE setColor_2)

    Q_PROPERTY(QString channels_3 READ getPV_3 WRITE setPV_3)
    Q_PROPERTY(curvStyle Style_3 READ getStyle_3 WRITE setStyle_3)
    Q_PROPERTY(curvSymbol symbol_3 READ getSymbol_3 WRITE setSymbol_3)
    Q_PROPERTY(QColor color_3 READ getColor_3 WRITE setColor_3)

    Q_PROPERTY(QString channels_4 READ getPV_4 WRITE setPV_4)
    Q_PROPERTY(curvStyle Style_4 READ getStyle_4 WRITE setStyle_4)
    Q_PROPERTY(curvSymbol symbol_4 READ getSymbol_4 WRITE setSymbol_4)
    Q_PROPERTY(QColor color_4 READ getColor_4 WRITE setColor_4)

    Q_PROPERTY(QString channels_5 READ getPV_5 WRITE setPV_5)
    Q_PROPERTY(curvStyle Style_5 READ getStyle_5 WRITE setStyle_5)
    Q_PROPERTY(curvSymbol symbol_5 READ getSymbol_5 WRITE setSymbol_5)
    Q_PROPERTY(QColor color_5 READ getColor_5 WRITE setColor_5)

    Q_PROPERTY(QString channels_6 READ getPV_6 WRITE setPV_6)
    Q_PROPERTY(curvStyle Style_6 READ getStyle_6 WRITE setStyle_6)
    Q_PROPERTY(curvSymbol symbol_6 READ getSymbol_6 WRITE setSymbol_6)
    Q_PROPERTY(QColor color_6 READ getColor_6 WRITE setColor_6)

    Q_PROPERTY(plMode plotMode READ getPlotMode WRITE setPlotMode)

    Q_PROPERTY(QString countNumOrChannel READ getCountPV WRITE setCountPV)
    Q_PROPERTY(QString triggerChannel READ getTriggerPV WRITE setTriggerPV)
    Q_PROPERTY(QString eraseChannel READ getErasePV WRITE setErasePV)

    Q_PROPERTY(eraMode eraseMode READ getEraseMode WRITE setEraseMode)

    Q_PROPERTY(QColor foreground READ getForeground WRITE setForeground)
    Q_PROPERTY(QColor background READ getBackground WRITE setBackground)
    Q_PROPERTY(QColor scaleColor READ getScaleColor WRITE setScaleColor)

    Q_PROPERTY(bool grid READ getGrid WRITE setGrid)
    Q_PROPERTY(QColor gridColor READ getGridColor WRITE setGridColor)

    Q_PROPERTY(axisScaling XaxisScaling READ getXscaling WRITE setXscaling)
    Q_PROPERTY(axisScaling YaxisScaling READ getYscaling WRITE setYscaling)

    Q_PROPERTY(QString XaxisLimits  READ getXaxisLimits WRITE setXaxisLimits)
    Q_PROPERTY(QString YaxisLimits  READ getYaxisLimits WRITE setYaxisLimits)

    Q_PROPERTY(bool XaxisEnabled READ getXaxisEnabled WRITE setXaxisEnabled)
    Q_PROPERTY(bool YaxisEnabled READ getYaxisEnabled WRITE setYaxisEnabled)

public:

    bool getXaxisEnabled() const { return thisXshow; }
    void setXaxisEnabled(bool thisXshow);
    bool getYaxisEnabled() const { return thisYshow; }
    void setYaxisEnabled(bool thisYshow);

    enum {curveCount = 6};

    enum axisScaling { Auto = 0, Channel, User};

    enum curvSymbol {  NoSymbol = -1,
                       Ellipse,
                       Rect,
                       Diamond,
                       Triangle,
                       DTriangle,
                       UTriangle,
                       LTriangle,
                       RTriangle,
                       Cross,
                       XCross,
                       HLine,
                       VLine,
                       Star1,
                       Star2,
                       Hexagon};

    enum  curvStyle { NoCurve = 0,
      Lines,
      Sticks,
      Steps,
      Dots,
      FillUnder,
      ThinLines,
      HorSticks
    };

    enum plMode {
        PlotNPointsAndStop = 0,
        PlotLastNPoints
    };

    enum eraMode {
        ifnotzero = 0,
        ifzero
    };

    enum  CartesianChannelType { CH_X=0, CH_Y, CH_Trigger, CH_Count, CH_Erase, CH_Xscale, CH_Yscale};

    enum curvType {XY_nothing=0, X_only, Y_only, XY_both};

    QString getXaxisLimits() const {return thisXaxisLimits.join(";");}
    void setXaxisLimits(QString const &newX);

    QString getYaxisLimits() const {return thisYaxisLimits.join(";");}
    void setYaxisLimits(QString const &newY);

    QString getTitlePlot() const {return thisTitle;}
    void setTitlePlot(QString const &title);

    QString getTitleX() const {return thisTitleX;}
    void setTitleX(QString const &title);

    QString getTitleY() const {return thisTitleY;}
    void setTitleY(QString const &title);

    QString getPV_1() const {return thisPV[0].join(";");}
    void setPV_1(QString const &newPV) {thisPV[0] = newPV.split(";");}

    QString getPV_2() const {return thisPV[1].join(";");}
    void setPV_2(QString const &newPV) {thisPV[1] = newPV.split(";");}

    QString getPV_3() const {return thisPV[2].join(";");}
    void setPV_3(QString const &newPV)  {thisPV[2] = newPV.split(";");}

    QString getPV_4() const {return thisPV[3].join(";");}
    void setPV_4(QString const &newPV)  {thisPV[3] = newPV.split(";");}

    QString getPV_5() const {return thisPV[4].join(";");}
    void setPV_5(QString const &newPV)  {thisPV[4] = newPV.split(";");}

    QString getPV_6() const {return thisPV[5].join(";");}
    void setPV_6(QString const &newPV)  {thisPV[5] = newPV.split(";");}

    QString getTriggerPV() const {return thisTriggerPV;}
    void setTriggerPV(QString const &newPV);

    QString getErasePV() const {return thisErasePV;}
    void setErasePV(QString const &newPV)  {thisErasePV = newPV;}

    QString getCountPV() const {return thisCountPV;}
    void setCountPV(QString const &newPV);

    bool hasCountNumber(int *Number);
    void setCountNumber(int number);

    plMode getPlotMode() const {return thisPlotMode;}
    void setPlotMode(plMode mode) {thisPlotMode = mode;}

    eraMode getEraseMode() const {return thisEraseMode;}
    void setEraseMode(eraMode mode) {thisEraseMode = mode;}

    curvSymbol getSymbol_1() const {return thisSymbol[0];}
    void setSymbol_1(curvSymbol s);

    curvSymbol getSymbol_2() const {return thisSymbol[1];}
    void setSymbol_2(curvSymbol s);

    curvSymbol getSymbol_3() const {return thisSymbol[2];}
    void setSymbol_3(curvSymbol s);

    curvSymbol getSymbol_4() const {return thisSymbol[3];}
    void setSymbol_4(curvSymbol s);

    curvSymbol getSymbol_5() const {return thisSymbol[4];}
    void setSymbol_5(curvSymbol s);

    curvSymbol getSymbol_6() const {return thisSymbol[5];}
    void setSymbol_6(curvSymbol s);

    curvStyle getStyle_1() const {return thisStyle[0];}
    void setStyle_1(curvStyle s);

    curvStyle getStyle_2() const {return thisStyle[1];}
    void setStyle_2(curvStyle s);

    curvStyle getStyle_3() const {return thisStyle[2];}
    void setStyle_3(curvStyle s);

    curvStyle getStyle_4() const {return thisStyle[3];}
    void setStyle_4(curvStyle s);

    curvStyle getStyle_5() const {return thisStyle[4];}
    void setStyle_5(curvStyle s);

    curvStyle getStyle_6() const {return thisStyle[5];}
    void setStyle_6(curvStyle s);

    QColor getColor_1() const {return thisLineColor[0];}
    void setColor_1(QColor c);

    QColor getColor_2() const {return thisLineColor[1];}
    void setColor_2(QColor c);

    QColor getColor_3() const {return thisLineColor[2];}
    void setColor_3(QColor c);

    QColor getColor_4() const {return thisLineColor[3];}
    void setColor_4(QColor c);

    QColor getColor_5() const {return thisLineColor[4];}
    void setColor_5(QColor c);

    QColor getColor_6() const {return thisLineColor[5];}
    void setColor_6(QColor c);

    QColor getForeground() const {return thisForeColor;}
    void setForeground(QColor c);

    QColor getBackground() const {return thisBackColor;}
    void setBackground(QColor c);

    QColor getScaleColor() const {return thisScaleColor;}
    void setScaleColor(QColor c);

    bool getGrid() const {return thisGrid;}
    void setGrid(bool m);

    QColor getGridColor() const {return thisGridColor;}
    void setGridColor(QColor c);

    axisScaling getXscaling() const {return thisXscaling;}
    void setXscaling(axisScaling s);
    axisScaling getYscaling() const {return thisYscaling;}
    void setYscaling(axisScaling s);

    caCartesianPlot(QWidget *parent);

    QwtSymbol::Style myMarker(curvSymbol m);
    QwtPlotCurve::CurveStyle myStyle(curvStyle s);

    void  setData(const QVector<double>& vector, int curvIndex, int curvType, int curvXY);
    void setScaleX(double minX, double maxX);
    void setScaleY(double minY, double maxY);
    int getYLimits(double &minY, double &maxY);
    int getXLimits(double &minX, double &maxX);
    void setScaleXlimits(double value, int maxormin);
    void setScaleYlimits(double value, int maxormin);

    void erasePlots();
    void setWhiteColors();
    void setAllProperties();

    void setSymbol(curvSymbol s, int indx);
    curvSymbol getSymbol(int indx) const {return thisSymbol[indx];}

    void setColor(QColor c, int indx);
    QColor getColor(int indx) const {return thisLineColor[indx];}

    void setStyle(curvStyle s, int indx);
    curvStyle getStyle(int indx) const {return thisStyle[indx];}

    QString getPV(int indx) const {return thisPV[indx].join(";");}
    void setPV(QString const &newPV, int indx)  {thisPV[indx] = newPV.split(";");}

signals:

    void ShowContextMenu(const QPoint&);

protected:

    void resizeEvent ( QResizeEvent * event);

private:

    QString thisTitle, thisTitleX, thisTitleY, thisTriggerPV, thisCountPV, thisErasePV;
    QStringList	 thisPV[curveCount], thisXaxisLimits, thisYaxisLimits;
    QColor thisForeColor;
    QColor thisBackColor;
    QColor thisScaleColor;
    QColor thisGridColor;
    QColor thisLineColor[curveCount];
    curvSymbol thisSymbol[curveCount];
    curvStyle thisStyle[curveCount];
    plMode thisPlotMode;
    eraMode thisEraseMode;
    int thisCountNumber;

    bool thisGrid;
    axisScaling thisXscaling, thisYscaling;

    double AxisLowX;
    double AxisUpX;
    double AxisLowY;
    double AxisUp;

    QwtPlotCurve curve[curveCount];

    QVarLengthArray<double> X[curveCount];
    QVarLengthArray<double> Y[curveCount];

    QVarLengthArray<double> accumulX[curveCount];
    QVarLengthArray<double> accumulY[curveCount];

    QwtPlotGrid *plotGrid;
    QPen penGrid;

    bool thisXshow, thisYshow, thisToBeTriggered;

    void setBackgroundColor(QColor c);
    void setForegroundColor(QColor c);
    void setScalesColor(QColor c);
    void setGridsColor(QColor c);

    bool eventFilter(QObject *obj, QEvent *event);

};

#endif
