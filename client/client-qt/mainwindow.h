#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QIcon>
#include <memory>
#include "configfile.h"
#include "prinetdiskclientter.h"

//#define DIRE_NUM 0x04
//#define FILE_NUM 0x08
#define QFOLDER_SYMBOL "Folder"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
//	enum{
//		NONE	= 0,				//取消传输
//		NONEALL
//		ONE		= 1,				//传输，仅针对该文件
//		ALL		= 2					//传输，对于所有重复文件
//	};
	explicit MainWindow(const std::string &localRootPath ,
						QWidget *parent = 0);
	~MainWindow();
	QFileSystemModel	*modelLocal;
	QStandardItemModel	*modelPriDisk;
	const QString rootPathLocal;

private:
	Ui::MainWindow *ui;
	QDir::Filters fileSystemfilter;
	QMenu *rightClickMenuLocal;
	QMenu *rightClickMenuPriDisk;
	std::shared_ptr<priNetDiskClientTer> grapTer;
	std::shared_ptr<QIcon> iconFile;
	std::shared_ptr<QIcon> iconFolder;
	std::shared_ptr<configFile> pConfigFile;
	bool priDiskFileVisible;

	bool check_connection();
	void set_toolBar();
	void set_right_click();
	void config_dialog_action();
	void connect_Server_priDisk();
	void generate_file_item();
	void systemFile_tableView();
	void priDiskFile_tableView();

private slots:
	/* click event */
	void change_dirLocal(const QModelIndex&);
	void change_dirPriDisk(const QModelIndex&);
	/*Display Right Click Menu*/
	void display_right_click_local(QPoint pos);
	void display_right_click_priDisk(QPoint pos);
	/* Action */
	void show_confDialog();
	void delete_file_priDisk();
	void delete_file_local();
	void create_folder_priDisk();
	void create_folder_local();
	void change_file_visible(bool selected);
	void switch_connect(bool turnOn);
	void update_display();
	/* line edit*/
	void on_lineEditLocal_returnPressed();
	void on_lineEditpriDisk_returnPressed();
	/* Button */
	void on_BtopriDisk_clicked();
	void on_BtoLocal_clicked();
};

#endif // MAINWINDOW_H
