#include "smibrowse.h"
#include <smiuirtl.h>
#include "mainwindow.h"

//#include "smidevice.h"
#include "graphbrowse.h"
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

pthread_mutex_t obj_mutex;

///vector< MySmiObject * > HbSmiBrowse::smi_children_objects;
///MySmiObject *HbSmiBrowse::smi_parent_object;

vector< SmiQPushButton * >  HbSmiBrowse::children_buttons;
vector< SmiQLabel * >  HbSmiBrowse::children_labels;
vector< SmiQComboBox * >  HbSmiBrowse::children_combos;
SmiQPushButton *HbSmiBrowse::parent_button;
SmiQPushButton *HbSmiBrowse::back_button;

vector< string >  HbSmiBrowse::children_object_names;

string HbSmiBrowse::parent_name;
string HbSmiBrowse::parent_alias_name;
///SmiObjectSet *HbSmiBrowse::objects_set;

//----
int HbSmiBrowse::set_id;
int HbSmiBrowse::set_done;
vector< int > HbSmiBrowse::children_object_ids;
QTimer *HbSmiBrowse::timer_update;
int HbSmiBrowse::rate_update;
vector<int>  HbSmiBrowse::children_update_needs;
int HbSmiBrowse::parent_update_need;
int HbSmiBrowse::parent_id;
SmiQComboBox *HbSmiBrowse::parent_combo;
SmiQLabel *HbSmiBrowse::parent_label;

string HbSmiBrowse::button_domain_name;
string HbSmiBrowse::button_obj_name_short;
string HbSmiBrowse::button_alias_name;

int HbSmiBrowse::node_type; // smi folder or smi device
QLabel *HbSmiBrowse::path_label;

extern MainWindow *mainw;
extern GrBrowse *grbr;

//========================================================================================
void *HbSmiBrowse::mysmi_set_handler(){
//printf("2\n");fflush(stdout);
 char tmp[100];
 //char *tmp=0;
 printf("===\n"); fflush(stdout);
 while(smiui_get_next_object_in_set(HbSmiBrowse::set_id, tmp)) {
   printf("%s id=%d  %p\n", tmp, HbSmiBrowse::set_id , tmp); fflush(stdout);
  HbSmiBrowse::children_object_names.push_back(string(tmp));

  //setid=smiui_book_statechange(tmp, (void (*) (...))object_handler,0);
 }
 HbSmiBrowse::set_done=1;

 return 0;
}

//========================================================================================

void *HbSmiBrowse::mysmi_object_handler(int *id){
  //  printf("1\n");fflush(stdout);
 char tmp[100];
 char curr_state[100];
 int busy_flag,nac;
 //char *tmp=0;
 //while(smiui_get_next_object_in_set((*id), tmp)) {
 pthread_mutex_lock(&obj_mutex);
 smiui_get_name((*id),tmp);
 smiui_get_state((*id),&busy_flag,curr_state,&nac);
  printf("id=%d %s nactions=%d size=%d\n", *id, tmp, nac,children_buttons.size()); fflush(stdout);
  int i_f=-1;
  for(int i=0;i<(int)children_buttons.size();i++){
      if(children_object_ids[i]==(*id)){i_f=i;break;}
  }

  if(i_f != -1 && (*id)==parent_id){printf("alert print parent_id i_f=%d id=%d \n", i_f, (*id));fflush(stdout);exit(1);}

  if((*id)==parent_id){
  // parent_button->setText(tmp);
  // parent_label->setText(curr_state);
      parent_label->current_state=QString(curr_state);
  }
  else{
   if(i_f==-1){printf("alert print i_f=%d id=%d size=%d\n", i_f, (*id), children_buttons.size());fflush(stdout);exit(1);}
  // children_buttons[i_f]->setText(tmp);
 //  children_labels[i_f]->setText(curr_state);
   children_labels[i_f]->current_state=QString(curr_state);

  }


 if((*id)==parent_id){
     parent_combo->actions.clear();
     parent_update_need=2;
 }
 else {
     children_combos[i_f]->actions.clear();children_update_needs[i_f]=1;
     printf("++++++++++++++++ %s state=%s index=%d id=%d update=%d\n", tmp, curr_state, i_f, *id, children_update_needs[i_f]); fflush(stdout);
 }

  int npar;
  while (smiui_get_next_action((*id),tmp,&npar)) {

    if((*id)==parent_id){
     //   printf("tmp============================%s count=%d\n", tmp, parent_combo->count()); fflush(stdout);
      parent_combo->actions.push_back(QString(tmp));
      ///parent_update_need=1;
    }
    else{
    //  printf("++++++++++++++++tmp=%s count=%d\n", tmp, children_combos[i_f]->count()); fflush(stdout);
      children_combos[i_f]->actions.push_back(QString(tmp));
      ///children_update_needs[i_f]=1;
    }
  }

  set_done=1;
  pthread_mutex_unlock(&obj_mutex);
  return 0;

}

