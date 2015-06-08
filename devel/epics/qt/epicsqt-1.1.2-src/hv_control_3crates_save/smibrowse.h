#include <smiuirtl.hxx>
#include <QtGui/QPushButton>
#include <vector>

using namespace std;

#ifndef BhSmiBrowse_h
#define BhSmiBrowse_h

//====================================================

class SmiQPushButton : public QPushButton {

    Q_OBJECT

private:

public:
    SmiQPushButton(QWidget *widget, char *domain_name, char *obj_name) :  QPushButton(widget){
      this->domain_name=string(domain_name);
      this->obj_name=string(obj_name);
    }
 private:
   //QString smi_domain;
   string domain_name;
   string obj_name;
private slots:

/// call the findChildrenObjectsbyName
   void s_to_browse_this_smi_object();

};

//====================================================
class MySmiObject : public SmiObject{


public:
 void smiStateChangeHandler ( );

// MySmiObject(char *domain, char *name);

 MySmiObject(char *domain, char *name) : SmiObject(domain, name) {

 }

};
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



class HbSmiBrowse {  // Hall B smi browsing

friend class SmiQPushButton;
friend class MySmiObject;

private:


//public:

  static vector< MySmiObject * > smi_children_objects;
  static MySmiObject *smi_parent_object;
  static SmiObjectSet *objects_set;

  static vector< string > children_object_names;
  static string parent_name;
  static string prev_parent_name;
  string objects_set_name;

  static vector< SmiQPushButton * >  children_buttons;
  static SmiQPushButton *parent_button;
  static SmiQPushButton *back_button;

/// a) opens parent smi_object (named as smi_full_name) as MySmiObject
/// b) finds all smi_children objects and puts them into smi_children_objects
  static int findChildrenObjectsbyName(char *domain_name, char *obj_name);
  int splitDomainAnsObjName(char *smi_full_name, char *smi_domain_name, char *smi_obj_name);
  static int findIndexByName(char *obj_name, int &index);
  static void cleanCurrentUiTree();

public:
  HbSmiBrowse(){
      prev_parent_name=string("");
      parent_name=string("");

      back_button=0;
      parent_button=0;

      smi_parent_object=0;
      objects_set=0;

      children_buttons.clear();
      children_object_names.clear();
      smi_children_objects.clear();
  }


};
//====================================================

#endif

