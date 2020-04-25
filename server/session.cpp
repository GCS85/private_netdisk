#include "session.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "nwmanagerudt.h"

//#define Test
#ifdef Test
#include <iomanip>
static void displayBit( const std::string &temp)
{
	for( u_char x : temp)
	{
		std::cout << std::hex			<<
					 std::setw(2)		<<
					 std::setfill('0')	<<
					 (int)x;
	}
	std::cout << std::endl;
}
#endif

static off64_t getFileSize(const char* filePath)
{
	struct stat fileInfo;
	bzero(&fileInfo,sizeof( fileInfo));
	int ret = stat(filePath,&fileInfo);
	if( ret < 0)
	{
		return -1;
	}
	return fileInfo.st_size;
}

session::session(const UDTSOCKET &clientsock ,
				 const std::function<int (const std::string &,
										   const std::string &,
										   const int &, std::string &)> &tempDealMsg,
				 std::shared_ptr<rsa_encrypt> temPtr):
	clientSock(clientsock),
	rootPath("/home/libo"),
	headOver(false),
	haveRead(0),fileHaveSend(0),bodyLen(0),
	dealMsg(tempDealMsg),
	SPLCHAR('#'),
	login(false),
	pRsaEncrypt(temPtr)
{
	memset(msg,0,MSGBODYLEN);
}
session::~session()
{
	UDT::close(clientSock);
	std::cout << "destory session " <<std::endl;
}

int session::splitMsg()
{
	std::string result;
	int ret;
	auto itor = std::find(msg,msg+bodyLen,SPLCHAR);
	if( itor - msg == bodyLen)
	{
		result.assign("MSG_NOCOM");
		ret = NWmanagerUDT::NOT_DELETE;
	}
	else
	{
		ret = dealMsg(std::string(msg,itor),std::string(itor+1),this->clientSock , result);
		result.insert( 0 ,msg, itor - msg + 1);		//添加返回消息头部
	}
	if( NWmanagerUDT::DELETE_IMME != ret )
		this->workMsg(result);
	return ret;
}

int session::workMsg(std::string &mess)
{
	/*ENCRYPT*/
	std::cout << "msg send :" << mess <<std::endl;
	std::string returnResult(this->pRsaEncrypt->encrypt_from_memory(mess , this->clientPubKey));
#ifdef Test
	std::cout << "EN Info : \n";
	displayBit(returnResult);
#endif
	/* END */
	addMsgHead(returnResult);
	memcpy(msg , returnResult.c_str() , returnResult.length());
	this->cipherLength = returnResult.length();
	this->msg[ returnResult.length() ] ='\0';
}

bool session::setFileInfo(const std::string &temFilePath, const off64_t &temFileSize )
{
	if( !(temFilePath.empty()))
	{
		this->filePath.assign(this->rootPath+temFilePath);		//abs path
	}
	else
		return false;
	if( -1 == temFileSize )
	{
		struct stat statBuf;
		if( -1 == stat(this->filePath.c_str(),&statBuf))
			return false;
		fileSize = statBuf.st_size;
	}
	else
		this->fileSize = temFileSize;
	return true;
}

void session::addMsgHead(std::string &result)
{
	std::string Head = std::to_string(result.length());
	while( Head.length() != MSGHEADLEN)
		Head.insert(Head.begin(),'0');
	result.insert(0,Head);
}
