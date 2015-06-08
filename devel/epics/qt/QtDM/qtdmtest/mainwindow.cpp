#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "ui_motor_expert.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->catextentry->setVisible(true);
  //  ui->catextentry->setChannelA(QApplication::translate("MainWindow", "claspcal_m1.VAL", 0, QApplication::UnicodeUTF8));
//   ui->catextentry->setPV("claspcal_m1.VAL");
//    ui->calineedit->setPV("claspcal_m1.RMP");
    ui->catextentry->setPV("B_HV000_8_13_rampup");
    ui->calineedit->setPV("B_HV000_8_13_rampup");
    printf("1\n"); fflush(stdout);
//B_HV000_8_13_rampup
}

MainWindow::~MainWindow()
{
    delete ui;
}
