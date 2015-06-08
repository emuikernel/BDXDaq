#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QCaLabelPlugin.h"
#include <QtGui/QPushButton>
#include "QCaComboBox.h"
#include "QCaLineEdit.h"

#include "smidevice.h"

#define IS_DEMO_TREE

class CrateButton;

namespace Ui {
class MainWindow;

}
//========================================================================
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
#ifdef IS_DEMO_TREE
    void paintEvent(QPaintEvent *);
#endif
/*
    const static int PARS_READ=3;
    const static int PARS_SWITCH=1;
    const static int PARS_SET=5;
    const static int PARS_NUM=PARS_READ+PARS_SWITCH+PARS_SET;
    const static int CHS_NUM=24;
    const static int MAX_DEVICES_PER_LOGIC_UNIT=16;
    QLabel *label_hvpar[PARS_NUM];

    struct HvChannel{
       QCaLabel *hvpar_read[PARS_READ];
       QCaComboBox *hvpar_switch[PARS_SWITCH];
       QCaLineEdit *hvpar_set[PARS_SET];


       int hb_isactivated[PARS_NUM];
       //int hb_isHidden[PARS_NUM];
    };

   static HvChannel hvch[CHS_NUM];
   BoardButton *hvb[MAX_DEVICES_PER_LOGIC_UNIT];
   */
    static int constructed;
    static CrateButton *current_crate;
    static HvCrate *hvcrate;
    static QPushButton *bback;



private:

    char **crate_names;
    int crates_number;
    CrateButton *hvcrates[20]; // temporal
    Ui::MainWindow *ui;
private slots:
    void s_update();
  //  void s_destructor();
  //  void s_back();
  //  void s_crateListDelete();
};


#endif // MAINWINDOW_H
