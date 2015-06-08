/********************************************************************************
** Form generated from reading UI file 'main.ui'
**
** Created: Wed Oct 2 14:13:29 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_H
#define UI_MAIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *exitAction;
    QAction *reloadAction;
    QAction *unconnectedAction;
    QAction *fileAction;
    QAction *aboutAction;
    QWidget *centralwidget;
    QStatusBar *statusbar;
    QMenuBar *menuBar;
    QMenu *menuMenu;
    QMenu *menuPV;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setWindowModality(Qt::NonModal);
        MainWindow->resize(493, 144);
        exitAction = new QAction(MainWindow);
        exitAction->setObjectName(QString::fromUtf8("exitAction"));
        reloadAction = new QAction(MainWindow);
        reloadAction->setObjectName(QString::fromUtf8("reloadAction"));
        unconnectedAction = new QAction(MainWindow);
        unconnectedAction->setObjectName(QString::fromUtf8("unconnectedAction"));
        fileAction = new QAction(MainWindow);
        fileAction->setObjectName(QString::fromUtf8("fileAction"));
        aboutAction = new QAction(MainWindow);
        aboutAction->setObjectName(QString::fromUtf8("aboutAction"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 493, 22));
        menuMenu = new QMenu(menuBar);
        menuMenu->setObjectName(QString::fromUtf8("menuMenu"));
        menuPV = new QMenu(menuBar);
        menuPV->setObjectName(QString::fromUtf8("menuPV"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuMenu->menuAction());
        menuBar->addAction(menuPV->menuAction());
        menuMenu->addAction(fileAction);
        menuMenu->addSeparator();
        menuMenu->addSeparator();
        menuMenu->addAction(reloadAction);
        menuMenu->addAction(exitAction);
        menuMenu->addSeparator();
        menuMenu->addSeparator();
        menuMenu->addAction(aboutAction);
        menuPV->addAction(unconnectedAction);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        exitAction->setText(QApplication::translate("MainWindow", "&Exit", 0, QApplication::UnicodeUTF8));
        reloadAction->setText(QApplication::translate("MainWindow", "&Reload", 0, QApplication::UnicodeUTF8));
        unconnectedAction->setText(QApplication::translate("MainWindow", "&Unconnected", 0, QApplication::UnicodeUTF8));
        unconnectedAction->setIconText(QApplication::translate("MainWindow", "Unconnected PV's", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        unconnectedAction->setToolTip(QApplication::translate("MainWindow", "Unconnected PV's", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        fileAction->setText(QApplication::translate("MainWindow", "Open &File", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        fileAction->setToolTip(QApplication::translate("MainWindow", "Open File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        aboutAction->setText(QApplication::translate("MainWindow", "&About", 0, QApplication::UnicodeUTF8));
        menuMenu->setTitle(QApplication::translate("MainWindow", "Menu", 0, QApplication::UnicodeUTF8));
        menuPV->setTitle(QApplication::translate("MainWindow", "PV", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_H
