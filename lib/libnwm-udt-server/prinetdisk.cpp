#include "prinetdisk.h"

#include <functional>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include <string>
#include <iostream>

static std::string change_size_unit(off64_t fileSize)
{
	if( fileSize < 0)
		return "";
	std::string resultString;
	off64_t result = 0;
	if( fileSize < 1024 )
		result = fileSize;
	int i = 0;
	while( fileSize >= 1024 )
	{
		result = fileSize % 1024;
		fileSize /= 1024;
		i++;
	}
	resultString.assign(std::to_string(fileSize)+ "." + std::to_string((result*1000)/1024));
	switch (i) {
	case 0:
		resultString.append("B");
		break;
	case 1:
		resultString.append("KB");
		break;
	case 2:
		resultString.append("MB");
		break;
	case 3:
		resultString.append("GB");
		break;
	default:
		break;
	}
	return resultString;
}

static int remove_file_dir(std::string path )
{
	struct stat statBuf;
	struct dirent **nameList;
	int n = -1;
	if( stat(path.c_str(),&statBuf) != -1 )
	{
		if( S_ISDIR(statBuf.st_mode) )		//文件夹 进入递归删除
		{
			if( path.back() != '/')
				path.push_back('/');
			n = scandir(path.c_str(),&nameList,NULL,alphasort);
			if( 2 != n )		// . and ..		dir is empty
			{
				while(n--)
				{
					if( strcmp(nameList[n]->d_name , "." )== 0 ||
						strcmp(nameList[n]->d_name , "..")== 0 )
						continue;
					if( nameList[n]->d_type == 0x04 )
						remove_file_dir(path+nameList[n]->d_name);
					else
					{
						if( -1 == ::remove((path+nameList[n]->d_name).c_str()))
							perror("1Del:");
					}
					free(nameList[n]);
				}
			}
			free(nameList);
			::remove(path.c_str());
		}
		else
		{
			if( -1 == ::remove(path.c_str()))
				perror("2Del");
		}
	}
	return 0;
}

int RE_alphasort(const struct dirent **a, const struct dirent **b)
{
	return -(alphasort(a,b));
}

priNetdisk::priNetdisk(const unsigned short &port, const bool &verifyModel):
	netWork(),verifySwitch(verifyModel)
{
	netWork.bindLocal(port);
	if( true == verifySwitch )
	{
		mysqlCon = mysql_init(NULL);
		if( !mysql_real_connect(mysqlCon,_STR_MHOST ,_STR_MUSER , _STR_MPASSWD,_STR_MDB,0,NULL,0))
		{
			throw std::runtime_error("database connect error");
		}
	}
}

priNetdisk::~priNetdisk()
{
	for( auto &te : clientMap)
	{
		UDT::close( te.first );
	}
	if( true == verifySwitch )
	{
		mysql_close(mysqlCon);
	}
}

void priNetdisk::doLoop()
{
	netWork.doStart();
}

void priNetdisk::onAccept(const UDTSOCKET &clientSock , const std::shared_ptr<session> pSession)
{
	clientMap[clientSock] = pSession;
}

void priNetdisk::onDelete(const UDTSOCKET &clientSock)
{
	if( ! clientMap[clientSock]->clientName.empty() )
		clientMemu.erase(clientMap[clientSock]->clientName);

	clientMap.erase(clientSock);
	//	if( clientMap.empty() )
	//		netWork.doStop();
}

std::shared_ptr<session> priNetdisk::onSearch(const UDTSOCKET &clientSock)
{
	return clientMap[clientSock];
}

