#ifndef SESSION_H
#define SESSION_H

#include <arpa/inet.h>
#include <memory>
#include <functional>
#include "udt.h"
#include "encrypt.h"

class session
{
public:
	enum{
		MSGHEADLEN = 4,
		MSGBODYLEN = 2048
	};

	session(const UDTSOCKET & ,/* const sockaddr_in &,*/
			const std::function<int (const std::string &msgHead ,
									 const std::string &msgBody,
									 const UDTSOCKET &clientSock,
									 std::string &dealResult)>& ,
			std::shared_ptr<rsa_encrypt> temPtr);
	~session();

	char* getMsg()
	{
		return msg;
	}
	void	addMsgHead(std::string&);
	bool	setFileInfo(const std::string& temFileName, const off64_t& temFileSize = -1 );
	int		splitMsg();
	int		workMsg(std::string &mess);
	/*member */
	const char SPLCHAR;
	bool login;
	std::string clientName;
	std::function<int(const std::string &msgHead ,
					   const std::string &msgBody,
					   const UDTSOCKET &clientSock,
					   std::string &dealResult)> dealMsg;
	const std::string rootPath;
	std::string filePath;
	off64_t fileSize;
	friend class NWmanagerUDT;

private:
	char msg[MSGBODYLEN];
	int cipherLength;
	int haveRead;
	int bodyLen;
	bool headOver;			//标识读取head 是否已经读完
	int64_t fileHaveSend;
	UDTSOCKET clientSock;
	std::string clientPubKey;
	std::shared_ptr<rsa_encrypt>  pRsaEncrypt;
};

#endif // SESSION_H
