#include "smibrowse.h"
#include <smiuirtl.h>
#include "mainwindow.h"


/**

  The idea of this software:
  a) this software uses smiui
  b) for this reason, it does not affect the smi processing itself
  c) for example we need to intercept with our handler changes of states
     of all objects in the object set. What we do:
  d) we create the object set object
  e) as this class (SmiObjectSet) has a function getNextObject that means
     that the creation of objectset object also create all its objects
  f) but those object that we can derive with getNextObject are of SmiObject
     class, so they can not be intercepted with smiStateChangeHandler
  g) So, We derive all objectset objects with getNextObject to know their SMI names.
     We delete the objectset, that presumably means deletion of all it smiui objects.
     We re-create all derived objects with MySmiObject class that allows to intercept theit changes.

     Valeri Sytnik.
     2013/06/29

 */


vector< MySmiObject * > HbSmiBrowse::smi_children_objects;
MySmiObject *HbSmiBrowse::smi_parent_object;

vector< SmiQPushButton * >  HbSmiBrowse::children_buttons;
SmiQPushButton *HbSmiBrowse::parent_button;
SmiQPushButton *HbSmiBrowse::back_button;

vector< string >  HbSmiBrowse::children_object_names;

string HbSmiBrowse::parent_name;

SmiObjectSet *HbSmiBrowse::objects_set;
extern MainWindow *mainw;

//===================================================================
void MySmiObject::smiStateChangeHandler ( ){

    char *name, *state;

    name = getName();
    state = getState();
    int index;
    if(name)
    {
        HbSmiBrowse::findIndexByName(name, index);
        if(state){
          (HbSmiBrowse::children_buttons[index])->setText(state);
        }
        //  cout << name << " in state " << getState() << endl;
        else{
          (HbSmiBrowse::children_buttons[index])->setText("DEAD");
        }
        //  cout << name << " is dead" << endl;
    }
}
//===================================================================
//void MySmiObjectSet::smiObjSetChangeHandler ( ){
//
//}

//===================================================================
void HbSmiBrowse::cleanCurrentUiTree(){

  if(parent_button){
    delete smi_parent_object;
    delete parent_button;
    delete back_button;

    for(int i=0; i< (int) children_object_names.size();i++){
        delete smi_children_objects[i];
        delete children_buttons[i];
    }

    children_buttons.clear();
    children_object_names.clear();
    smi_children_objects.clear();
  }
}

//===================================================================
int HbSmiBrowse::findChildrenObjectsbyName(char *domain_name, char *obj_name){

    cleanCurrentUiTree();



    back_button=new SmiQPushButton(mainw, domain_name, (char *) parent_name.c_str());
    parent_button->setGeometry(QRect(70, 100, 110, 30));
    if(parent_name=="")parent_button->setVisible(false);

    parent_name=string(obj_name);
    smi_parent_object= new MySmiObject(domain_name, obj_name);

    parent_button=new SmiQPushButton(mainw, domain_name, obj_name);
    parent_button->setGeometry(QRect(70, 20, 110, 30));
    char tmp[81]="s_"; // temporal
    strcat(tmp,obj_name);

    objects_set = new SmiObjectSet ( domain_name, obj_name) ;

    while(1){
     SmiObject* smio=objects_set->getNextObject( );
     if(!smio)break;
     children_object_names.push_back(string(smio->getName()));
    // MySmiObject *s= new MySmiObject (*((MySmiObject *)smio));
    //if(smio)smi_children_objects.push_back(smio);
    }

    delete objects_set; // my: presumably to delete all ui objects created under objects_set
                        // because we need are own which are monitored via handlers

    for(int i=0; i< (int) children_object_names.size();i++){
        smi_children_objects.push_back
                (new MySmiObject (domain_name, (char*) children_object_names[i].c_str()));
        children_buttons.push_back
                (new SmiQPushButton(mainw, domain_name, (char*) children_object_names[i].c_str()));
        (children_buttons[children_buttons.size()-1])->setGeometry(QRect(200, 20+(i+1)*20, 60, 20));
        mainw->connect(children_buttons[children_buttons.size()-1], SIGNAL(clicked()),
                children_buttons[children_buttons.size()-1], SLOT(s_to_browse_this_smi_object()));
    }
/*
  char *smi_domain_name, *smi_obj_name;
   splitDomainAnsObjName(smi_full_name, smi_domain_name, smi_obj_name);
   char tmp[81]; // temporal
   strcpy(tmp,smi_domain_name);
   strcat(tmp,"::");
   strcat(tmp,"s_");
   strcat(tmp,smi_obj_name);
   long param=0;
   int ret= smiui_book_objectsetchange(tmp, handler_set, param);//param);
*/
    //splitDomainAnsObjName(smi_full_name, smi_domain_name, smi_obj_name);


return 0;

}
//===================================================================
int HbSmiBrowse::findIndexByName(char *obj_name, int &index){

    for(int i=0; i< (int) children_object_names.size();i++){
        if(!strcmp(children_object_names[i].c_str(), obj_name )){
          index=i;
          return 0;
        }
    }
    return -1;
}
//===================================================================
int HbSmiBrowse::splitDomainAnsObjName(char *smi_full_name, char *smi_domain_name, char *smi_obj_name){


  return 0;
}
//===================================================================
void SmiQPushButton::s_to_browse_this_smi_object(){

    int ret = HbSmiBrowse::findChildrenObjectsbyName((char *)domain_name.c_str(), (char *)obj_name.c_str());

}
//===================================================================
