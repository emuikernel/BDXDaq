/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Oct 17 19:53:35 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
#include "QCaComboBoxPlugin.h"
#include "QCaLabelPlugin.h"
#include "QCaLineEditPlugin.h"
#include "QCaPushButtonPlugin.h"
#include "QCaSpinBoxPlugin.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QCaLabelPlugin *qcal_1;
    QCaLabelPlugin *qcal_2;
    QPushButton *pushButton;
    QCaPushButtonPlugin *qcapush;
    QCaSpinBoxPlugin *qcaspin;
    QCaLineEditPlugin *qcaedit;
    QCaComboBoxPlugin *qcacombo;
    QLabel *label23;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(907, 639);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        qcal_1 = new QCaLabelPlugin(centralWidget);
        qcal_1->setObjectName(QString::fromUtf8("qcal_1"));
        qcal_1->setGeometry(QRect(140, 120, 57, 15));
        qcal_1->setFrameShape(QFrame::Box);
        qcal_1->setProperty("visible", QVariant(false));
        qcal_2 = new QCaLabelPlugin(centralWidget);
        qcal_2->setObjectName(QString::fromUtf8("qcal_2"));
        qcal_2->setGeometry(QRect(210, 120, 57, 15));
        qcal_2->setFrameShape(QFrame::Box);
        qcal_2->setProperty("visible", QVariant(false));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(60, 420, 121, 26));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        pushButton->setFont(font);
        qcapush = new QCaPushButtonPlugin(centralWidget);
        qcapush->setObjectName(QString::fromUtf8("qcapush"));
        qcapush->setGeometry(QRect(130, 250, 100, 26));
        qcaspin = new QCaSpinBoxPlugin(centralWidget);
        qcaspin->setObjectName(QString::fromUtf8("qcaspin"));
        qcaspin->setGeometry(QRect(70, 320, 101, 25));
        qcaedit = new QCaLineEditPlugin(centralWidget);
        qcaedit->setObjectName(QString::fromUtf8("qcaedit"));
        qcaedit->setGeometry(QRect(40, 270, 127, 25));
        qcacombo = new QCaComboBoxPlugin(centralWidget);
        qcacombo->setObjectName(QString::fromUtf8("qcacombo"));
        qcacombo->setGeometry(QRect(50, 220, 82, 25));
        label23 = new QLabel(centralWidget);
        label23->setObjectName(QString::fromUtf8("label23"));
        label23->setGeometry(QRect(130, 390, 71, 16));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 907, 23));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        qcal_1->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_STATUSTIP
        qcal_2->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        pushButton->setText(QApplication::translate("MainWindow", "BP56", 0, QApplication::UnicodeUTF8));
        label23->setText(QApplication::translate("MainWindow", "TextLabel24", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
