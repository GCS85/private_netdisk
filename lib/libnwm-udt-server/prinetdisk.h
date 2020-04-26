#ifndef PRINETDISK_H
#define PRINETDISK_H

#include <map>
#include <memory>
#include <unistd.h>
#include <functional>
#include <mysql/mysql.h>

#include "nwmanagerudt.h"

#define _STR_MHOST      "localhost"
#define _STR_MUSER      "root"
#define _STR_MPASSWD    "00000000"
#define _STR_MDB		"priDiskServerTest"
//#define _STR_MDB        "priDiskServer"

#define ORDER_CONTENT_HEAD_ERR "ERR"
#define ORDER_CONTENT_HEAD_SUC "SUC"

#define DIRE_NUM 0x04
#define FILE_NUM 0x08

const std::string H_SEARCHCLIENT	("searchClient");
const std::string H_CLOSESESSION	("closeSession");
const std::string H_LOGIN			("login");
const std::string H_SHOWFILES		("showFiles");
const std::string H_DOWNLOADFILE	("downloadFile");
const std::string H_UPLOADFILE		("uploadFile");
const std::string H_REMOVEFILES		("removeFiles");
const std::string H_CREATEFOLDER	("createFolder");
const std::string H_DOWNLOADFOLDER	("downloadFolder");
class priNetdisk
{
public:
	enum{
		TCP = SOCK_STREAM,
		UDP = SOCK_DGRAM
	};
	priNetdisk(const unsigned short &port , const bool &verifyModel = true);
	void onInit();	//初始化，包括网络NWmanger
	void doLoop();
	~priNetdisk();

	int onMsgDeal(const std ::string &msgHead,
				  const std::string &msgBody,
				  const UDTSOCKET &clientSock,
				  std::string &dealResult);

private:
	const bool verifySwitch;
	/* CallBack function */
	void onAccept(const UDTSOCKET &clientSock , const std::shared_ptr<session> pSession);
	void onDelete(const UDTSOCKET &clientSock);
	std::shared_ptr<session> onSearch(const UDTSOCKET &clientSock);
	/*消息处理函数*/
	int verifyUser		(const std::string &Account_PassWord ,	const UDTSOCKET &clientSock, std::string &result);
	int searchClient	(const std::string &otherClientName,	const UDTSOCKET &clientSock, std::string &result);
	int uploadFile		(const std::string &filePath,			const UDTSOCKET &clientSock, std::string &result);
	int downloadFile	(const std::string &otherClientName,	const UDTSOCKET &clientSock, std::string &result);
	int createFolder	(const std::string &,					const UDTSOCKET &clientSock, std::string &result);
	int downloadFolder	(const std::string &,					const UDTSOCKET &clientSock, std::string &result);
	int showFiles		(const std::string &,					const UDTSOCKET &clientSock, std::string &result);
	int removeFiles		(const std::string &,					const UDTSOCKET &clientSock, std::string &result);
	int closeSession	(const std::string &,					const UDTSOCKET &clientSock, std::string &result);
	/*common function*/
	bool verifyClientLimit(const std::string &userName, const std::string &deviceName);
	/* END */
	NWmanagerUDT netWork;
	MYSQL *mysqlCon;
//	const int epSize;
	std::map<UDTSOCKET,std::shared_ptr<session>> clientMap;		//根据UDTSOCKET进行查找
	std::map<std::string , UDTSOCKET > clientMemu;				//根据客户端名字进行查找
	std::map<std::string, std::function<int (const std::string&,
											  const UDTSOCKET&,
											  std::string& )>> dealMsgFunc;
};
#endif // PRINETDISK_H
