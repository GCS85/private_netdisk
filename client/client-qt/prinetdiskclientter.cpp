#include "prinetdiskclientter.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <queue>

#include <sys/stat.h>
#include <dirent.h>
#include <cstring>

off_t getFileSize(std::string fileName)
{
	struct stat statBuf;
	if( -1 == stat(fileName.c_str(),&statBuf))
		return -1;
	return statBuf.st_size;
}

static void displayCurDir()
{
	struct dirent **nameList;
	int n = scandir("." ,&nameList,nullptr ,alphasort);
	if(n < 0 )
	{
		std::cout <<"ERROR"<<std::endl;
	}
	else
	{
		while(n--)
		{
			if( 0 == strcmp(nameList[n]->d_name , "."))
				continue;
			std::cout << nameList[n]->d_name << "   ";
			free(nameList[n]);
		}
		free(nameList);
	}
}

/* class priNetDiskClientTer */
priNetDiskClientTer::priNetDiskClientTer(const std::string &rootPath,
										 const int& protoType ):
//	priNetDiskClient(/*myPassPort*/),
	curPathLocal(rootPath),
	curPathPriDisk("/"),
	connected(false)
{
	initClient();
}

void priNetDiskClientTer::startInterActive()
{
	showMemu();
	int order;

	while(1)
	{
		std::cout <<"\nplease Input the order number : ";
		std::cin >> order;
		switch(order)
		{
		case 1:
		{
			std::string dirPath;
			std::cout <<"please Input the Dir Path : ";
			std::cin >> dirPath;
			if(0 == dirPath.compare(".."))
			{
				if( curPathLocal == "/" )
				{
					std::cout << "Error Path"<<std::endl;
					break;
				}
			}
			if( 0 != dirPath.compare("."))
			{
				curPathLocal.append(dirPath);
			}
			talkToServer("showFiles#"+curPathLocal);
			break;
		}
		case 2:
		{
			std::string fileName;
			std::cout <<"please Input the fileName : ";
			std::cin >> fileName;
//			fileName.push_back(':');
//			fileName.push_back(0x08);
//			if( curDirContent.end() == std::find(curDirContent.begin(),curDirContent.end(),fileName))
//			{
//				std::cout << "Not found this File: "<< fileName<<std::endl;
//				break;
//			}
			talkToServer("downloadFile#"+curPathLocal+fileName);
			break;
		}
		case 3:
		{
			displayCurDir();
			std::string fileName;
			std::cout <<"\nplease Input the fileName : ";
			std::cin >> fileName;
			const off_t &fileSize = getFileSize(fileName);
			talkToServer("uploadFile#"+
						 curPathLocal+
						 fileName +":"+
						 std::to_string(fileSize)
						 );
			std::cout <<"Start Send File"<<std::endl;
			this->pNWman->doWriteFile(fileName,fileSize);
			break;
		}
		case 4:
			return ;
		default:
		{
			std::cout << "error Input!"<<std::endl;
			break;
		}
		}
	}
}

int priNetDiskClientTer::connectNetDisk(const std::string &netDiskName)
{
	if( talkToServer(H_SEARCHCLIENT + "#" + netDiskName) == -2)
		return -1;
	//必须睡眠等待 PI端进入监听状态
	std::this_thread::sleep_for(std::chrono::seconds(4));
	pNWman.reset();
	pNWman = std::make_shared<NWmanagerUDTClient>();
	pNWman->bindLocal(this->localPort);
	pNWman->dealMsg = std::bind(&priNetDiskClient::onMsgDeal ,this , std::placeholders::_1,std::placeholders::_2);
	if( pNWman->doStart(OclientIP,OclientPort) == -1)
	{
//		std::cout <<" connect NetDisk refuse " <<std::endl;
		this->errMes.assign("connect NetDisk refuse");
		return -1;
	}
	this->connected = true;
	return 0;
}

int priNetDiskClientTer::disconnectNetDisk()
{
	if( this->connected )
	{
		this->pNWman->doStop();
		this->connected =false;
	}
}