/* 进行回调函数注册 */
void priNetdisk::onInit()
{
	/*网络模块回调函数注册*/
	netWork.AcceptCallBack = std::bind(&priNetdisk::onAccept,this,
									   std::placeholders::_1 ,
									   std::placeholders::_2);

	netWork.DeletCallBack = std::bind(&priNetdisk::onDelete,this,std::placeholders::_1);
	netWork.searchSession = std::bind(&priNetdisk::onSearch,this,std::placeholders::_1);
	netWork.dealMsg = std::bind(&priNetdisk::onMsgDeal,this,
								std::placeholders::_1,
								std::placeholders::_2,
								std::placeholders::_3,
								std::placeholders::_4);
	/* 消息处理函数添加 */

	dealMsgFunc[H_SEARCHCLIENT] = std::bind(&priNetdisk::searchClient,this,
											std::placeholders::_1 ,
											std::placeholders::_2 ,
											std::placeholders::_3 );
	dealMsgFunc[H_UPLOADFILE] = std::bind(&priNetdisk::uploadFile,this,
										  std::placeholders::_1 ,
										  std::placeholders::_2 ,
										  std::placeholders::_3 );


	dealMsgFunc[H_DOWNLOADFILE] = std::bind(&priNetdisk::downloadFile,this,
											std::placeholders::_1 ,
											std::placeholders::_2 ,
											std::placeholders::_3 );
	dealMsgFunc[H_SHOWFILES] = std::bind(&priNetdisk::showFiles,this,
										 std::placeholders::_1 ,
										 std::placeholders::_2 ,
										 std::placeholders::_3 );
	dealMsgFunc[H_REMOVEFILES] = std::bind(&priNetdisk::removeFiles,this,
										 std::placeholders::_1 ,
										 std::placeholders::_2 ,
										 std::placeholders::_3 );
	dealMsgFunc[H_CLOSESESSION] = std::bind(&priNetdisk::closeSession,this,
											std::placeholders::_1 ,
											std::placeholders::_2 ,
											std::placeholders::_3 );

	dealMsgFunc[H_CREATEFOLDER] = std::bind(&priNetdisk::createFolder,this,
											std::placeholders::_1 ,
											std::placeholders::_2 ,
											std::placeholders::_3 );

	dealMsgFunc[H_DOWNLOADFOLDER] = std::bind(&priNetdisk::downloadFolder,this,
											std::placeholders::_1 ,
											std::placeholders::_2 ,
											std::placeholders::_3 );

}

int priNetdisk::onMsgDeal(const std::string &msgHead ,
						  const std::string &msgBody,
						  const UDTSOCKET &clientSock,
						  std::string &dealResult)
{
	if( msgHead.compare(H_LOGIN) == 0)
	{
		return this->verifyUser(msgBody,clientSock,dealResult);
	}
	if( dealMsgFunc.find(msgHead) == dealMsgFunc.end())	//对象消息处理函数未找到
	{
		dealResult.assign(ORDER_CONTENT_HEAD_ERR);
		dealResult.append("DFUN_NOFUN");
		return NWmanagerUDT::NOT_DELETE;
	}

	if( this->verifySwitch )
	{
		if(clientMap[clientSock]->login)
			return this->dealMsgFunc[msgHead](msgBody ,clientSock, dealResult);//调用消息处理函数
		else
		{
			dealResult.assign(ORDER_CONTENT_HEAD_ERR);
			dealResult.append("NOT_LOGIN");
			return NWmanagerUDT::DELETE_DELAY;
		}
	}
	else
	{
		return this->dealMsgFunc[msgHead](msgBody , clientSock , dealResult);
	}
}

