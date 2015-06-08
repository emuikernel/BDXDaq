#ifndef SMIDEVICE_H
#define SMIDEVICE_H

#include <QMainWindow>
#include "QCaLabelPlugin.h"
#include <QtGui/QPushButton>
#include "QCaComboBox.h"
#include "QCaLineEdit.h"
#include <vector>
#include <string>
#include <QtGui/QFileDialog>
#include <QTimer>
using namespace std;

class BoardButton;



class HvCrate;
class CrateButton;

//=================================================================
class MyQCaLabel : public QCaLabel
{

   Q_OBJECT

public:

    MyQCaLabel(QString &name, QWidget *w, int param) : QCaLabel(name, w){
       parameter=param;
    }
private:
  int parameter;
  void colorControl(QString &qs, QPalette &pal);
private slots:
    void s_dbValueChanged(QString str);//{printf("1\n");fflush(stdout);setText("hhh");};
};
//=================================================================
class CrateButton : public QPushButton
{

   Q_OBJECT

 public:

    enum CRATE_TYPES{
      CAEN_527,
      CAEN_1527,
      CAEN_2527,
      WMPOD

    };


  CrateButton(QWidget *widget, int index, int type) :  QPushButton(widget){
     crate_index=index;
     crate_type=type;
  }

 private:
    int crate_index;
    int crate_type;
 //  void construct();
  // void destruct();
 private slots:
    void s_crateDisplay();

};
//=================================================================
// I did this class for two rreasons:
// a) to emit signals (signal is protected method)
// b) to intercept dbCalueChanged
class MyQCaComboBox : public QCaComboBox
{
     Q_OBJECT

   /// friend class BoardButton;
    friend class HvCrate;

public:
    MyQCaComboBox(const QString &qs, QWidget *parent, int param) : QCaComboBox(qs, parent){
     parameter=param;
    }
private:
  int parameter;


private slots:
 void s_dbValueChanged(qlonglong out);
};

//=================================================================
/*
class HvCrate : public QObject // e.g. HV devive of chamber
{
    Q_OBJECT

    int boards_number;
public:

    const static int TABLE_XBASE=200;
    const static int TABLE_YBASE=50+24+20;

    static int constructed;
    enum BOARD_TYPES{
      CAEN_A1535, CAEN_A944, WMPOD_HV

    };


    HvCrate( int number, int type) ;
    virtual ~HvCrate();
    const static int PARS_READ=3;
    const static int PARS_SWITCH=1;
    const static int PARS_SET=5;
    const static int PARS_NUM=PARS_READ+PARS_SWITCH+PARS_SET;
    const static int CHS_NUM=24;
    const static int MAX_DEVICES_PER_LOGIC_UNIT=16;
    static QLabel *label_hvpar[PARS_NUM];

    static int crate_type;

    static int params_read;
    static int params_switch;
    static int params_set;
    static int params_num;
    static int channels_num;
    static int first_channel;

    struct HvChannel{
       QCaLabel *hvpar_read[PARS_READ];
       MyQCaComboBox *hvpar_switch[PARS_SWITCH];
       QCaLineEdit *hvpar_set[PARS_SET];

       int hb_isactivated[PARS_NUM];

       //int hb_isHidden[PARS_NUM];
    };

   static BoardButton *current_board;
  // static BoardButton *current_board_prev;


   static HvChannel hvch[CHS_NUM];
   static QPushButton *bturnonallchannels;
   static QPushButton *bturnoffallchannels;
   static QPushButton *bstore;
   static QPushButton *bload;
   BoardButton *hvb[MAX_DEVICES_PER_LOGIC_UNIT];

   static vector<string> board_names_by_crate;
   static vector<int> board_types_by_crate;

private slots:
    void s_destructor();
    void s_turnonallchannels();
    void s_turnoffallchannels();
    void s_store();
    void s_load();
};
*/

//=================================================================

class HvCrate : public QObject /// smi:cpy and modified // e.g. HV devive of chamber
{
    Q_OBJECT

    int boards_number;