void priNetDiskClientTer::initClient()
{
	dealMsgFunc.clear();
	dealMsgFunc[H_SEARCHCLIENT]		 = std::bind(&priNetDiskClientTer::searchClient,	this,std::placeholders::_1);
	dealMsgFunc[H_SHOWFILES]		 = std::bind(&priNetDiskClientTer::showFiles,		this,std::placeholders::_1);
	dealMsgFunc[H_DOWNLOADFILE]		 = std::bind(&priNetDiskClientTer::downloadFile,	this,std::placeholders::_1);
	dealMsgFunc[H_UPLOADFILE]		 = std::bind(&priNetDiskClientTer::uploadFile,		this,std::placeholders::_1);
	dealMsgFunc[H_REMOVEFILES]		 = std::bind(&priNetDiskClientTer::removeFiles,		this,std::placeholders::_1);
	dealMsgFunc[H_CREATEFOLDER]		 = std::bind(&priNetDiskClientTer::createFolder,	this,std::placeholders::_1);
	dealMsgFunc[H_DOWNLOADFOLDER]	 = std::bind(&priNetDiskClientTer::downloadFolder,	this,std::placeholders::_1);
}

void priNetDiskClientTer::showMemu()
{
	std::cout <<"*************************"<<std::endl;
	std::cout <<"1 : show file "<<std::endl;
	std::cout <<"2 : download file " <<std::endl;
	std::cout <<"3 : upload file " <<std::endl;
	std::cout <<"4 : quit " <<std::endl;
	std::cout <<"*************************"<<std::endl;
}

void priNetDiskClientTer::searchClient(const std::string &Oclient)
{
	//std::cout << Oclient <<std::endl;
	OclientIP = Oclient.substr(0,Oclient.find(':'));
	std::istringstream in(Oclient.substr(Oclient.find(':')+1));
	in >> OclientPort;
	this->serverSignal = true;
}

void priNetDiskClientTer::showFiles(const std::string &filesInfos)	//filesInfos == fileName1:fileType1:fileSize1(string)&fileName2:fileType2:fileSize2
{
	this->curDirNoHDContent.clear();
	this->curDirHiddenContent.clear();

	std::size_t posColonFir = 0 , posColonSec = 0 , posAnd = 0 ,posBeg = 0;
	while(posBeg < filesInfos.length())
	{
		posColonFir = filesInfos.find(':' , posAnd);
		posColonSec = filesInfos.find(':' , posColonFir+1);
		posAnd = filesInfos.find('&',posColonFir);
		struct fileInfo temp;
		temp.fileName.assign(filesInfos.substr(posBeg,posColonFir-posBeg));
		temp.fileType = filesInfos.at(posColonFir+1);
		temp.fileSize = filesInfos.substr(posColonSec+1,posAnd-posColonSec-1);

		if( temp.fileName[0] == '.')		//Hidden File
			curDirHiddenContent.push_back(temp);
		else
			curDirNoHDContent.push_back(temp);
		posBeg = posAnd+1;
	}

}

void priNetDiskClientTer::removeFiles(const std::string &result)
{
	//	std::cout << result<< std::endl;
}

void priNetDiskClientTer::createFolder(const std::string &)
{
}

void priNetDiskClientTer::downloadFolder(const std::string &filesInfos)
{
	std::size_t posColon = 0 , posAnd = 0 ;
	std::queue<std::string> WRFolders;				//Wait for Recv Folders
	while(posAnd < filesInfos.length())
	{
		posColon = filesInfos.find(':' , posAnd);
		const std::string	&fileName(filesInfos.substr(posAnd,posColon-posAnd));
		const char			&fileType(filesInfos.at(posColon+1));

		posAnd = filesInfos.find('&' , posColon);
		posAnd ++;
		if( fileType == DIRE_NUM )
		{
			WRFolders.push(fileName);
			continue;
		}
		this->order_download_file( this->curPathRelaFolder+fileName);

	}

	while( !WRFolders.empty() )
	{
		order_download_folder(WRFolders.front());
		WRFolders.pop();
	}
	this->curPathRelaFolder.clear();
}

void priNetDiskClientTer::uploadFile(const std::string &result)
{
	std::cout << result <<std::endl;

}

void priNetDiskClientTer::downloadFile(const std::string &fileInfo)// fileInfo == "fileName:fileSize"
{
	auto pos = fileInfo.find(':');
	if( pos == fileInfo.length())
	{
		std::cout << "fileInfo error"<<std::endl;
		return;
	}
	const std::string fileName(fileInfo.substr(0,pos));
	const off64_t  fileSize(atoi(fileInfo.substr(pos+1).c_str()));
//	std::cout << "start recv " <<fileName <<std::endl;
//	FILE *fp = nullptr;
//	fp = fopen(fileName.c_str(),"w+");
//	if( fp == nullptr)
//	{
//		std::cout <<"open file failed"<<std::endl;
//		return ;
//	}
	pNWman->doReadFile(this->curPathLocal+this->curPathRelaFolder+fileName,fileSize);
	std::cout << "recv over :" << curPathLocal+fileName<<std::endl;
}

