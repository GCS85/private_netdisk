/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionShow_Hidden_Files;
    QWidget *centralWidget;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayoutLeft;
    QLineEdit *lineEditLocal;
    QTableView *tableViewLocal;
    QWidget *layoutWidget_2;
    QVBoxLayout *verticalLayout;
    QPushButton *BtopriDisk;
    QPushButton *BtoLocal;
    QWidget *layoutWidget_3;
    QVBoxLayout *verticalLayoutRight;
    QLineEdit *lineEditpriDisk;
    QTableView *tableViewpriDisk;
    QMenuBar *menuBar;
    QMenu *menuTool;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(657, 429);
        actionShow_Hidden_Files = new QAction(MainWindow);
        actionShow_Hidden_Files->setObjectName(QStringLiteral("actionShow_Hidden_Files"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setGeometry(QRect(30, 10, 598, 225));
        splitter->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        verticalLayoutLeft = new QVBoxLayout(layoutWidget);
        verticalLayoutLeft->setSpacing(6);
        verticalLayoutLeft->setContentsMargins(11, 11, 11, 11);
        verticalLayoutLeft->setObjectName(QStringLiteral("verticalLayoutLeft"));
        verticalLayoutLeft->setContentsMargins(0, 0, 0, 0);
        lineEditLocal = new QLineEdit(layoutWidget);
        lineEditLocal->setObjectName(QStringLiteral("lineEditLocal"));

        verticalLayoutLeft->addWidget(lineEditLocal);

        tableViewLocal = new QTableView(layoutWidget);
        tableViewLocal->setObjectName(QStringLiteral("tableViewLocal"));

        verticalLayoutLeft->addWidget(tableViewLocal);

        splitter->addWidget(layoutWidget);
        layoutWidget_2 = new QWidget(splitter);
        layoutWidget_2->setObjectName(QStringLiteral("layoutWidget_2"));
        verticalLayout = new QVBoxLayout(layoutWidget_2);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        BtopriDisk = new QPushButton(layoutWidget_2);
        BtopriDisk->setObjectName(QStringLiteral("BtopriDisk"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(BtopriDisk->sizePolicy().hasHeightForWidth());
        BtopriDisk->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(BtopriDisk);

        BtoLocal = new QPushButton(layoutWidget_2);
        BtoLocal->setObjectName(QStringLiteral("BtoLocal"));
        sizePolicy.setHeightForWidth(BtoLocal->sizePolicy().hasHeightForWidth());
        BtoLocal->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(BtoLocal);

        splitter->addWidget(layoutWidget_2);
        layoutWidget_3 = new QWidget(splitter);
        layoutWidget_3->setObjectName(QStringLiteral("layoutWidget_3"));
        verticalLayoutRight = new QVBoxLayout(layoutWidget_3);
        verticalLayoutRight->setSpacing(6);
        verticalLayoutRight->setContentsMargins(11, 11, 11, 11);
        verticalLayoutRight->setObjectName(QStringLiteral("verticalLayoutRight"));
        verticalLayoutRight->setContentsMargins(0, 0, 0, 0);
        lineEditpriDisk = new QLineEdit(layoutWidget_3);
        lineEditpriDisk->setObjectName(QStringLiteral("lineEditpriDisk"));

        verticalLayoutRight->addWidget(lineEditpriDisk);

        tableViewpriDisk = new QTableView(layoutWidget_3);
        tableViewpriDisk->setObjectName(QStringLiteral("tableViewpriDisk"));

        verticalLayoutRight->addWidget(tableViewpriDisk);

        splitter->addWidget(layoutWidget_3);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 657, 26));
        menuTool = new QMenu(menuBar);
        menuTool->setObjectName(QStringLiteral("menuTool"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuTool->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        actionShow_Hidden_Files->setText(QApplication::translate("MainWindow", "Show Hidden Files", Q_NULLPTR));
        BtopriDisk->setText(QApplication::translate("MainWindow", "-->", Q_NULLPTR));
        BtoLocal->setText(QApplication::translate("MainWindow", "<--", Q_NULLPTR));
        menuTool->setTitle(QApplication::translate("MainWindow", "Tool", Q_NULLPTR));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