//===================================================================
//void MySmiObjectSet::smiObjSetChangeHandler ( ){
//
//}

//===================================================================
void HbSmiBrowse::cleanCurrentUiTree(){

  if(parent_button){

    delete parent_button;
    delete back_button;
    delete parent_combo;
    delete parent_label;

    smiui_cancel_statechange(parent_id);

    smiui_cancel_objectsetchange(set_id);

    for(int i=0; i< (int) children_object_names.size();i++){
      if(children_buttons[i]){
        smiui_cancel_statechange(children_object_ids[i]);

        delete children_buttons[i];
        delete children_labels[i];
        delete children_combos[i];
      }
      children_buttons[i]=0;
    }
    children_update_needs.clear();
    children_buttons.clear();
    children_object_names.clear();
    children_labels.clear();
    children_combos.clear();
    children_object_ids.clear();

    if(node_type==1)delete MainWindow::hvcrate;

  }
}
//===================================================================
void SmiQComboBox::s_to_send_command_for_this_smi_object(int index){

printf("send\n");fflush(stdout);
  QString qs=itemText(index);//currentText();
  char cmnd[81]; // temporal
  strcpy(cmnd,qs.toAscii());
   printf("============================%s %s\n", (char *)obj_name.c_str(), cmnd); fflush(stdout);

  smiui_send_command((char *)obj_name.c_str(), cmnd);

}