    void load_db(int br, int ch, int i, vector<string> &ps );
    BoardButton *db_board_finding(int br);



public:
    const static int TABLE_XBASE=200; /// smi: modified
    const static int TABLE_YBASE=50+24+20; /// smi: modified

//    const static int TABLE_XBASE=200;
//    const static int TABLE_YBASE=40;//50+24+20; // smi++ is different
    const static int X_BOARDS=155;
    int need_db_store;
    int need_db_load;
    int db_under_processing;
    QLabel *warning_db;
    static int constructed;
    enum BOARD_TYPES{
      CAEN_A1535, CAEN_A944, WMPOD_HV

    };

    HvCrate(/*int *types, char **fsm_names,*/ int number, int type) ; //smi:
    virtual ~HvCrate();

    //void *cb_thread_config(void *arg);
    void s_store_process(char *store_file);
    void s_load_process(char *load_file);
    void activate_after_config();
    void s_crateonoff(int ison);

    const static int PARS_READ=3;
    const static int PARS_SWITCH=1;
    const static int PARS_SET=5;
    const static int PARS_NUM=PARS_READ+PARS_SWITCH+PARS_SET;
    const static int CHS_NUM=24;
    const static int MAX_DEVICES_PER_LOGIC_UNIT=16;
    static QLabel *label_hvpar[PARS_NUM];
    static QLabel *label_chnumber_title;

    static int crate_type;

    static int params_read;
    static int params_switch;
    static int params_set;
    static int params_num;
    static int channels_num;
    static int first_channel;

    struct HvChannel{
       QCaLabel *hvpar_read[PARS_READ];
       MyQCaComboBox *hvpar_switch[PARS_SWITCH];
       QCaLineEdit *hvpar_set[PARS_SET];
       QLabel *label_chnumber;

       int hb_isactivated[PARS_NUM];

       //int hb_isHidden[PARS_NUM];
    };

   static BoardButton *current_board;
  // static BoardButton *current_board_prev;
   static QTimer *timer_db;
   const static int rate_update_db=1;

   static HvChannel hvch[CHS_NUM];
   static QPushButton *bturnonallchannels;
   static QPushButton *bturnoffallchannels;
   static QPushButton *bcrateon;
   static QPushButton *bcrateoff;
   static QPushButton *bstore;
   static QPushButton *bload;
   BoardButton *hvb[MAX_DEVICES_PER_LOGIC_UNIT];

   static vector<string> board_names_by_crate; /// smi: modified
   static vector<int> board_types_by_crate; /// smi: modified


private slots:
    void s_destructor();
    void s_turnonallchannels();
    void s_turnoffallchannels();
    void s_store();
    void s_load();
    void cb_update_db();

    void s_crateon();
    void s_crateoff();

};

/*
//=================================================================
class HvCaenDevice : public HvCrate // e.g. HV devive of chamber
{

};
*/
//=================================================================
class BoardButton : public QPushButton
{

   Q_OBJECT

 public:
 QString epics_name;

    HvCrate::BOARD_TYPES board_type;
    int crate, board, board_part;
    vector<string> onoff_records;
    char **epics_siffixes_parameters;
    int BOARD_CHS_NUM;
    void destruct();

  BoardButton(QWidget *widget, char *ename) :  QPushButton(widget){
     //char ename1[81]=// temporal
     boardNameParser(ename);
     epics_name=QString(ename);

    }
    void boardConnect();
 static int boardNameParserStatic(char *epics_name, int &crate, int &board, int &board_part);
 private:
   //QString epics_name;


   int boardNameParser(char *epics_name);

   void construct();
 private slots:
    void s_boardDisplay();

};


//=================================================================
class BoardButtonCaenA1535 : public BoardButton
{


public:
    BoardButtonCaenA1535(QWidget *widget, char *ename);


};
//=================================================================
class BoardButtonCaenA944 : public BoardButton
{


public:
    BoardButtonCaenA944(QWidget *widget, char *ename);


};
//=================================================================
class BoardButtonWMpod : public BoardButton
{


public:
    BoardButtonWMpod(QWidget *widget, char *ename);


};
#endif
