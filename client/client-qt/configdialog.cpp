#include "configdialog.h"
#include "ui_configdialog.h"

#include <QDebug>

ConfigDialog::ConfigDialog(std::shared_ptr<configFile> pConf,QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ConfigDialog),
	pConfigFile(pConf)
{
	ui->setupUi(this);
	setLayout(ui->verticalLayout);
	ui->labelErrorMessage->hide();
}

ConfigDialog::~ConfigDialog()
{
	delete ui;
}

void ConfigDialog::rewrite_config_file()
{
	ui->accountALineEdit->setText(QString(pConfigFile->configKV[ACCOUNT].c_str()));
	ui->passWordPLineEdit->setText(QString(pConfigFile->configKV[PASSWORD].c_str()));
	ui->priDiskLineEdit->setText(QString(pConfigFile->configKV[PRIDISK].c_str()));
	ui->localPortLLineEdit->setText(QString(pConfigFile->configKV[LOCALPROT].c_str()));
}

void ConfigDialog::on_pushButton_OK_clicked()
{
	std::string ErrorMessage;
	std::string tempContent;

	tempContent = ui->accountALineEdit->text().toStdString();
	if( tempContent.empty())
	{
		ErrorMessage = "Account is Empty";
		goto ERROR_OUT;
	}
	pConfigFile->configKV[ACCOUNT] = tempContent;

	tempContent = ui->passWordPLineEdit->text().toStdString();
	if( tempContent.empty() )
	{
//		switch( ret )
//		{
//		case -1 :
//			ErrorMessage = "PassWord is Empty";
//			break;
//		case -2:
//			ErrorMessage = "Public Key is not Exist";
//			break;
//		case -3:
//			ErrorMessage = "Public Key have Error";
//			break;
//		}
		ErrorMessage = "PassWord is Empty";
		goto ERROR_OUT;
	}
	pConfigFile->configKV[PASSWORD] = tempContent;

	tempContent = ui->priDiskLineEdit->text().toStdString();
	if( tempContent.empty())
	{
		ErrorMessage = "PriDisk is Empty";
		goto ERROR_OUT;
	}
	pConfigFile->configKV[PRIDISK] = tempContent;


	tempContent = ui->localPortLLineEdit->text().toStdString();
	for( auto &x : tempContent)
	{
		if( !(x >= '0' && x<= '9') )
		{
			ErrorMessage = "LocalPort Format Error";
			goto ERROR_OUT;
		}
	}
	if( tempContent.empty())
	{
		ErrorMessage = "LocalPort is Empty";
		goto ERROR_OUT;
	}
	pConfigFile->configKV[LOCALPROT] = tempContent;

	if( pConfigFile->flush_file() < 0)
	{
		qDebug()<< ("flush config file failed");
	}
	done(1);

ERROR_OUT:
	ui->labelErrorMessage->setText(ErrorMessage.c_str());
	ui->labelErrorMessage->setStyleSheet("color:red;");
	ui->labelErrorMessage->setFont(QFont("Timers", 10, QFont::Bold));
	ui->labelErrorMessage->setAlignment(Qt::AlignCenter);
	ui->labelErrorMessage->show();
	return ;
}