int priNetDiskClientTer::order_upload_file(const std::string &fileName)
{
	const off_t &fileSize = getFileSize(this->curPathLocal+fileName);
	this->talkToServer(H_UPLOADFILE + "#" + this->curPathPriDisk + fileName  +":" + std::to_string(fileSize));
	return this->pNWman->doWriteFile(this->curPathLocal+fileName,fileSize);
}

int priNetDiskClientTer::order_show_file(const std::string &filePath)
{
	int ret = -1;
	if( filePath.empty() )
		ret = this->talkToServer( H_SHOWFILES + "#"+ this->curPathPriDisk);
	else
	{
		if(filePath.back() != '/')
			return -1;
		if( filePath[0] == '/' ) //abs path
			ret = this->talkToServer( H_SHOWFILES + "#" + filePath);
		else					 //rela path
			ret = this->talkToServer( H_SHOWFILES + "#" + this->curPathPriDisk + filePath);
	}
	return ret;
}

int priNetDiskClientTer::order_download_file(const std::string &fileName)
{
	if(fileName.empty())
		return -1;
	return this->talkToServer(H_DOWNLOADFILE+ "#" + this->curPathPriDisk + fileName);
}

int priNetDiskClientTer::order_download_folder(const std::string &folderName)
{
	if( folderName.empty() )
		return -1;
	/*mkdir named folderName*/
	int ret = mkdir((this->curPathLocal+this->curPathRelaFolder+folderName).c_str(),
					S_IRUSR | S_IWUSR | S_IXUSR|    \
					S_IRGRP | S_IXGRP |				\
					S_IROTH | S_IXOTH );
	if( ret < 0 )
		return -1;
	/* End */
	this->curPathRelaFolder.append(folderName+"/");
	return this->talkToServer(H_DOWNLOADFOLDER+"#"+this->curPathPriDisk+this->curPathRelaFolder);
}

int priNetDiskClientTer::order_create_folder(const std::string &folderName)
{
	int ret = this->talkToServer(H_CREATEFOLDER+"#"+this->curPathPriDisk+folderName);			//send "createFolder"  order
	if( ret < 0 )
		return -1;
}

int priNetDiskClientTer::order_upload_folder(const std::string &folderName)
{
	if( folderName.empty() )
		return -1;
	int ret = -1;
//	int ret = this->talkToServer(H_CREATEFOLDER+"#"+this->curPathPriDisk+folderName);			//send "createFolder"  order
//	if( ret < 0 )
//		return -1;
	this->order_create_folder(folderName);
	/*开始扫描文件夹，进行递归传输*/
	//使用广度搜索算法进行扫描
	struct dirent **nameList = nullptr;
	std::queue<std::string> WSFolders;		//Wait for Scan Folders
	ret = scandir((this->curPathLocal+folderName).c_str() , &nameList,NULL,alphasort);
	if( ret < 0 )
	{
		std::cout << "Scan Dir error\n";
		return -1;
	}
	else
	{
		while( ret-- )
		{
			if(strcmp(nameList[ret]->d_name,".") == 0 ||
			   strcmp(nameList[ret]->d_name,"..")== 0 )
				continue;
			if( nameList[ret]->d_type == DIRE_NUM )
			{
				WSFolders.push(nameList[ret]->d_name);
				continue;
			}
			this->order_upload_file(folderName+"/"+nameList[ret]->d_name);
		}
		while( !WSFolders.empty() )
		{
			this->order_upload_folder(folderName+"/"+WSFolders.front());
			WSFolders.pop();
		}
	}
	return 1;
}

int priNetDiskClientTer::order_remove_files(const std::vector<std::string> &fileName)
{
	std::string orderContent(H_REMOVEFILES+"#" + this->curPathPriDisk);
	if( !fileName.empty() )
	{
		orderContent.push_back(':');
		for( auto x : fileName)
		{
			orderContent.append(x);
			orderContent.push_back(':');
		}
		int ret = this->talkToServer(orderContent);
		return ret;
	}
	else
		return -1;
}