/*消息处理函数*/
//1.用户账号密码验证 && 设置客户端名称
int priNetdisk::verifyUser	(const std::string &Account_PassWord, const UDTSOCKET &clientSock, std::string &result)
{
	std::shared_ptr<session> temPtr(clientMap[clientSock]);
	int retVal = -1;

	std::string tableName;
	/** 根据 Account_PassWord第一个字母判断客户端身份 **
	 * first letter : C  --> client
	 *				  I	 --> PI
	 * */
	if( Account_PassWord.front() == 'C' )
		tableName.assign("user");
	else if( Account_PassWord.front() == 'I')
		tableName.assign("device");
	else
	{
		result.assign(ORDER_CONTENT_HEAD_ERR);
		result.append("MES_ERROR");
		return NWmanagerUDT::NOT_DELETE;
	}


	/*Mysql select */
	if( false == temPtr->login )
	{
		size_t pos = Account_PassWord.find(':');
		if( pos != std::string::npos )
		{
			const std::string &userAccount(Account_PassWord.substr(1,pos-1));
//			const std::string &userPassWord(Account_PassWord.substr(pos+1));
			const std::string &userPW_MD5(MD5_32(Account_PassWord.substr(pos+1)));

			std::string querySQL("select "+tableName+"PassWord from "+ tableName +" where "+tableName
								 +"Account = '"+ userAccount+"'");
			mysql_query(mysqlCon , querySQL.c_str());
			MYSQL_RES *queryResult = mysql_store_result(mysqlCon);
			unsigned int rows = mysql_num_rows(queryResult);
			if( rows != 0  )
			{
				MYSQL_ROW row = mysql_fetch_row(queryResult);
//				std::cout << row[0]<<std::endl;
//				std::cout << userPW_MD5<< std::endl;
				if( 0 == userPW_MD5.compare(row[0]))
				{

					//set clientName
					if( clientMap.find(clientSock) == clientMap.end())		//对应客户端socket未找到
					{
						result.assign(ORDER_CONTENT_HEAD_ERR);
						result.append("SESSION_NOT_FOUND");
						retVal = NWmanagerUDT::DELETE_DELAY;
						goto FREE_OUT;
					}
					else
					{	temPtr->login =true;
						clientMap[clientSock]->clientName = userAccount;
						clientMemu[userAccount] = clientSock;
					}
					/* End */
					result.assign(ORDER_CONTENT_HEAD_SUC);
					result.append("SUCCESS_LOGIN");
					//					return NWmanagerUDT::NOT_DELETE;
					retVal = NWmanagerUDT::NOT_DELETE;
					goto FREE_OUT;
				}
			}
			result.assign(ORDER_CONTENT_HEAD_ERR);
			result.append("ACCOUNT_OR_PW_ERROR");
			retVal = NWmanagerUDT::DELETE_DELAY;
FREE_OUT:
			mysql_free_result(queryResult);
			return retVal;
		}
		else
		{
			/* message format ERROR */
			result.assign(ORDER_CONTENT_HEAD_ERR);
			result.append("MES_ERROR");
			return NWmanagerUDT::NOT_DELETE;
		}
	}
	else
	{
		result.assign(ORDER_CONTENT_HEAD_ERR);
		result.append("HAVE_LOGIN");
		return NWmanagerUDT::NOT_DELETE;
	}
	/* End */

}
//2.查找客户端IP+PORT
int priNetdisk::searchClient(const std::string &otherClientName, const UDTSOCKET &clientSock, std::string &result)
{
	if( clientMemu.find(otherClientName) == clientMemu.end())
	{
		result.assign(ORDER_CONTENT_HEAD_ERR+otherClientName+" NOT_FOUND");
		return NWmanagerUDT::DELETE_DELAY;
	}
	/*主动客户端权限验证*/
	if( !(verifyClientLimit(this->clientMap[clientSock]->clientName,otherClientName)) )
	{
		result.assign(ORDER_CONTENT_HEAD_ERR+otherClientName+" NOT_PERMIT");
		return NWmanagerUDT::DELETE_DELAY;
	}

	/*给主动客户端查找  被动客户端*/
	UDTSOCKET resultSock = clientMemu[otherClientName];
	sockaddr_in otherClientAddr;
	int addrLen = sizeof( sockaddr );
	if(UDT::getpeername(resultSock,(sockaddr*)&otherClientAddr,&addrLen) < 0)
	{
		result.assign(ORDER_CONTENT_HEAD_ERR+otherClientName+" ERROR");
		return NWmanagerUDT::DELETE_DELAY;
	}

	result.assign(ORDER_CONTENT_HEAD_SUC);
	result.append(inet_ntoa(otherClientAddr.sin_addr));
	result.push_back(':');
	result.append(std::to_string(ntohs(otherClientAddr.sin_port)));

	/*给被动客户端发送主动客户端信息*/
	sockaddr_in  clientAddr;
	std::string  clientMsg;
	clientMsg.assign(H_SEARCHCLIENT + "#");
	if(UDT::getpeername(clientSock,(sockaddr*)&clientAddr,&addrLen) < 0)
	{	clientMsg.assign(ORDER_CONTENT_HEAD_ERR);
		clientMsg.append("CLIENT IP&PORT");
	}
	else
	{
		clientMsg.append(ORDER_CONTENT_HEAD_SUC);
		clientMsg.append(inet_ntoa(clientAddr.sin_addr));
		clientMsg.push_back(':');
		clientMsg.append(std::to_string(ntohs(clientAddr.sin_port)));
	}
	clientMap[resultSock]->workMsg(clientMsg);
	netWork.doWrite(resultSock);
	return NWmanagerUDT::NOT_DELETE;
}
//3.上传文件---存储接受文件
int priNetdisk::uploadFile	(const std::string &fileInfo,const UDTSOCKET &clientSock, std::string &result)
{
	std::size_t pos;
	if( (pos = fileInfo.find(':')) != std::string::npos )
	{
		const std::string &filePath(fileInfo.substr(0,pos));
		const off64_t &fileSize(atol(fileInfo.substr(pos+1).c_str()));

		std::shared_ptr<session> temPtr(clientMap[clientSock]);
		if( temPtr->setFileInfo(filePath,fileSize) )
		{
			netWork.doRecvFile(clientSock);			//async task
		}
		result.assign(ORDER_CONTENT_HEAD_SUC);
		result.append("READY_RECV");
	}
	else
	{
		result.assign(ORDER_CONTENT_HEAD_ERR);
		result.append("FILE_INFO_ERROR");
	}
	return NWmanagerUDT::NOT_DELETE;
}
//4.下载文件---存储传输文件
int priNetdisk::downloadFile(const std::string &filePath, const UDTSOCKET &clientSock, std::string &result)
{
	std::shared_ptr<session> temPtr(clientMap[clientSock]);
	int pos = filePath.find_last_of('/');
	const std::string &fileName(filePath.substr(pos+1));
	if( temPtr->setFileInfo(filePath) )		//发送完毕后文件描述符自动关闭
	{
		result.assign(ORDER_CONTENT_HEAD_SUC);
		result.append(fileName+":"+std::to_string(temPtr->fileSize));
		netWork.doSendFile(clientSock);
	}
	/***
	* TODO
	* 服务端打开文件失败,返回消息格式需要作定义,并在客户端进行相应的处理
	*/
	else
	{
		result.assign(ORDER_CONTENT_HEAD_ERR);
		result.append("FILE_OPEN_ERROR");
	}
	return NWmanagerUDT::NOT_DELETE;
}
//5.创建文件夹指令
int priNetdisk::createFolder(const std::string &dirPath, const UDTSOCKET &clientSock, std::string &result)
{
	//1.判断 folderPath 是否存在，并返回相应 信息
	if(dirPath.empty())
	{
		result.assign(ORDER_CONTENT_HEAD_ERR);
		result.append("DIR_PATH");
		return NWmanagerUDT::NOT_DELETE;
	}
	std::shared_ptr<session> temPtr(clientMap[clientSock]);
	const std::string &curPath(temPtr->rootPath+dirPath);
	DIR *curDir = opendir(curPath.c_str());
	if( curDir == nullptr)		//DIR  do not exists
	{
		/*create folder*/
		int ret = -1;
		ret = mkdir(curPath.c_str(),
					S_IRUSR | S_IWUSR | S_IXUSR|    \
					S_IRGRP | S_IXGRP |				\
					S_IROTH | S_IXOTH );
		if( ret < 0 )
		{
			result.assign(ORDER_CONTENT_HEAD_ERR);
			result.append("DIR_PATH");
		}
		else
		{
			result.assign(ORDER_CONTENT_HEAD_SUC);
			result.append("CREATED");
		}
	}
	else				// DIR exists
	{
		result.assign(ORDER_CONTENT_HEAD_SUC);
		result.append("EXIST");
	}
	return NWmanagerUDT::NOT_DELETE;
}
//6.下载文件夹指令
int priNetdisk::downloadFolder(const std::string &dirPath, const UDTSOCKET &clientSock, std::string &result)
{
	std::shared_ptr<session> temPtr(clientMap[clientSock]);
	struct dirent **nameList;
	int n;
	const std::string curPath(temPtr->rootPath+dirPath);
	n = scandir(curPath.c_str() ,&nameList,nullptr ,RE_alphasort);
	if(n < 0 )
	{
		result.assign(ORDER_CONTENT_HEAD_ERR);
		result.append("DIR_PATH");
	}
	else
	{
		result.append(ORDER_CONTENT_HEAD_SUC);
		while(n--)
		{
			if( 0 == strcmp(nameList[n]->d_name , "."))
				continue;
			if( 0 == strcmp(nameList[n]->d_name ,".."))
				continue;
			result.append(nameList[n]->d_name);
			result.push_back(':');
			result.push_back(nameList[n]->d_type);		//Directory: d_type == 0x04		File:d_type == 0x08
			result.push_back('&');
			free(nameList[n]);
		}
		free(nameList);
	}
	return NWmanagerUDT::NOT_DELETE;
}
//7.传输当前文件夹下所有文件名
int priNetdisk::showFiles	(const std::string &dirPath, const UDTSOCKET &clientSock, std::string &result)
{
	std::shared_ptr<session> temPtr(clientMap[clientSock]);
	struct dirent **nameList;
	int n;
	const std::string curPath(temPtr->rootPath+dirPath);
	struct stat statBuf;
	n = scandir(curPath.c_str() ,&nameList,nullptr ,RE_alphasort);
	if(n < 0 )
	{
		result.assign(ORDER_CONTENT_HEAD_ERR);
		result.append("DIR_PATH");
	}
	else
	{
		result.append(ORDER_CONTENT_HEAD_SUC);
		while(n--)
		{
			if( 0 == strcmp(nameList[n]->d_name , "."))
				continue;
			if( 0 == strcmp(nameList[n]->d_name ,".."))
				continue;
			result.append(nameList[n]->d_name);
			result.push_back(':');
			result.push_back(nameList[n]->d_type);		//Directory: d_type == 0x04		File:d_type == 0x08
			result.push_back(':');
			if(nameList[n]->d_type == DIRE_NUM)
			{
				result.push_back('0');
			}
			else
			{
				if( -1 != stat((curPath+nameList[n]->d_name).c_str(),&statBuf))
				{
					result.append(change_size_unit(statBuf.st_size));
				}
				else
					result.push_back('0');
			}
			result.push_back('&');
			free(nameList[n]);
		}
		free(nameList);
	}
	return NWmanagerUDT::NOT_DELETE;
}
//8.删除文件	dirPathFileNames = dirPath:fileName1:fileName2...
int priNetdisk::removeFiles	(const std::string &dirPathFileNames, const UDTSOCKET &clientSock, std::string &result)
{
	if( !dirPathFileNames.empty() )
	{
		std::shared_ptr<session> temPtr(clientMap[clientSock]);
		int posFir = dirPathFileNames.find(':') , posSec = 0;
		if( posFir == std::string::npos )
			goto ERROR_OUT;
		std::string dirPath(temPtr->rootPath+dirPathFileNames.substr(0,posFir));
		if( dirPath.back() != '/')
			dirPath.push_back('/');
		while( posFir != std::string::npos )
		{
			posSec = dirPathFileNames.find(':' , posFir+1 );
			if(posSec == std::string::npos)
				goto ERROR_OUT;
			const std::string &fileName(dirPathFileNames.substr(posFir+1 , posSec-posFir-1));
			if( -1 == remove_file_dir(dirPath+fileName))
				goto ERROR_OUT;
			if( dirPathFileNames.length()-1 == posSec)
				break;
			posFir = posSec;
		}
		result.assign(ORDER_CONTENT_HEAD_SUC);
		result.append("SUCCESS_REMOVE");
		return NWmanagerUDT::NOT_DELETE;
	}

ERROR_OUT:
	result.assign(ORDER_CONTENT_HEAD_ERR);
	result.append("REMOVE_FILE_ERROR");
	return NWmanagerUDT::NOT_DELETE;
}
//9.关闭当前Socket
int priNetdisk::closeSession(const std::string &, const UDTSOCKET &clientSock, std::string &result)
{
	if( false == this->verifySwitch )
		netWork.doStop();
	return NWmanagerUDT::DELETE_IMME;
}
//验证客户端是否有权限访问存储设备
bool priNetdisk::verifyClientLimit(const std::string &userName , const std::string &deviceName)
{
	bool ret = false;
	std::string querySQL("select deviceAccount from device where deviceID in ( \
						 select bind.deviceID from bind left outer join user on  \
						 bind.userID=user.userID where user.userAccount='"+userName+"')" );

	mysql_query(mysqlCon , querySQL.c_str());
	MYSQL_RES *queryResult = mysql_store_result(mysqlCon);
	if( queryResult == nullptr)
		return false;
	unsigned int rows = mysql_num_rows(queryResult);
	if( rows != 0  )
	{
		for( int i = 0 ; i < rows ; ++i )
		{
			MYSQL_ROW row = mysql_fetch_row(queryResult);
			if( 0 == (deviceName.compare(row[0])))
			{
				ret = true;
				goto OUT;
			}
		}
	}
OUT:
	mysql_free_result(queryResult);
	return ret;
}
/*END*/























