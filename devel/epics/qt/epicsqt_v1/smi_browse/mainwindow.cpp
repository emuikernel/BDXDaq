#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "smibrowse.h"
#include "graphbrowse.h"
#include <QPainter>

//#include "QCaLabelPlugin.h"
/**
QCaComboBox *ggg;

//HvDevice::HvChannel HvDevice::hvch[HvDevice::CHS_NUM];
HvDevice::HvChannel HvDevice::hvch[HvDevice::CHS_NUM];
HB_DeviceButton *HvDevice::current_board;
int HvDevice::chs_num;
//HB_DeviceButton *HvDevice::current_board_prev;
char *read_parameters[]={"F","E","L","pwonoff","v0set","i0set","trip","rampup","rampdn"}; // vmon, imon, status
char *params_labels[]={"vmon","imon","status","on/off","v0set","i0set","trip(s)","rampup","rampdn"}; // vmon, imon, status
/// char *read_parameters[]={"L","pwonoff","v0set","i0set","trip","rampup","rampdn"}; // vmon, imon, status
*/
MainWindow *mainw;
QPainter *mypaint;
GrBrowse *grbr=0;
HbSmiBrowse *smibrowse;
//QWidget *tree_holder;
//========================================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainw=this;
    //SmiObjectSet *objects_set=0;
    //objects_set = new SmiObjectSet ( "CLAS12::S_DETECTOR") ;
   // printf("hb debug %p 1\n", objects_set);fflush(stdout);
   // int n=0;
    //n=objects_set->getNObjects( );
   // char tmp[100];
///=============================================================================================
///setStyleSheet("background : red ;");
///

    QPalette pal_main = this->palette();
    pal_main.setColor(QPalette::Background , QColor(210,210,210));  /// needed to draw lines of the same color
    setPalette(pal_main);
    setAutoFillBackground(true);

    smibrowse =  new HbSmiBrowse();
    HbSmiBrowse::button_domain_name=string("CLAS12");
    HbSmiBrowse::button_obj_name_short=string("DETECTOR");
    HbSmiBrowse::button_alias_name=string("DETECTOR");
    smibrowse->findChildrenObjectsbyName(0/*"CLAS12", "DETECTOR"*/);
/*
    char *fsm_names[]={"b_hv000_0_p0", "b_hv000_2_p1"};
    //HvDevice hvdevice(fsm_names, 2);
    HvDevice *hvdevice=new HvDevice(fsm_names, 2);
    connect(ui->pushButton, SIGNAL(clicked()), hvdevice, SLOT(s_destructor()));
*/

    HbSmiBrowse::timer_update = new QTimer(this);
    connect(HbSmiBrowse::timer_update, SIGNAL(timeout()), smibrowse, SLOT(cb_update()));
    HbSmiBrowse::timer_update->start(HbSmiBrowse::rate_update*100); // ms

   // tree_holder = new QWidget(mainw);
#ifdef IS_DEMO_TREE
    grbr= new GrBrowse();
#endif

    ui->qcal_1->setVisible(false);
    ui->qcal_2->setVisible(false);
    ui->pushButton->setVisible(false);
    ui->qcacombo->setVisible(false);
    ui->qcapush->setVisible(false);
    ui->qcaspin->setVisible(false);
    ui->qcaedit->setVisible(false);
    ui->label23->setVisible(false);

   // ui->widget22->setStyleSheet
    //        ( " background : lightgreen; border: 2px solid red ; ");

}

//=================================================================
#ifdef IS_DEMO_TREE
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    mypaint=&painter;

    painter.setRenderHint(QPainter::Antialiasing, true);
    if(HbSmiBrowse::parent_update_need==1){
      grbr->draw_all_lines();
      HbSmiBrowse::parent_update_need--;
    }


 //printf("2 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& is_visible %d \n",GrBrowse::is_visible);fflush(stdout);
/*
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::black, 2));
    //QPen Red((QColor::QColor(255,0,0)),1);
    //mypaint->setPen(Red);

     printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& \n");fflush(stdout);
    mypaint->drawLine(50,50,250,250);
*/


}
#endif
//=================================================================
void MainWindow::s_update(){
    GrBrowse::is_visible = 0;
    update();
}
//=================================================================
MainWindow::~MainWindow()
{

    delete ui;
}
//=================================================================
