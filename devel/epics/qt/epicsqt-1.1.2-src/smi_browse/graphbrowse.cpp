#include "graphbrowse.h"
//#include <smiuirtl.h>
#include "mainwindow.h"
#include <QPainter>
//#include "smidevice.h"
//#include "smibrowse.h"

vector<int> GrBrowse::level_numbers[LEVELS_NUMBER];
vector< string > GrBrowse::level_names[LEVELS_NUMBER] ;
vector< string > GrBrowse::alias_level_names[LEVELS_NUMBER] ;

vector<GrQLabel *> GrBrowse::level_labels[LEVELS_NUMBER];
vector< string > GrBrowse::shown_smi_nodes;
bool GrBrowse::is_visible;

extern MainWindow *mainw;
extern QPainter *mypaint;
extern QWidget *tree_holder;
//extern HbSmiBrowse *smibrowse;
//========================================================================================



int numbers_by_level[LEVELS_NUMBER][16]=
{
{4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
{3,3,3,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
{2,1,1,2,1,1,2,1,1,2,1,1,-1,-1,-1,-1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

char *names_by_level[LEVELS_NUMBER][16]=
{
{"DETECTOR","","","", "","","","", "","","",""},
{"CHAMBER_0","CHAMBER_1","CHAMBER_2","CHAMBER_3", "","","","", "","","",""},
{"HV_CHAMBER_0","LV_CHAMBER_0","GAS_CHAMBER_0","HV_CHAMBER_1","LV_CHAMBER_1","GAS_CHAMBER_1",
 "HV_CHAMBER_2","LV_CHAMBER_2","GAS_CHAMBER_2","HV_CHAMBER_3","LV_CHAMBER_3","GAS_CHAMBER_3","","","",""},
{"B_HV000_0_P0","B_HV000_2_P0","","","B_HV000_4_P0","B_HV000_6_P0","","",
 "B_HV000_7_P0","B_HV000_8_P0","","","B_HV000_9_P0","B_HV000_10_P0","",""}
};
char *alias_names_by_level[LEVELS_NUMBER][16]=
{
{"DET","","","", "","","","","","","","","","","",""},
{"CH0","CH1","CH2","CH3", "","","","","","","","","","","",""},
{"HV","LV","GAS","HV","LV","GAS",
 "HV","LV","GAS","HV","LV","GAS","","","",""},
{"B0","B1","","","B0","B1","","",
 "B0","B1","","","B0","B1","",""}
};
//========================================================================================
GrBrowse::GrBrowse(){

 //   tree_holder->setGeometry(GX_CENTER-G_WID/2, GYY_BASE, G_WID, G_HEI);
 //   tree_holder->setStyleSheet
                ( " background : lightgreen; border: 2px solid red ; ");
 int size;
 for(int i=0;i<LEVELS_NUMBER;i++){
    // printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& %d \n",i);fflush(stdout);

  size=sizeof(numbers_by_level[i])/sizeof(numbers_by_level[i][0]);
  for(int j=0;j<size;j++){
   if(numbers_by_level[i][j]>=0){
    level_numbers[i].push_back(numbers_by_level[i][j]);
  //  printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& %s %d %d %d \n",names_by_level[i][j],i,j,size);fflush(stdout);
    level_names[i].push_back(string(names_by_level[i][j]));
    alias_level_names[i].push_back(string(alias_names_by_level[i][j]));
   // level_labels[i].push_back(new GrQLabel(mainw,size,i,j));
   }
  }
  int cur_sum=0;
  //size=level_numbers[i].size();
  for(unsigned int j=0;j<level_numbers[i].size();j++){
    level_labels[i].push_back(new GrQLabel(mainw,cur_sum,i,j));
    cur_sum+=level_numbers[i][j];
  }

 }

 for(int i=0;i<LEVELS_NUMBER;i++){
    // printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& %d \n",i);fflush(stdout);

  for(unsigned int j=0;j<level_numbers[i].size();j++){
   level_labels[i][j]->buildLines();
  }
 }

}

//========================================================================================

void GrBrowse::draw_all_lines(){
  //  painter.setRenderHint(QPainter::Antialiasing, true);
  //  mypaint->setPen(QPen( QColor(0xa0,0xa0,0xa0), 2));
    static int c1=0;
    c1++;
    mypaint->setPen(QPen( QColor(0xa0,0xa0,0xa0), 2));

    for(int i=0;i<LEVELS_NUMBER;i++){
       // printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& %d %d \n",i,level_numbers[i].size());fflush(stdout);

     for(unsigned int j=0;j<level_numbers[i].size();j++){
         if(is_visible)level_labels[i][j]->setVisible(true);
         else level_labels[i][j]->setVisible(false);

         if(level_labels[i][j]->smi_name!="" && is_visible)
         (level_labels[i][j])->setStyleSheet
                    ("QLabel { background : #a0a0a0 ; border: 2px solid black ; color : black; qproperty-alignment: AlignCenter;}");

         if(!is_visible)
         mypaint->setPen(QPen( QColor(210,210,210), 2)); /// color is the same as mainw background
         level_labels[i][j]->draw_lines();
     }
    }

    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& is_visible %d c1=%d\n",is_visible, c1);fflush(stdout);

    if(is_visible)
    for(int i=0;i<LEVELS_NUMBER;i++){

     for(unsigned int j=0;j<level_numbers[i].size();j++){
         if((level_labels[i][j])->smi_name==shown_smi_nodes[0]){
          (level_labels[i][j])->setStyleSheet
                     ("QLabel { background : lightgreen; border: 2px solid red ; color : red; qproperty-alignment: AlignCenter;}");
          for(unsigned int i10=0;i10<level_labels[i][j]->children_boxes.size();i10++){
              if(level_labels[i][j]->children_boxes[i10]->smi_name!="")
              level_labels[i][j]->children_boxes[i10]->setStyleSheet
                      ("QLabel { background : lightgreen; border: 2px solid red ; color : red; qproperty-alignment: AlignCenter;}");
          }
          mypaint->setPen(QPen( QColor(0xff,0x0,0x0), 2));
          level_labels[i][j]->draw_lines();

         }
     }

    }


}
//========================================================================================

void GrQLabel::draw_lines(){

    for(int j=0;j< lines_to_children.size();j++){

     mypaint->drawLine(*(lines_to_children[j]));

    // lines_to_children[lines_to_children.size()-1]->setVisible(true);
    }

}

//========================================================================================

void GrQLabel::buildLines(){

 for(int j=n_first_child;j<(n_first_child+chilren_number);j++){

  int x1=ix+iw/2;
  int y1=iy+ih;
  int x2=GrBrowse::level_labels[its_level+1][j]->ix+(GrBrowse::level_labels[its_level+1][j]->iw)/2;
  int y2=GrBrowse::level_labels[its_level+1][j]->iy;
  lines_to_children.push_back(new QLine(x1,y1,x2,y2));
  children_boxes.push_back(GrBrowse::level_labels[its_level+1][j]);
  // printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& lev=%d num=%d first_ch=%d number_ch=%d \n",
    //      its_level, its_number, n_first_child,chilren_number);fflush(stdout);
// mypaint->drawLine(*(lines_to_children[lines_to_children.size()-1]));
 // lines_to_children[lines_to_children.size()-1]->setVisible(true);
 }

}


//===================================================================
//===================================================================
