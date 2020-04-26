#include "prinetdiskclient.h"

#include <iostream>
#include <string>

priNetDiskClient::priNetDiskClient(/*const std::string &AccPW*/)
	:/*myPassPort(AccPW),*/serverSignal(false),localPort(0)
{

}

int priNetDiskClient::startClient(const std::string &destIP, const unsigned short destPort , const std::string passPort)
{
	pNWman = std::make_shared<NWmanagerUDTClient>();
//	pNWman->bindLocal(this->localPort);
	pNWman->dealMsg = std::bind(&priNetDiskClient::onMsgDeal ,this ,
								std::placeholders::_1,std::placeholders::_2);
	int ret = pNWman->doStart(destIP,destPort);
	if( ret == -1)
	{
//		std::cout <<" connect refuse " <<std::endl;
		this->errMes.assign("connect server failed");
		pNWman.reset();
		return -1;
	}
	this->localPort = ret;
	if( talkToServer("login#"+passPort) == -2 )
		return -1;
	return 1;
}

int priNetDiskClient::talkToServer(const std::string &Msg)
{
	pNWman->setMsg(Msg);
	pNWman->doWrite();
	return pNWman->doRead();
}

const std::string priNetDiskClient::getErrorMessage()
{
	return this->errMes;
}

int priNetDiskClient::onMsgDeal(const std::string &msgHead, const std::string &msgBody)
{
	if( 0 == msgBody.substr(0,3).compare(ORDER_CONTENT_HEAD_ERR))
	{
		std::cout << msgBody.substr(3)<<std::endl;
		this->errMes.assign(msgBody.substr(3));
		return -2;
	}
	else
	{
		if( dealMsgFunc.find(msgHead) == dealMsgFunc.end())	//对象消息处理函数未找到
		{
			this->errMes.assign("recv message ERROR");
			return -1;
		}
		this->dealMsgFunc[msgHead](msgBody.substr(3));//调用消息处理函数
		return 1;
	}

}
