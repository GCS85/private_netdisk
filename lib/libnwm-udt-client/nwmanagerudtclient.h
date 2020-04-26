#ifndef NWMANAGERUDTCLIENT_H
#define NWMANAGERUDTCLIENT_H

#include "udt.h"
#include <string>
#include <functional>
#include <future>

#include "encrypt.h"

class NWmanagerUDTClient
{
public:
	enum
	{
		MSGHEADLEN = 4,
		MSGBODYLEN = 2048,
		ECONNREJ = 1004
	};
	NWmanagerUDTClient();
	~NWmanagerUDTClient();

	int doStart(const std::string & , const unsigned short &);
	int doRead();
	int doReadFile(const std::string &filePath,const int64_t &fileSize);
	int doWrite();
	int doWriteFile(const std::string &filePath , const int64_t &fileSize);
	int doStop();
	int sendHolePacket(const std::string &OclientIP , const unsigned short &OclientPort,
						const unsigned &localPort);
	void setMsg(std::string );
	void bindLocal(const unsigned short&);

	std::function<int(const std::string &msgHead ,
					   const std::string &msgBody)> dealMsg;
private:
	std::vector<std::future<int>> asyncTask;
	UDTSOCKET localSock;
	const char SPLCHAR;
	/*session 包含内容*/
	char msg[MSGBODYLEN];
	int cipherLength;
	int bodyLen;
	int haveRead;
	bool headOver;			//标识读取head 是否已经读完
	rsa_encrypt rsaEncrypt;
	std::string serverPubKey;
	/*member function*/
	int splitMsg();
	void addMsgHead(std::string &Msg);
	int doReadPubKey();
	int doWritePubKey();
};

#endif // NWMANAGERUDTCLIENT_H
