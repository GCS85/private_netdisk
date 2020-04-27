#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configdialog.h"

#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QAction>
#include <QProgressDialog>
#include <QInputDialog>
#include <QFileIconProvider>

static bool is_valid_path(const QString &filePath)
{
	/* Path Rule */
	if( filePath.indexOf("..") != -1)
		return false;
	if( filePath.indexOf(".") != -1)
		return false;
	if( filePath.indexOf("//") != -1)
		return false;

	return true;
}

MainWindow::MainWindow( const std::string &localRootPath ,	QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	rootPathLocal(localRootPath.c_str()),
	priDiskFileVisible(false),
	fileSystemfilter(QDir::AllDirs|QDir::Files|QDir::NoDot),
	pConfigFile(std::make_shared<configFile>())
{
	ui->setupUi(this);
	setCentralWidget(ui->splitter);
	this->config_dialog_action();

	QFileIconProvider fileIcon;
	this->iconFile		= std::make_shared<QIcon>(fileIcon.icon(QFileIconProvider::File));
	this->iconFolder	= std::make_shared<QIcon>(fileIcon.icon(QFileIconProvider::Folder));

	this->grapTer		= std::make_shared<priNetDiskClientTer>( localRootPath );
	this->modelPriDisk	= new QStandardItemModel(this);


	this->connect_Server_priDisk();		//contain function priDiskFile_tableView()
	this->systemFile_tableView();
	this->set_right_click();
	this->set_toolBar();
}

MainWindow::~MainWindow()
{
	delete ui;
}

bool MainWindow::check_connection()
{
	if(false == this->grapTer->connected )
	{
		QMessageBox::warning(this,tr("Warning") , tr("Disconnect priNetDisk"),QMessageBox::Abort);
		return false;
	}
	return true;
}

void MainWindow::set_toolBar()
{
	/* ToolBar & MenuTool */
	QAction *setFileVisible = new QAction(QIcon(":/images/visible"),tr("Show Hidden Files"),this);
	setFileVisible->setToolTip(tr("Show Hidden Files"));
	setFileVisible->setCheckable(true);
	connect(setFileVisible , &QAction::toggled , this, &MainWindow::change_file_visible);
	ui->toolBar->addAction(setFileVisible);
	ui->menuTool->addAction(setFileVisible);

	QAction *setConfigFile = new QAction(QIcon(":/images/setting"),tr("Setting"),this);
	setConfigFile->setToolTip(tr("Setting"));
	connect(setConfigFile , &QAction::triggered , this, &MainWindow::show_confDialog);
	ui->toolBar->addAction(setConfigFile);
	ui->menuTool->addAction(setConfigFile);

	QAction *setConnectPriDisk = new QAction(QIcon(":/images/connect") ,tr("Connect/DisConnect") , this);
	setConnectPriDisk->setToolTip(tr("Connect/DisConnect"));
	setConnectPriDisk->setCheckable(true);
	setConnectPriDisk->setChecked(this->grapTer->connected);
	connect(setConnectPriDisk , &QAction::toggled , this , &MainWindow::switch_connect);
	ui->toolBar->addAction(setConnectPriDisk);
	ui->menuTool->addAction(setConnectPriDisk);

	QAction *setUpdate = new QAction(QIcon(":/images/update"),tr("update display"),this);
	setUpdate->setToolTip(tr("update display"));
	connect(setUpdate, &QAction::triggered , this , &MainWindow::update_display);
	ui->toolBar->addAction(setUpdate);
	ui->menuTool->addAction(setUpdate);
	/* End */
}

void MainWindow::set_right_click()
{
	/* PriDisk Right Click Menu */
	ui->tableViewpriDisk->setContextMenuPolicy(Qt::CustomContextMenu);			//right hand-button menu
	connect(ui->tableViewpriDisk, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(display_right_click_priDisk(QPoint)));
	QAction *deletePriDiskFile = new QAction(this);
	deletePriDiskFile->setText("Delete");
	connect(deletePriDiskFile , &QAction::triggered ,
			this , &MainWindow::delete_file_priDisk);
	QAction *transferFileToLocal = new QAction(this);
	transferFileToLocal->setText("Transfer Files to Local");
	connect(transferFileToLocal , &QAction::triggered ,
			this,&MainWindow::on_BtoLocal_clicked);

	QAction *createPriDiskFolder = new QAction(this);
	createPriDiskFolder->setText("New Folder");
	connect(createPriDiskFolder, &QAction::triggered ,
			this,&MainWindow::create_folder_priDisk);
	rightClickMenuPriDisk = new QMenu(ui->tableViewpriDisk);
	rightClickMenuPriDisk->addAction(deletePriDiskFile);
	rightClickMenuPriDisk->addAction(transferFileToLocal);
	rightClickMenuPriDisk->addAction(createPriDiskFolder);

	/* Local Right Click Menu */
	ui->tableViewLocal->setContextMenuPolicy(Qt::CustomContextMenu);			//right hand-button menu
	connect(ui->tableViewLocal, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(display_right_click_local(QPoint)));

	QAction *deleteLocalFile = new QAction (this);
	deleteLocalFile ->setText (  "Delete" );
	connect( deleteLocalFile , &QAction::triggered ,
			 this , &MainWindow::delete_file_local );
	QAction	*transferFileToPriDisk = new QAction(this);
	transferFileToPriDisk->setText( "Transfer Files to PriDisk" );
	connect( transferFileToPriDisk , &QAction::triggered ,
			 this , &MainWindow::on_BtopriDisk_clicked);
	QAction *createLocalFolder = new QAction(this);
	createLocalFolder->setText("New Folder");
	connect(createLocalFolder , &QAction::triggered ,
			this,&MainWindow::create_folder_local);

	rightClickMenuLocal = new QMenu(ui->tableViewLocal);
	rightClickMenuLocal->addAction(deleteLocalFile);
	rightClickMenuLocal->addAction(transferFileToPriDisk);
	rightClickMenuLocal->addAction(createLocalFolder);
	/* End */
}

void MainWindow::config_dialog_action()
{
	if( pConfigFile->analyze_config_file() )
	{
		ConfigDialog DiaLog(pConfigFile,this);
		if ( DiaLog.exec() == QDialog::Rejected)
		{
			exit(0);
		}
	}
	/* config File content*/
	{
		for( auto &x : pConfigFile->configKV)
			qDebug() << x.first.c_str() << "=" << x.second.c_str();
	}
	/* END */
}

void MainWindow::connect_Server_priDisk()
{
	while( ! this->grapTer->connected )
	{
		QCoreApplication::processEvents();
		while(  grapTer->startClient("127.0.0.1" , std::stoi(pConfigFile->configKV[LOCALPROT]) ,
									 "C"+pConfigFile->configKV[ACCOUNT]+":"+pConfigFile->configKV[PASSWORD])
				< 0 )
		{
			const std::string errorMes(grapTer->getErrorMessage()+"\ncontinue connect?");
			if( QMessageBox::warning(this,tr("Warning") , tr(errorMes.c_str()),
									 QMessageBox::Yes , QMessageBox::No) == QMessageBox::No)
			{
				exit(0);
			}
			show_confDialog();

		}

		if( grapTer->connectNetDisk(pConfigFile->configKV[PRIDISK]) < 0)
		{
			const std::string &errorMes(grapTer->getErrorMessage()+"\nshow settings?");
			if( QMessageBox::warning(this,tr("Warning") , tr(errorMes.c_str()),
									 QMessageBox::Yes , QMessageBox::No) == QMessageBox::No)
				exit(0);
			show_confDialog();
		}
		else
		{
			this->grapTer->order_show_file();
			/* tableView PriDisk display */
			this->generate_file_item();
			break;
		}
	}
}

void MainWindow::generate_file_item()
{
	QStandardItem *tempItem;
	this->modelPriDisk->clear();
	auto containNoHD = this->grapTer->curDirNoHDContent;

	/* Display  Directory .. */
	tempItem = new QStandardItem(*iconFolder , ".." );
	this->modelPriDisk->setItem(0 , 0 , tempItem );
	/* End */

	int disPos = 1;
	if(this->priDiskFileVisible)
	{
		auto containHD = this->grapTer->curDirHiddenContent;
		for( int i = 0 ; i < containHD.size() ; ++i )
		{
			if( containHD[i].fileType == DIRE_NUM)
				tempItem = new QStandardItem(*iconFolder,QString(containHD[i].fileName.c_str()));
			else	//contain[i].fileType == DIRE_NUM
			{
				tempItem = new QStandardItem(containHD[i].fileSize.c_str());
				this->modelPriDisk->setItem( disPos  , 1 , tempItem);
				tempItem = new QStandardItem(*iconFile,QString(containHD[i].fileName.c_str()));
			}
			this->modelPriDisk->setItem( disPos  , 0  ,tempItem);
			disPos++;
		}
	}

	for( int i = 0 ; i < containNoHD.size() ; i++)
	{
		if( containNoHD[i].fileType == DIRE_NUM)
			tempItem = new QStandardItem(*iconFolder,QString(containNoHD[i].fileName.c_str()));
		else	//contain[i].fileType == DIRE_NUM
		{
			tempItem = new QStandardItem(containNoHD[i].fileSize.c_str());
			this->modelPriDisk->setItem( disPos  , 1 , tempItem);
			tempItem = new QStandardItem(*iconFile,QString(containNoHD[i].fileName.c_str()));
		}
		this->modelPriDisk->setItem( disPos  , 0  ,tempItem);
		disPos++;
	}

	this->priDiskFile_tableView();
}

void MainWindow::systemFile_tableView()
{
	/* System File */
	modelLocal = new QFileSystemModel(this);
	modelLocal->setRootPath(rootPathLocal);
	modelLocal->setFilter(fileSystemfilter);

	ui->tableViewLocal->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableViewLocal->setModel(modelLocal);
	ui->tableViewLocal->setRootIndex( modelLocal->index(this->grapTer->curPathLocal.c_str()));
	ui->tableViewLocal->setColumnHidden(2,true);
	ui->tableViewLocal->setColumnHidden(3,true);
	ui->tableViewLocal->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui->tableViewLocal->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableViewLocal->setShowGrid(false);
	ui->tableViewLocal->verticalHeader()->hide();

	ui->lineEditLocal->setText(this->grapTer->curPathLocal.c_str());
	connect(ui->tableViewLocal , SIGNAL(doubleClicked(QModelIndex)),this,SLOT(change_dirLocal(QModelIndex)));
	/* End */
}

void MainWindow::priDiskFile_tableView()
{
	QStringList tableHead;
	tableHead <<"Name"<< "Size";
	this->modelPriDisk->setHorizontalHeaderLabels(tableHead);

	ui->tableViewpriDisk->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableViewpriDisk->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

	ui->tableViewpriDisk->verticalHeader()->hide();

	ui->tableViewpriDisk->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui->tableViewpriDisk->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableViewpriDisk->setShowGrid(false);
	ui->tableViewpriDisk->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableViewpriDisk->setModel(this->modelPriDisk);

	ui->lineEditpriDisk->setText(this->grapTer->curPathPriDisk.c_str());
	connect(ui->tableViewpriDisk, SIGNAL(doubleClicked(QModelIndex)),this,SLOT(change_dirPriDisk(QModelIndex)));
}

void MainWindow::change_dirLocal(const QModelIndex &selected)
{
	QModelIndex Index = modelLocal->index(selected.row(),0,selected.parent());
	qDebug() << "double Clicked\t " << Index.data();
	if( Index.flags())
	{
		if( modelLocal->isDir(Index))
		{
			if( Index.data().toString() == QString(".."))
			{
				if( 0 == this->grapTer->curPathLocal.compare(rootPathLocal.toStdString()))
					return ;

				int pos = this->grapTer->curPathLocal.find_last_of('/', this->grapTer->curPathLocal.length()-2 );
				this->grapTer->curPathLocal.erase(pos+1);
				ui->tableViewLocal->setRootIndex( modelLocal->index(this->grapTer->curPathLocal.c_str()));
			}
			else
			{
				ui->tableViewLocal->setRootIndex( Index );
				this->grapTer->curPathLocal.append(Index.data().toString().toStdString() + "/");
			}
		}
		ui->lineEditLocal->setText(this->grapTer->curPathLocal.c_str());
	}
	qDebug() << this->grapTer->curPathLocal.c_str();
}

void MainWindow::change_dirPriDisk(const QModelIndex &selected)
{
	if( this->check_connection() )
	{
		qDebug("PriDisk double Clicked");
		int rowDirContent = selected.row() - 1;
		auto Index = this->grapTer->curDirNoHDContent;
		auto NoHDContentSize = this->grapTer->curDirHiddenContent.size();
		if( -2 == rowDirContent )		//when click the ..
			return ;
		if( -1 == rowDirContent )		//  selected  Dir equal ".."
		{
			if( 0 != this->grapTer->curPathPriDisk.compare("/"))
			{
				// equal
				int pos = this->grapTer->curPathPriDisk.find_last_of('/' , this->grapTer->curPathPriDisk.length()-2);
				this->grapTer->curPathPriDisk.erase(pos+1);
				if( /*this->grapTer->talkToServer(H_SHOWFILES + "#" + this->grapTer->curPathPriDisk)*/
						this->grapTer->order_show_file() != -1)
					this->generate_file_item();
			}
			return;
		}

		if( this->priDiskFileVisible )
			if( rowDirContent < NoHDContentSize )		// Hidden File
				Index = this->grapTer->curDirHiddenContent;
			else										// Visible && select normalFile
				rowDirContent -= NoHDContentSize;

		if( Index[rowDirContent].fileType == DIRE_NUM)
		{
			if( /*this->grapTer->talkToServer(H_SHOWFILES + "#" +this->grapTer->curPathPriDisk + Index[rowDirContent].fileName+"/")*/
					this->grapTer->order_show_file(Index[rowDirContent].fileName+"/")!= -1)
			{
				this->grapTer->curPathPriDisk.append(Index[rowDirContent].fileName + "/");
				this->generate_file_item();
			}
		}
	}
}

void MainWindow::display_right_click_local(QPoint pos)
{
	QModelIndex posDisplay = ui->tableViewLocal->indexAt(pos);
	if (posDisplay.isValid())
		rightClickMenuLocal->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
}

void MainWindow::display_right_click_priDisk(QPoint pos)
{
	QModelIndex posDisplay = ui->tableViewpriDisk->indexAt(pos);

	if ( posDisplay.isValid() )
	{
		qDebug() << "click valid\n" << posDisplay.data();
		rightClickMenuPriDisk->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
	}
}

void MainWindow::on_lineEditLocal_returnPressed()
{
	QString filePath(ui->lineEditLocal->text());
	QFileInfo fileInfo(filePath);
	if( !is_valid_path(filePath) )
		goto ERROR_OUT;
	if( fileInfo.exists())
	{
		if( fileInfo.isFile() )
		{
			int pos = filePath.lastIndexOf('/');
			filePath.remove(pos+1,filePath.length()-pos-1);
		}
		else		//fileInfo is directory
		{
			if( *(filePath.rbegin()) != QChar('/') )
			{
				filePath.append('/');
				ui->lineEditLocal->setText(filePath);
			}
			if( filePath.indexOf(rootPathLocal) != 0)
			{
				QMessageBox::warning(this,tr("Warning") , tr("NOT Permit"),QMessageBox::Abort);
				return ;
			}
		}
		this->grapTer->curPathLocal.assign(filePath.toStdString());
		ui->tableViewLocal->setRootIndex(modelLocal->index(this->grapTer->curPathLocal.c_str()));
	}
	else
ERROR_OUT:
		QMessageBox::warning(this,tr("Warning") , tr("Error File|Dir Path"),QMessageBox::Abort);
}

void MainWindow::on_lineEditpriDisk_returnPressed()
{
	if( this->check_connection())
	{
		QString filePath(ui->lineEditpriDisk->text());
		if( (*filePath.rbegin()) != '/')
			filePath.push_back('/');
		if( this->grapTer->curPathPriDisk.compare(filePath.toStdString()) == 0)
			return ;
		if( is_valid_path(filePath) && filePath.indexOf('/') == 0 )
		{
			if( /*this->grapTer->talkToServer(H_SHOWFILES + "#" + filePath.toStdString())*/
					this->grapTer->order_show_file(filePath.toStdString() ) != -1)
			{
				this->grapTer->curPathPriDisk.assign(filePath.toStdString());
				this->generate_file_item();
				return ;
			}
		}
ERROR_OUT:
		ui->lineEditpriDisk->setText(this->grapTer->curPathPriDisk.c_str());
		QMessageBox::warning(this,tr("Warning") , tr("Error File|Dir Path"),QMessageBox::Abort);
	}
}

void MainWindow::change_file_visible(bool selected)
{
	/* set LocalFileSystem */
	fileSystemfilter.setFlag( QDir::Hidden , selected);
	modelLocal->setFilter(fileSystemfilter);

	/* set PriDisk Filesystem*/
	this->priDiskFileVisible = selected;
	this->generate_file_item();
}

void MainWindow::switch_connect(bool turnOn)
{
	if( turnOn )
	{
		if( !this->grapTer->connected )
			connect_Server_priDisk();
	}
	else
	{
		this->grapTer->disconnectNetDisk();
		this->modelPriDisk->clear();
		ui->lineEditpriDisk->clear();
	}
}

void MainWindow::update_display()
{
	//	ui->tableViewLocal->setRootIndex( modelLocal->index(this->grapTer->curPathLocal.c_str()));
	if( this->grapTer->order_show_file() != -1 )
		this->generate_file_item();
}

void MainWindow::show_confDialog()
{
	ConfigDialog dialog(this->pConfigFile , this);
	dialog.rewrite_config_file();
	if ( dialog.exec() == QDialog::Rejected )
	{
		if( this->isHidden() )
			exit(0);
		else
			dialog.close();
	}
}

void MainWindow::delete_file_priDisk()
{
	if( this->check_connection() )
	{
		std::vector<std::string> fileNames;
		QModelIndexList selectedFile = ui->tableViewpriDisk->selectionModel()->selectedIndexes();
		if( selectedFile.empty() )
		{
			QMessageBox::warning(this,tr("Warning") , tr("Select No One"));
			return ;
		}
		else
		{
			QString deleteFiles("Delete Under Files? \n");
			for( auto temp = selectedFile.begin() ; temp != selectedFile.end() ; temp +=2)
			{
				if( 0 == temp->data().toString().compare("..") )
				{
					QMessageBox::warning(this,tr("Warning"), tr("Can Not Delete under folder\n .. "));
					return ;
				}
				/* send delete order */
				const QString &fileName(temp->data().toString());
				fileNames.push_back(fileName.toStdString());
				deleteFiles.append(fileName);
				deleteFiles.push_back('\n');
			}
			if( QMessageBox::information(this,tr("Delete") , tr(deleteFiles.toStdString().c_str()),QMessageBox::Yes , QMessageBox::No) != QMessageBox::No )
			{
				if( -1 != this->grapTer->order_remove_files(fileNames))
					this->update_display();
			}
		}
	}
}

void MainWindow::delete_file_local()
{
	QModelIndexList selectedFile = ui->tableViewLocal->selectionModel()->selectedIndexes();
	if( selectedFile.empty() )
	{
		QMessageBox::warning(this,tr("Warning") , tr("Select No One"));
		return;
	}
	else
	{
		QString deleteFiles("Delete Under Files? \n");
		for( auto temp = selectedFile.begin() ; temp != selectedFile.end() ; temp += 4 )
		{
			if( 0 == temp->data().toString().compare("..") )
			{
				QMessageBox::warning(this,tr("Warning"), tr("Can Not Delete under folder\n .. "));
				return ;
			}
			deleteFiles.append(temp->data().toString());
			deleteFiles.push_back('\n');
		}
		if( QMessageBox::information(this,tr("Delete") , tr(deleteFiles.toStdString().c_str()),QMessageBox::Yes , QMessageBox::No) != QMessageBox::No )
		{
			/* Delete File by QFileSystemModle*/
			for( auto temp = selectedFile.begin() ; temp != selectedFile.end() ; temp += 4 )
				this->modelLocal->remove(*temp);
		}
	}
}

void MainWindow::create_folder_priDisk()
{
	bool ok;
	QString Text = QInputDialog::getText(this, tr("Input the Folder Name") ,
										 tr("Folder Name:") , QLineEdit::Normal , tr("New Folder") , &ok);
	if( ok )
	{
		this->grapTer->order_create_folder(Text.toStdString());
		this->update_display();
	}

}

void MainWindow::create_folder_local()
{
	bool ok;
	QString Text = QInputDialog::getText(this, tr("Input the Folder Name") ,
										 tr("Folder Name:") , QLineEdit::Normal , tr("New Folder") , &ok);
	if( ok )
		this->modelLocal->mkdir(this->modelLocal->index(this->grapTer->curPathLocal.c_str()),
							Text);
}

/* uploadFile order */
void MainWindow::on_BtopriDisk_clicked()
{
	if( this->check_connection())
	{

		QModelIndexList selectedFile = ui->tableViewLocal->selectionModel()->selectedIndexes();
		if( selectedFile.empty() )
		{
			QMessageBox::warning(this,tr("Warning") , tr("Select No One"));
			return ;
		}
		else
		{
			QMessageBox msgBox;
			msgBox.setInformativeText("This file have exited");
			msgBox.setStandardButtons(	QMessageBox::Yes	|
										QMessageBox::YesAll	|
										QMessageBox::No		|
										QMessageBox::NoAll	);

			msgBox.setDefaultButton(QMessageBox::Yes);
			int transferOption = QMessageBox::Yes;			//标志重复文件  传输方案

			int ret = -1;
			for( auto temp = selectedFile.begin() ; temp != selectedFile.end() ; temp += 4 )
			{
				bool transfer = true;
				const std::string &fileName(temp->data().toString().toStdString());
				if( 0 == fileName.compare(".."))
				{
					QMessageBox::warning(this,tr("Warning"), tr("Can Not Transfer under folder\n .. "));
					return ;
				}

				qDebug() <<"fileName :"<< fileName.c_str() <<
						   "\nfiletype :" << (temp+2)->data() <<
						   "\nfilePath :" << this->grapTer->curPathLocal.c_str();
				/*在 grapTer-> curDirNoHDContent curDirHiddenContent 中 查找 是否有匹配的内容 */
				if( transferOption != QMessageBox::YesAll)
				{
					if( fileName[0] == '.')							//查找  curDirHiddenContent
					{
						for( auto x : this->grapTer->curDirHiddenContent )
							if( x.fileName.compare(fileName) == 0 )
							{
								if( transferOption == QMessageBox::NoAll )
									transfer = false;
								else
								{
									msgBox.setText((fileName+" have exist\n Cover It?").c_str());
									transferOption = msgBox.exec();
									if( transferOption == QMessageBox::No  ||
										transferOption == QMessageBox::NoAll)
										transfer = false;
								}
								break;
							}
					}
					else											//查找  curDirNoHDContent
					{
						for( auto x : this->grapTer->curDirNoHDContent )
							if( x.fileName.compare(fileName) == 0 )
							{
								if( transferOption == QMessageBox::NoAll)
									transfer = false;
								else
								{
									msgBox.setText((fileName+" have exist\n Cover It?").c_str());
									transferOption = msgBox.exec();
									if( transferOption == QMessageBox::No ||
										transferOption == QMessageBox::NoAll)
										transfer = false;
								}
								break;
							}
					}
				}
				if( transfer )
				{
					if( (temp+2)->data().toString().compare(QFOLDER_SYMBOL) == 0)				/* folder transfer */
						ret = this->grapTer->order_upload_folder(fileName);
					else																		/* normel file Transfer */
						ret = this->grapTer->order_upload_file(fileName);
					if( ret < 0 )
					{
						QMessageBox::warning(this,tr("Warning"), tr("Transfer ERROR"));
						return ;
					}
				}

			}
		}
		/* update the folder */
		this->update_display();
	}
}
/* Send downloadFile order */
void MainWindow::on_BtoLocal_clicked()
{
	QModelIndexList selectedFile = ui->tableViewpriDisk->selectionModel()->selectedIndexes();
	auto folderContent = this->grapTer->curDirNoHDContent;
	int rowFileContent = -1;
	if( selectedFile.empty() )
	{
		QMessageBox::warning(this,tr("Warning") , tr("Select No One"));
		return;
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setInformativeText("This file have exited");
		msgBox.setStandardButtons(	QMessageBox::Yes	|
									QMessageBox::YesAll	|
									QMessageBox::No		|
									QMessageBox::NoAll	);

		msgBox.setDefaultButton(QMessageBox::Yes);
		int transferOption = QMessageBox::Yes;			//标志重复文件  传输方案

		for( auto temp = selectedFile.begin() ; temp != selectedFile.end() ; temp += 2 )
		{
			bool transfer = true;
			const QString &fileName(temp->data().toString());
			if( 0 == fileName.compare("..") )
			{
				QMessageBox::warning(this,tr("Warning"), tr("Can Not Transfer under folder\n .. "));
				return ;
			}

			if( transferOption != QMessageBox::YesAll)
			{
				//合法说明 文件或者文件夹存在
				if( modelLocal->index((this->grapTer->curPathLocal+fileName.toStdString()).c_str()).isValid())
				{
					if( transferOption == QMessageBox::NoAll)
						transfer = false;
					else
					{
						msgBox.setText( fileName + " have exist\n Cover It?" );
						transferOption = msgBox.exec();
						if( transferOption == QMessageBox::No ||
								transferOption == QMessageBox::NoAll)
							transfer = false;
					}
				}
			}
			if( transfer )
			{
				rowFileContent = temp->row()-1;
				if( rowFileContent != -1 )
				{
					qDebug() << "fileName :"<<  folderContent[rowFileContent].fileName.c_str() <<
								"\nfileType :"<< (int)folderContent[rowFileContent].fileType;
					if(folderContent[rowFileContent].fileType == DIRE_NUM)
					{
//						this->modelLocal->mkdir();
						this->grapTer->order_download_folder(folderContent[rowFileContent].fileName);
					}
					else
						this->grapTer->order_download_file(folderContent[rowFileContent].fileName);
				}
			}
		}
		ui->tableViewLocal->setRootIndex( modelLocal->index(this->grapTer->curPathLocal.c_str()));
	}
}

