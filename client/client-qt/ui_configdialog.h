/********************************************************************************
** Form generated from reading UI file 'configdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGDIALOG_H
#define UI_CONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigDialog
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *accountALabel;
    QLineEdit *accountALineEdit;
    QLabel *passWordPLabel;
    QLineEdit *passWordPLineEdit;
    QLabel *priDiskLabel;
    QLineEdit *priDiskLineEdit;
    QLabel *localPortLLabel;
    QLineEdit *localPortLLineEdit;
    QLabel *labelErrorMessage;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_Cancel;
    QPushButton *pushButton_OK;

    void setupUi(QDialog *ConfigDialog)
    {
        if (ConfigDialog->objectName().isEmpty())
            ConfigDialog->setObjectName(QStringLiteral("ConfigDialog"));
        ConfigDialog->resize(282, 233);
        layoutWidget = new QWidget(ConfigDialog);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 261, 211));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout->setContentsMargins(7, 7, 7, 7);
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        accountALabel = new QLabel(layoutWidget);
        accountALabel->setObjectName(QStringLiteral("accountALabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, accountALabel);

        accountALineEdit = new QLineEdit(layoutWidget);
        accountALineEdit->setObjectName(QStringLiteral("accountALineEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, accountALineEdit);

        passWordPLabel = new QLabel(layoutWidget);
        passWordPLabel->setObjectName(QStringLiteral("passWordPLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, passWordPLabel);

        passWordPLineEdit = new QLineEdit(layoutWidget);
        passWordPLineEdit->setObjectName(QStringLiteral("passWordPLineEdit"));
        passWordPLineEdit->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(1, QFormLayout::FieldRole, passWordPLineEdit);

        priDiskLabel = new QLabel(layoutWidget);
        priDiskLabel->setObjectName(QStringLiteral("priDiskLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, priDiskLabel);

        priDiskLineEdit = new QLineEdit(layoutWidget);
        priDiskLineEdit->setObjectName(QStringLiteral("priDiskLineEdit"));

        formLayout->setWidget(2, QFormLayout::FieldRole, priDiskLineEdit);

        localPortLLabel = new QLabel(layoutWidget);
        localPortLLabel->setObjectName(QStringLiteral("localPortLLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, localPortLLabel);

        localPortLLineEdit = new QLineEdit(layoutWidget);
        localPortLLineEdit->setObjectName(QStringLiteral("localPortLLineEdit"));

        formLayout->setWidget(3, QFormLayout::FieldRole, localPortLLineEdit);


        verticalLayout->addLayout(formLayout);

        labelErrorMessage = new QLabel(layoutWidget);
        labelErrorMessage->setObjectName(QStringLiteral("labelErrorMessage"));

        verticalLayout->addWidget(labelErrorMessage);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, -1, -1, 10);
        pushButton_Cancel = new QPushButton(layoutWidget);
        pushButton_Cancel->setObjectName(QStringLiteral("pushButton_Cancel"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton_Cancel->sizePolicy().hasHeightForWidth());
        pushButton_Cancel->setSizePolicy(sizePolicy);
        pushButton_Cancel->setIconSize(QSize(16, 16));

        horizontalLayout->addWidget(pushButton_Cancel);

        pushButton_OK = new QPushButton(layoutWidget);
        pushButton_OK->setObjectName(QStringLiteral("pushButton_OK"));
        sizePolicy.setHeightForWidth(pushButton_OK->sizePolicy().hasHeightForWidth());
        pushButton_OK->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(pushButton_OK);


        verticalLayout->addLayout(horizontalLayout);

#ifndef QT_NO_SHORTCUT
        accountALabel->setBuddy(accountALineEdit);
        passWordPLabel->setBuddy(passWordPLineEdit);
        localPortLLabel->setBuddy(localPortLLineEdit);
#endif // QT_NO_SHORTCUT

        retranslateUi(ConfigDialog);
        QObject::connect(pushButton_Cancel, SIGNAL(clicked()), ConfigDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *ConfigDialog)
    {
        ConfigDialog->setWindowTitle(QApplication::translate("ConfigDialog", "Dialog", Q_NULLPTR));
        accountALabel->setText(QApplication::translate("ConfigDialog", "Account(&A)", Q_NULLPTR));
        passWordPLabel->setText(QApplication::translate("ConfigDialog", "PassWord(&P)", Q_NULLPTR));
        priDiskLabel->setText(QApplication::translate("ConfigDialog", "PriDisk", Q_NULLPTR));
        localPortLLabel->setText(QApplication::translate("ConfigDialog", "LocalPort(&L)", Q_NULLPTR));
        localPortLLineEdit->setText(QApplication::translate("ConfigDialog", "8888", Q_NULLPTR));
        labelErrorMessage->setText(QApplication::translate("ConfigDialog", "TextLabel", Q_NULLPTR));
        pushButton_Cancel->setText(QApplication::translate("ConfigDialog", "Cancel", Q_NULLPTR));
        pushButton_OK->setText(QApplication::translate("ConfigDialog", "OK", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ConfigDialog: public Ui_ConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGDIALOG_H