//===================================================================
void HbSmiBrowse::cb_update(){
  //  printf("2\n");fflush(stdout);
pthread_mutex_lock(&obj_mutex);
 for(int i=0;i<(int)children_buttons.size();i++){
//printf(" ******************   CB_UPDATE  ***************** index=%i  id=%d update=%d\n",i, children_object_ids[i], children_update_needs[i]); fflush(stdout);
     if(children_update_needs[i]){
printf(" **********************CB_UPDATE********************** index=%i  id=%d \n",i, children_object_ids[i]); fflush(stdout);
      children_buttons[i]->setText(children_buttons[i]->alias_name.c_str());
      children_labels[i]->setText(children_labels[i]->current_state);
      if(children_labels[i]->current_state.indexOf("on",0,Qt::CaseInsensitive)!=-1)
      children_labels[i]->setStyleSheet("QLabel { background : green; border: 1px solid black ; color : black; }");
      else if(children_labels[i]->current_state.indexOf("off",0,Qt::CaseInsensitive)!=-1)
      children_labels[i]->setStyleSheet("QLabel { background : blue; border: 1px solid black ; color : white; }");
      else if(children_labels[i]->current_state.indexOf("error",0,Qt::CaseInsensitive)!=-1)
      children_labels[i]->setStyleSheet("QLabel { background : red; border: 1px solid black ; color : white; }");


      children_combos[i]->clear();
      for(int j=0;j<(int)children_combos[i]->actions.size();j++){
          if(children_combos[i]->actions[j].indexOf("SET_",0) !=0)
          children_combos[i]->insertItem(j,children_combos[i]->actions[j]);
      }
      children_update_needs[i]=0;
      children_combos[i]->actions.clear();
     }
 }
#ifdef IS_DEMO_TREE
 if(parent_update_need==2){
   //  printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& is_visible %d \n",1);fflush(stdout);

     mainw->update();  /// to updade lines painting.
                       /// The update will do at the parent_update_need=1 as it happens at the next lcycle of the loop
#else
  if(parent_update_need){
#endif

     //   printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& is_visible %d \n",2);fflush(stdout);

     parent_button->setText(parent_button->alias_name.c_str());

     parent_label->setText(parent_label->current_state);
     if(parent_label->current_state.indexOf("on",0,Qt::CaseInsensitive)!=-1){
     parent_label->setStyleSheet("QLabel { background : green; border: 1px solid black ; color : black; }");
  //    parent_label->setStyleSheet("QLabel { background-color : green; color : black; }");

     }
     else if(parent_label->current_state.indexOf("off",0,Qt::CaseInsensitive)!=-1){
      parent_label->setStyleSheet("QLabel { background : blue; border: 1px solid black ; color : white; }");
  //parent_label->setStyleSheet("QLabel { background : blue; border: 1px solid  #ff0000 ; color : white; }");
     }
     else if(parent_label->current_state.indexOf("error",0,Qt::CaseInsensitive)!=-1){
      parent_label->setStyleSheet("QLabel { background : red; border: 1px solid black ; color : white; }");
  //parent_label->setStyleSheet("QLabel { background : blue; border: 1px solid  #ff0000 ; color : white; }");
     }
     parent_combo->clear();
     for(int j=0;j<(int)parent_combo->actions.size();j++){
         if(parent_combo->actions[j].indexOf("SET_",0) !=0)
         parent_combo->insertItem(j,parent_combo->actions[j]);
     }
    parent_combo->actions.clear();
#ifdef IS_DEMO_TREE
    parent_update_need--;
#else
    parent_update_need=0;
#endif
 }
 //else if(parent_update_need==1){/*parent_update_need--;*/
pthread_mutex_unlock(&obj_mutex);
 return;
}

//===================================================================
int HbSmiBrowse::findChildrenObjectsbyName(int mode/*char *domain_name, char *obj_name*/){

   // if(grbr){
     GrBrowse::shown_smi_nodes.clear();
     GrBrowse::is_visible = (!mode);
   // }
    char domain_name[81]; char obj_name[81]; char obj_alias_name[81]; // temporal
    strcpy(domain_name, button_domain_name.c_str());
    strcpy(obj_name, button_obj_name_short.c_str());
    strcpy(obj_alias_name, button_alias_name.c_str());

    static int c1=0;
  //  printf(" ********************************************** %d\n",c1); fflush(stdout);
  //  c1++;
  //  printf("===+++===%s  \n", domain_name);fflush(stdout);

     int id;
     printf("hb debug 00\n");fflush(stdout);
//printf("===++++===%s  \n", domain_name);fflush(stdout);
    cleanCurrentUiTree();
    node_type=mode;
//printf("hb debug 01\n");fflush(stdout);
//printf("===+++++===%s  \n", domain_name);fflush(stdout);
//-------


//------
printf("hb debug 02\n");fflush(stdout);

//-----------------------------  back browsing -------------------------------------------------

 bool is_back_click=false;
 string s=string(obj_name);
 string s1=parent_alias_name;//string(obj_alias_name);
    if(strstr(obj_name, ">>")){ /// means the back_button clicked
        is_back_click=true;
        //string p;
        int found=s.rfind(">>");
        strcpy(obj_name,(s.substr(found+2)).c_str());
        parent_name=s.substr(0, found);

        found=s1.rfind(">>");
        //strcpy(obj_alias_name,(s1.substr(found+2)).c_str());
        parent_alias_name=s1.substr(0, found);

      //  if(parent_name=="")back_button->setVisible(false);
//        printf("===++++============================================1 =%s =%s\n", parent_name.c_str(), s.c_str());fflush(stdout);
    }
// printf("1===++++============================================2 =%s =%s \n", parent_name.c_str(),  s.c_str());fflush(stdout);
    back_button=new SmiQPushButton(mainw, domain_name, (char *) parent_name.c_str());

    //printf("hb debug 011\n");fflush(stdout);
    back_button->setGeometry(QRect(GX_BACK, GY_BACK, GW_BACK, GH_BACK));
    if(parent_name=="")back_button->setVisible(false);
    else back_button->setVisible(true);
    back_button->setText("BACK");

// printf("2===++++============================================2 =%s =%s \n", parent_name.c_str(),  s.c_str());fflush(stdout);

  ///  if(!strstr(obj_name, ">>")){
     parent_name=parent_name+">>"+string(obj_name);
     if(!is_back_click)parent_alias_name=parent_alias_name+">>"+string(obj_alias_name);
//        printf("===++++============================================2 =%s =%s \n", parent_name.c_str(),  s.c_str());fflush(stdout);

  ///  }

  //   printf("3===++++============================================2 =%s =%s \n", parent_name.c_str(),  s.c_str());fflush(stdout);
//---------------------------------  end of back browsing -----------------------------------------------------

   if(path_label==0){
     path_label= new QLabel(mainw);
     path_label->setGeometry(QRect(GX_BASE, GY_PATH,GW_PATH, GH_NODE));
     path_label->setStyleSheet("QLabel { background : lightgreen; border: 1px solid black ; color : black; }");
     path_label->setFrameShape(QFrame::Box);
   }
  // path_label->setText(parent_name.c_str());

   path_label->setText(parent_alias_name.c_str());
//-------
  /// smi_parent_object= new MySmiObject(domain_name, obj_name);
//------------------------------------------------------------------------
 //   printf("===++++===%s  \n", domain_name);fflush(stdout);
    parent_button=new SmiQPushButton(mainw, domain_name, obj_name);
    grbr->shown_smi_nodes.push_back(parent_button->obj_name_short);
//   printf("===++++===%s  \n", domain_name);fflush(stdout);
//printf("===%s \n", parent_button->domain_name);fflush(stdout);
    parent_button->setGeometry(QRect(GX_PARENT, GY_PARENT, GW_PARENT_BUTTON, GH_NODE));

    parent_button->setStyleSheet("background : #292929 ; border: 1px solid black ; color : white; ");
   /*
    QPalette pal_pb = parent_button->palette();
    pal_pb.setColor(QPalette::Button, QColor(121,121,121));
    pal_pb.setColor(QPalette::ButtonText, Qt::white);
    parent_button->setPalette(pal_pb);
    parent_button->setAutoFillBackground(true);
   */
    parent_button->setVisible(true);
///    mainw->connect(parent_button, SIGNAL(clicked()),
///            parent_button, SLOT(s_to_browse_this_smi_object()));
//
    mainw->connect(back_button, SIGNAL(clicked()),
            back_button, SLOT(s_to_browse_this_smi_object()));

   // parent_combo=new SmiQComboBox(mainw, domain_name, (char*) parent_name.c_str());
   parent_combo=new SmiQComboBox(mainw, domain_name, (char*) obj_name);
    parent_combo->setGeometry(QRect(GX_PARENT+ GW_PARENT_BUTTON+GGAP_SMALL, GY_PARENT, GW_PARENT_COMBO, GH_NODE));
    parent_combo->setStyleSheet(" background : #292929;  color : white; ");
parent_combo->setVisible(true);
//parent_label=new SmiQLabel(mainw, domain_name, (char*) parent_name.c_str());
    parent_label=new SmiQLabel(mainw, domain_name, (char*) obj_name);
     parent_label->setStyleSheet("QLabel { qproperty-alignment: AlignCenter;}");

     parent_label->setGeometry(QRect(GX_PARENT+ GW_PARENT_BUTTON+GGAP_SMALL, GY_PARENT, GW_PARENT_LABEL, GH_NODE));
    parent_label->setFrameShape(QFrame::Box);
 parent_label->setVisible(true);
//------------------------------------------------------------------------


    char tmp[81]; // temporal
    strcpy(tmp, domain_name);
    strcat(tmp, "::");
    strcat(tmp,"S_");
    strcat(tmp,obj_name);

//   printf("===%s %s \n", domain_name, parent_button->domain_name.c_str());fflush(stdout);
//
   /// objects_set = new SmiObjectSet ( domain_name, tmp) ;
   set_done=0;
   set_id=smiui_book_objectsetchange(tmp, (void (*) (...))mysmi_set_handler,0);
   while(!set_done);

//printf("hb debug %s %s 1\n",domain_name, tmp);fflush(stdout);
   /// while(1){
printf("hb debug 11\n");fflush(stdout);
    /// SmiObject* smio=objects_set->getNextObject( );
printf("hb debug 2\n");fflush(stdout);
    /// if(!smio)break;
    /// children_object_names.push_back(string(smio->getName()));
    // MySmiObject *s= new MySmiObject (*((MySmiObject *)smio));
    //if(smio)smi_children_objects.push_back(smio);
   /// }

  ///  delete objects_set; // my: presumably to delete all ui objects created under objects_set
  ///                      // because we need are own which are monitored via handlers

strcpy(tmp, domain_name);
strcat(tmp, "::");
strcat(tmp,obj_name);
//--------------------------------------------------------------------------------------------------------------------
    set_done=0;
    pthread_mutex_lock(&obj_mutex);
    id=smiui_book_statechange(tmp, (void (*) (...))mysmi_object_handler,0);
    parent_id=id;
printf("id+++p %d size=%d\n",id,children_object_ids.size());fflush(stdout);
    parent_update_need=2;
    pthread_mutex_unlock(&obj_mutex);
    while(!set_done);
//--------------------------------------------------------------------------------------------------------------------
/** comment out to speed up graphics

    for(int i=0;i<(int)parent_combo->actions.size();i++){
        if(parent_combo->actions[i].indexOf("SET_",0) !=0)
        parent_combo->insertItem(i,parent_combo->actions[i]);
  //  children_combos[children_combos.size()-1]->insertItem(0,"hh");
    }
*/
    mainw->connect(parent_combo, SIGNAL(activated(int)),
       parent_combo, SLOT(s_to_send_command_for_this_smi_object(int)));

//--------------------------------------------------------------------------------------------------------------------
     if(mode==1)return 0;
    for(int i=0; i< (int) children_object_names.size();i++){

      //  strcpy(tmp, domain_name);
      //  strcat(tmp, "::");
        strcpy(tmp, children_object_names[i].c_str());


        printf("s====%s\n",children_object_names[i].c_str());fflush(stdout);

        ///   smi_children_objects.push_back
        ///           (new MySmiObject (domain_name, (char*) children_object_names[i].c_str()));
//--------------------------------------------------------------------------------------------------
        children_buttons.push_back
                (new SmiQPushButton(mainw, domain_name, (char*) children_object_names[i].c_str()));
        grbr->shown_smi_nodes.push_back((children_buttons[children_buttons.size()-1])->obj_name_short);
        (children_buttons[children_buttons.size()-1])->setGeometry(QRect(GX_CHILD, GY_CHILD+(i+1)*GH_NODE, GW_CHILD_BUTTON, GH_NODE));
        (children_buttons[children_buttons.size()-1])->setStyleSheet("background : #797979 ; border: 1px solid black ; color : white; ");
        (children_buttons[children_buttons.size()-1])->setVisible(true);

//        mainw->connect(children_buttons[children_buttons.size()-1], SIGNAL(clicked()),
//                mainw, SLOT(s_update()));
        mainw->connect(children_buttons[children_buttons.size()-1], SIGNAL(clicked()),
                children_buttons[children_buttons.size()-1], SLOT(s_to_browse_this_smi_object()));


        children_combos.push_back
                (new SmiQComboBox(mainw, domain_name, (char*) children_object_names[i].c_str()));
        (children_combos[children_combos.size()-1])->setGeometry
                (QRect(GX_CHILD+GW_CHILD_BUTTON+GGAP_SMALL, GY_CHILD+(i+1)*GH_NODE, GW_CHILD_COMBO, GH_NODE));
        (children_combos[children_combos.size()-1])->setStyleSheet(" background : #898989;  color : black; ");
        (children_combos[children_combos.size()-1])->setVisible(true);

        children_labels.push_back
                (new SmiQLabel(mainw, domain_name, (char*) children_object_names[i].c_str()));
        (children_labels[children_labels.size()-1])->setGeometry
                (QRect(GX_CHILD+GW_CHILD_BUTTON+GGAP_SMALL, GY_CHILD+(i+1)*GH_NODE, GW_CHILD_LABEL, GH_NODE));
        (children_labels[children_labels.size()-1])->setFrameShape(QFrame::Box);
        (children_labels[children_labels.size()-1])->setVisible(true);
//(children_labels[children_labels.size()-1])->setStyleSheet("QLabel { background-color : red; color : blue; }");
//--------------------------------------------------------------------------------------------------
        set_done=0;
        pthread_mutex_lock(&obj_mutex);
        id=smiui_book_statechange(tmp, (void (*) (...))mysmi_object_handler,0);

        children_object_ids.push_back(id);
        printf("id+++c %d size=%d\n",id,children_object_ids.size());fflush(stdout);
        children_update_needs.push_back(1);
        pthread_mutex_unlock(&obj_mutex);
     //   parent_update_need=1;

        while(!set_done);
//--------------------------------------------------------------------------------------------------
         children_labels[children_labels.size()-1]->setStyleSheet("QLabel { qproperty-alignment: AlignCenter;  }");
/** comment out to speed up graphics

        for(int i=0;i<(int)children_combos[children_combos.size()-1]->actions.size();i++){
            if(children_combos[children_combos.size()-1]->actions[i].indexOf("SET_",0) !=0)
            children_combos[children_combos.size()-1]->insertItem(i,children_combos[children_combos.size()-1]->actions[i]);
      //  children_combos[children_combos.size()-1]->insertItem(0,"hh");
        }
*/
        mainw->connect(children_combos[children_combos.size()-1], SIGNAL(/*currentIndexChanged*/activated(int)),
           children_combos[children_combos.size()-1], SLOT(s_to_send_command_for_this_smi_object(int)));


    }

/*
  char *smi_domain_name, *smi_obj_name;
   splitDomainAnsObjName(smi_full_name, smi_domain_name, smi_obj_name);
   char tmp[81]; // temporal
   strcpy(tmp,smi_domain_name);
   strcat(tmp,"::");
   strcat(tmp,"S_");
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

int SmiQWidget::splitDomainAndObjName(char *smi_full_name/*, char *smi_domain_name, char *smi_obj_name*/){

    char *tmp1;

     tmp1=strstr(smi_full_name,"::");
     if(!tmp1){
        // printf("++++++++++++++++++++++++++++++++ %s\n", smi_full_name); fflush(stdout);
         //strcpy(smi_full_name, smi_full_name);
         //smi_obj_name=smi_full_name;
         return 0;

     }
   //  smi_domain_name=smi_full_name;
   //  *(smi_domain_name+strlen(smi_full_name)-strlen(tmp1))=0;

     strcpy(smi_full_name, tmp1+2);
 //printf("++++++++++++++++++++++++++++++++ %s\n", smi_obj_name); fflush(stdout);
  return 0;
}
//===================================================================
//extern char *board_names_by_crate[3][13];
//extern int board_types_by_crate[3][13];
//===================================================================
void SmiQPushButton::s_to_browse_this_smi_object(){
    /// attention: this function is called for the object (SmiQPushButton) which is deleted in the findChildrenObjectsbyName

    int ret;
      //printf("===+++%s  \n", domain_name.c_str());fflush(stdout);
    HbSmiBrowse::button_domain_name=domain_name;
    HbSmiBrowse::button_obj_name_short=obj_name_short;
    HbSmiBrowse::button_alias_name=alias_name;
    //if(!strncasecmp("B_",(char *)obj_name_short.c_str(),2)){
    if(is_device){
      //  GrBrowse::is_visible = 0;
     //   mainw->update();
      ///  int crate_index=0;
      ///  int crate_type=CrateButton::CAEN_1527;
      ///  int size=sizeof(board_names_by_crate[crate_index])/sizeof(board_names_by_crate[crate_index][0]);
     /// printf("ggg %d\n", size);fflush(stdout);

        HvCrate::board_types_by_crate.clear();
        HvCrate::board_types_by_crate.push_back(HvCrate::CAEN_A1535);
        HvCrate::board_names_by_crate.clear();
        HvCrate::board_names_by_crate.push_back(obj_name_short);

        MainWindow::hvcrate=new HvCrate(/*board_types_by_crate[crate_index], board_names_by_crate[crate_index],*/
                    1/*size*/, CrateButton::CAEN_1527);
        MainWindow::constructed=1;

     ///   MainWindow::current_crate=this;
     ret= HbSmiBrowse::findChildrenObjectsbyName(1/*(char *)domain_name.c_str(), (char *)obj_name_short.c_str()*/);
    }
    else{
     ret= HbSmiBrowse::findChildrenObjectsbyName(0/*(char *)domain_name.c_str(), (char *)obj_name_short.c_str()*/);
    }
    printf("===+++++++++++++++++++++++++++++++++++++++++++++++++++++  \n");fflush(stdout);

}
//===================================================================
