
#ifndef HbSmiBrowse_h
#define HbSmiBrowse_h

#include <smiuirtl.hxx>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <vector>
#include <QTimer>
#include "smidevice.h"

using namespace std;

class HbSmiBrowse;


//====================================================

class SmiQWidget {
 //  Q_OBJECT
private:

public:
    SmiQWidget(char *domain_name, char *obj_name) {

      this->domain_name=string(domain_name);
      if(strstr(obj_name,"::"))
       this->obj_name=string(obj_name);
      else this->obj_name=string(domain_name)+"::"+string(obj_name);
        char tmp[81]; // temporal
        strcpy(tmp, this->obj_name.c_str());
       // char *dom;
       // char *obj;
       // printf("tmp1\n"); fflush(stdout);
        splitDomainAndObjName(tmp/*, dom, obj*/);
        // printf("tmp2\n"); fflush(stdout);
      //  printf("++++++++++++++++++++++++++++++++ %s\n", obj); fflush(stdout);
        obj_name_short=string(tmp);

//        int pos;
//        string short_name;

     //       short_name=obj_name_short;

        if(!strncasecmp("B_",(char *)obj_name_short.c_str(),2)){
            is_device=true;
            char tmp[81];// temporal
            strcpy(tmp,(char *)obj_name_short.c_str());

//            BoardButton::boardNameParser(tmp);
  //          sprintf(tmp,"BOARD %d",BoardButton::board);
            int crate, board, board_part;
            BoardButton::boardNameParserStatic(tmp, crate, board, board_part);
            sprintf(tmp,"Board %d Part %d", board, board_part);
            sprintf(tmp,"Board %d", board);
            alias_name=string(tmp);
        }
        else {
            is_device=false;
            if(!strncasecmp("HV_",(char *)obj_name_short.c_str(),3))
                 alias_name=string("HV");
            else if(!strncasecmp("LV_",(char *)obj_name_short.c_str(),3))
                 alias_name=string("LV");
            else if(!strncasecmp("GAS_",(char *)obj_name_short.c_str(),4))
                 alias_name=string("GAS");
            else alias_name=obj_name_short;

        }
    }
 private:

   string domain_name;
   string obj_name;
   string obj_name_short;
   string alias_name;

   bool is_device;

int splitDomainAndObjName(char *smi_full_name/*, char *smi_domain_name, char *smi_obj_name*/);
friend class HbSmiBrowse;
friend class SmiQPushButton;
friend class SmiQLabel;
friend class SmiQComboBox;
};
//====================================================

class SmiQPushButton :  public QPushButton , public SmiQWidget{

    Q_OBJECT

private:

public:
    SmiQPushButton(QWidget *widget, char *domain_name, char *obj_name) : QPushButton(widget), SmiQWidget(domain_name, obj_name) {
  ///    this->domain_name=string(domain_name);
  ///    this->obj_name=string(obj_name);
    }
 private:

 ///  string domain_name;
 ///  string obj_name;
 ///  string obj_name_short;
private slots:

/// calls the findChildrenObjectsbyName
   void s_to_browse_this_smi_object();
friend class HbSmiBrowse;
};
//====================================================
class SmiQComboBox : public QComboBox , public SmiQWidget{

    Q_OBJECT
friend class HbSmiBrowse;
private:

public:
       vector <QString> actions;

    SmiQComboBox(QWidget *widget, char *domain_name, char *obj_name) :  QComboBox(widget), SmiQWidget(domain_name, obj_name){
     /// this->domain_name=string(domain_name);
     /// this->obj_name=string(obj_name);
      actions.clear();


    }

 private:
   //QString smi_domain;

 ///  string domain_name;
 ///  string obj_name;
 ///  string obj_name_short;

private slots:

///call the findChildrenObjectsbyName
 void s_to_send_command_for_this_smi_object(int index);

};
//====================================================
class SmiQLabel : public QLabel , public SmiQWidget{

    Q_OBJECT

private:

public:

    QString current_state;
    //   vector <QString> actions;

    SmiQLabel(QWidget *widget, char *domain_name, char *obj_name) : QLabel(widget), SmiQWidget(domain_name, obj_name){
    ///  this->domain_name=string(domain_name);
    ///  this->obj_name=string(obj_name);
     // actions.clear();
    }

