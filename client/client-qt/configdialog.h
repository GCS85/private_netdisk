#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <memory>

#include "configfile.h"


namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ConfigDialog(std::shared_ptr<configFile> pConf,QWidget *parent = 0);
	~ConfigDialog();
	void rewrite_config_file();
private slots:
	void on_pushButton_OK_clicked();

private:
	Ui::ConfigDialog *ui;
	std::shared_ptr<configFile> pConfigFile;
	std::string configFilePath;
};

#endif // CONFIGDIALOG_H
