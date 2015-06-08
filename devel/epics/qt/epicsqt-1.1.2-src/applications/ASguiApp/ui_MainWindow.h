/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created: Wed Jun 19 18:44:12 2013
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
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *actionOpen;
    QAction *actionAbout;
    QAction *actionNew_Window;
    QAction *actionNew_Tab;
    QAction *actionClose;
    QAction *actionDesigner;
    QAction *actionOpen_Current_Form_In_Designer;
    QAction *actionRefresh_Current_Form;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuWindows;
    QMenu *menuEdit;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QString::fromUtf8("MainWindowClass"));
        MainWindowClass->resize(548, 492);
        actionOpen = new QAction(MainWindowClass);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionAbout = new QAction(MainWindowClass);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionNew_Window = new QAction(MainWindowClass);
        actionNew_Window->setObjectName(QString::fromUtf8("actionNew_Window"));
        actionNew_Tab = new QAction(MainWindowClass);
        actionNew_Tab->setObjectName(QString::fromUtf8("actionNew_Tab"));
        actionClose = new QAction(MainWindowClass);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionDesigner = new QAction(MainWindowClass);
        actionDesigner->setObjectName(QString::fromUtf8("actionDesigner"));
        actionOpen_Current_Form_In_Designer = new QAction(MainWindowClass);
        actionOpen_Current_Form_In_Designer->setObjectName(QString::fromUtf8("actionOpen_Current_Form_In_Designer"));
        actionRefresh_Current_Form = new QAction(MainWindowClass);
        actionRefresh_Current_Form->setObjectName(QString::fromUtf8("actionRefresh_Current_Form"));
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 548, 25));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuWindows = new QMenu(menuBar);
        menuWindows->setObjectName(QString::fromUtf8("menuWindows"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuEdit->setEnabled(false);
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuBar->addAction(menuWindows->menuAction());
        menuFile->addAction(actionNew_Window);
        menuFile->addAction(actionNew_Tab);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionClose);
        menuHelp->addAction(actionAbout);
        menuEdit->addAction(actionDesigner);
        menuEdit->addAction(actionOpen_Current_Form_In_Designer);
        menuEdit->addAction(actionRefresh_Current_Form);

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("MainWindowClass", "&Open", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindowClass", "About", 0, QApplication::UnicodeUTF8));
        actionNew_Window->setText(QApplication::translate("MainWindowClass", "New Window", 0, QApplication::UnicodeUTF8));
        actionNew_Tab->setText(QApplication::translate("MainWindowClass", "New Tab", 0, QApplication::UnicodeUTF8));
        actionClose->setText(QApplication::translate("MainWindowClass", "Close", 0, QApplication::UnicodeUTF8));
        actionDesigner->setText(QApplication::translate("MainWindowClass", "Designer", 0, QApplication::UnicodeUTF8));
        actionOpen_Current_Form_In_Designer->setText(QApplication::translate("MainWindowClass", "Open CurrentForm In Designer", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionOpen_Current_Form_In_Designer->setToolTip(QApplication::translate("MainWindowClass", "Open Current Form In Designer", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionRefresh_Current_Form->setText(QApplication::translate("MainWindowClass", "Refresh Current Form", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindowClass", "File", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindowClass", "Help", 0, QApplication::UnicodeUTF8));
        menuWindows->setTitle(QApplication::translate("MainWindowClass", "Windows", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("MainWindowClass", "Edit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
