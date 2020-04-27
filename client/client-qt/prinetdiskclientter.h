#ifndef PRINETDISKCLIENTTER_H
#define PRINETDISKCLIENTTER_H

#include "prinetdiskclient.h"

#include <vector>
#include <string>

#ifndef DIRE_NUM
#define DIRE_NUM 0x04
#define FILE_NUM 0x08
#endif

const std::string H_SEARCHCLIENT	("searchClient");
const std::string H_SHOWFILES		("showFiles");
const std::string H_DOWNLOADFILE	("downloadFile");
const std::string H_UPLOADFILE		("uploadFile");
const std::string H_REMOVEFILES		("removeFiles");
const std::string H_CREATEFOLDER	("createFolder");
const std::string H_DOWNLOADFOLDER	("downloadFolder");
struct fileInfo
{
	std::string fileName;
	std::string fileSize;
	char fileType;
};
class priNetDiskClientTer: public priNetDiskClient
{
public:
	priNetDiskClientTer(/*const std::string &myPassPort,*/
						const std::string &rootPath,
						const int& protoType = priNetDiskClient::TCP);
	void startInterActive();
	int connectNetDisk(const std::string &netDiskName);			//连接网盘
	int disconnectNetDisk();
	void showMemu();
	void initClient();

	int order_show_file(const std::string &filePath = "");
	int order_upload_file(const std::string &fileName);			//just fileName not include filePath
	int order_download_file(const std::string &fileName);
	int order_upload_folder(const std::string &folderName);
	int order_download_folder(const std::string &folderName);
	int order_create_folder(const std::string &folderName);
	int order_remove_files(const std::vector<std::string> &fileName);			// include folder and file
	bool connected;
	std::string curPathLocal;
	std::string curPathPriDisk;
	std::vector<struct fileInfo> curDirNoHDContent;
	std::vector<struct fileInfo> curDirHiddenContent;
private:
	/*消息处理函数*/
	void searchClient(const std::string &);
	void uploadFile(const std::string &);
	void downloadFile(const std::string &);		//开始接收文件
	void showFiles(const std::string &);
	void removeFiles(const std::string &result);
	void createFolder(const std::string &);
	void downloadFolder(const std::string &);

private:
	std::string curPathRelaFolder;				//当前相对接受文件路径

};

#endif // PRINETDISKCLIENTTER_H
