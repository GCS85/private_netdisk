#ifndef PRINETDISKCLIENT_H
#define PRINETDISKCLIENT_H

#include <string>
#include <functional>
#include <map>
#include <memory>
#include "nwmanagerudtclient.h"

#define ORDER_CONTENT_HEAD_ERR "ERR"
#define ORDER_CONTENT_HEAD_SUC "SUC"


class priNetDiskClient
{
public:
	enum
	{
		TCP = SOCK_STREAM,
		UDP = SOCK_DGRAM
	};
	priNetDiskClient(/*const std::string &clientName */);
	int startClient(const std::string &destIP, const unsigned short destPort, const std::string passPort);			// connect  the server
	int onMsgDeal(const std::string & , const std::string &);
	int talkToServer(const std::string &);
	const std::string getErrorMessage();
protected:
	//网络层回调函数
	virtual void initClient() = 0;
	std::string errMes;

public:
	std::string OclientIP;
	unsigned short OclientPort;
	unsigned short localPort;
protected:
	std::shared_ptr<NWmanagerUDTClient> pNWman;
	std::map<std::string, std::function<void (const std::string&)>> dealMsgFunc;
	bool serverSignal;
//private:
//	const std::string myPassPort;
};

#endif // PRINETDISKCLIENT_H
