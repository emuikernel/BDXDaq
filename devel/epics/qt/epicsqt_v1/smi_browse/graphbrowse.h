
#ifndef HbGraphBrowse_h
#define HbGraphBrowse_h

//#include <smiuirtl.hxx>
//#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QLine>
//#include <QtGui/QComboBox>
#include <vector>
//#include <QTimer>
//#include "smidevice.h"

using namespace std;


class GrQLabel;
#define LEVELS_NUMBER 4


/*
const static int GX_CENTER=400.;
const static int GY_BASE=150.;
const static int  G_WID=600.;
const static int G_HEI=300.;
const static int G_BW=30;
const static int G_BH=30;

const static int GX_GAP=20.;
const static int GY_GAP=30.;
*/

//====================================================

class GrBrowse : public QObject{  // Hall B graph browsing

     Q_OBJECT

friend class GrQLabel;
//friend class MySmiObject;

private:


static vector<int> level_numbers[LEVELS_NUMBER];
static vector< string > level_names[LEVELS_NUMBER] ;
static vector< string > alias_level_names[LEVELS_NUMBER] ;

static vector<GrQLabel *> level_labels[LEVELS_NUMBER];


public:


static vector< string > shown_smi_nodes; // filled in the smi browsing
static bool is_visible;


GrBrowse();
void draw_all_lines();
void paintBoxes();

};
//====================================================
#define GX_CENTER 450.
#define GYY_BASE 300.
#define  G_WID 700.
#define G_HEI 300.
#define G_BW 30.
#define G_BH 30.

#define GX_GAP 20.
#define GY_GAP 30.
//====================================================
class GrQLabel : public QLabel {//, public SmiQWidget{

    Q_OBJECT

private:

public:

GrQLabel(QWidget *widget, int cur_sum, int i, int j) : QLabel(widget) { //, SmiQWidget(domain_name, obj_name){

 int size=GrBrowse::level_numbers[i].size();

 float x_gap=(G_WID - ((float)size)*G_BW)/((float)(size+1));
 float w=x_gap*(size+1)+G_BW*size;
 //float x_base;
 float x=(GX_CENTER-(w/2.))+x_gap*(j+1)+G_BW*j;
 if(i==0){
 //  printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& %f %f %f %d \n",x_gap,w,x, size);fflush(stdout);

 }
 float y=GYY_BASE+GY_GAP*i+G_BH*i;
 ix=(int)x; iy=(int)y; iw=(int)G_BW; ih=(int)G_BH;

 setGeometry(QRect(ix,iy,iw,ih));
 if(GrBrowse::alias_level_names[i][j]!=""){
     setStyleSheet
     ("QLabel { background : lightgreen; border: 1px solid black ; color : black; qproperty-alignment: AlignCenter;}");
    // setFrameShape(QFrame::Box);
     //setStyleSheet("QLabel { qproperty-alignment: AlignCenter;}");
     setText(GrBrowse::alias_level_names[i][j].c_str());
     setVisible(true);
 }

 its_level=i;
 its_number=j;
 n_first_child=cur_sum;
 chilren_number=GrBrowse::level_numbers[i][j];
 smi_name=GrBrowse::level_names[i][j];
 children_boxes.clear();
// printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& lev=%d num=%d first_ch=%d number_ch=%d \n",
  //         its_level, its_number, n_first_child,chilren_number);fflush(stdout);
}


 int its_level;
 int its_number;
 int ix, iy, iw, ih;
 int n_first_child;
 int chilren_number;
 string smi_name;

 void buildLines();
 void draw_lines();
 private:
   vector<QLine *> lines_to_children;
   vector<GrQLabel *> children_boxes;
  // QRect *box_rect;

private slots:

friend class GrBrowse;
};
//====================================================
//====================================================

#endif

