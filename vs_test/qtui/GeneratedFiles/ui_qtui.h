/********************************************************************************
** Form generated from reading UI file 'qtui.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTUI_H
#define UI_QTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qtuiClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *qtuiClass)
    {
        if (qtuiClass->objectName().isEmpty())
            qtuiClass->setObjectName(QStringLiteral("qtuiClass"));
        qtuiClass->resize(600, 400);
        menuBar = new QMenuBar(qtuiClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        qtuiClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(qtuiClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        qtuiClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(qtuiClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        qtuiClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(qtuiClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        qtuiClass->setStatusBar(statusBar);

        retranslateUi(qtuiClass);

        QMetaObject::connectSlotsByName(qtuiClass);
    } // setupUi

    void retranslateUi(QMainWindow *qtuiClass)
    {
        qtuiClass->setWindowTitle(QApplication::translate("qtuiClass", "qtui", 0));
    } // retranslateUi

};

namespace Ui {
    class qtuiClass: public Ui_qtuiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTUI_H