 private:
   //QString smi_domain;

 ///  string domain_name;
 ///  string obj_name;
 ///  string obj_name_short;
private slots:

///call the findChildrenObjectsbyName
 ///void s_to_send_command_for_this_smi_object(int index);
friend class HbSmiBrowse;
};
/*
//====================================================
class MySmiObject : public SmiObject{


public:
 //void smiStateChangeHandler ( );

// MySmiObject(char *domain, char *name);

 MySmiObject(char *domain, char *name) : SmiObject(domain, name) {

 }

};
*/
/*
//====================================================
class MySmiObjectSet : public SmiObjectSet{


public:
 void smiObjSetChangeHandler();

// MySmiObject(char *domain, char *name);

 MySmiObjectSet(char *domain, char *name) : SmiObjectSet(domain, name) {

 }

};
*/
//====================================================

class HbSmiBrowse : public QObject{  // Hall B smi browsing


     Q_OBJECT

friend class SmiQPushButton;
friend class MySmiObject;
friend class MainWindow;

private:

static int node_type; // smi folder or smi device

const static int GX_BASE=20;
const static int GY_PATH=20;
const static int GW_PATH=850;
const static int GX_PARENT=GX_BASE+50;
const static int GY_BASE=20+20;
const static int GY_SHIFT=24;
const static int GY_PARENT=GY_BASE+GY_SHIFT;
const static int GW_PARENT_BUTTON=180;
const static int GW_PARENT_COMBO=180;
const static int GW_PARENT_LABEL=160;

const static int GH_NODE=20;
const static int GGAP_SMALL=2;
const static int GGAP_BIG=40;

const static int GX_CHILD=GX_PARENT+50;
const static int GY_CHILD=GY_PARENT+GGAP_BIG;
const static int GW_CHILD_BUTTON=GW_PARENT_BUTTON;
const static int GW_CHILD_COMBO=GW_PARENT_COMBO;
const static int GW_CHILD_LABEL=GW_PARENT_LABEL;

const static int GX_BACK=GX_PARENT+GW_PARENT_BUTTON+GW_PARENT_COMBO+200;
const static int GY_BACK=GY_PARENT;
const static int GW_BACK=80;
const static int GH_BACK=GH_NODE;

//public:

 /// static vector< MySmiObject * > smi_children_objects;

 /// static MySmiObject *smi_parent_object;
///  static SmiObjectSet *objects_set;

  static vector< string > children_object_names;
  static vector< int > children_object_ids;
  static string parent_name;
  static string parent_alias_name;
  //static string prev_parent_name;
  string objects_set_name;

  static vector< SmiQPushButton * >  children_buttons;
  static vector< SmiQLabel * >  children_labels;
  static vector< SmiQComboBox * >  children_combos;
  static vector<int>  children_update_needs;

  static int parent_update_need;
  static int parent_id;
  static SmiQPushButton *parent_button;
  static SmiQLabel *parent_label;
  static SmiQComboBox *parent_combo;

  static QLabel *path_label;
  static SmiQPushButton *back_button;

/// a) opens parent smi_object (named as smi_full_name) as MySmiObject
/// b) finds all smi_children objects and puts them into smi_children_objects


  static int findIndexByName(char *obj_name, int &index);
  static void cleanCurrentUiTree();

public:
  HbSmiBrowse(){

      //prev_parent_name=string("");
      parent_name=string("");

      back_button=0;
      parent_button=0;
      parent_combo=0;
      parent_label=0;
      path_label=0;

      ///smi_parent_object=0;
      ///objects_set=0;

      children_update_needs.clear();
      children_buttons.clear();
      children_object_names.clear();
      children_labels.clear();
      children_combos.clear();
      children_object_ids.clear();

      ///smi_children_objects.clear();
      set_done=0;
      rate_update=3;

  }


  static void *mysmi_set_handler();
  static void *mysmi_object_handler(int *id);

  static QTimer *timer_update;
  static int rate_update;
  static int set_id; // my_new
  static int set_done; // my_new
  static string button_domain_name;
  static string button_obj_name_short;
  static string button_alias_name;

  static int findChildrenObjectsbyName(int mode/*char *domain_name, char *obj_name*/);

 private slots:
  static void cb_update();


};
//====================================================

#endif

