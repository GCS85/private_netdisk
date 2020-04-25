#ifndef NWMANAGERUDT_H
#define NWMANAGERUDT_H


#include <vector>
#include <string>
#include <map>
#include <set>
#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <future>

#include "encrypt.h"
#include "udt.h"
#include "session.h"


class NWmanagerUDT
{
public:
	enum
	{
		DELETE_IMME = -1,
		DELETE_DELAY = 0,
		NOT_DELETE = 1,
		LISTENSIZE = 10,
		EVENTSIZE = 50
	};
	NWmanagerUDT();
	~NWmanagerUDT();

	void bindLocal		(const unsigned short &port );
	void doStart		();
	void doStop			();
	void doWrite		(const UDTSOCKET &writeFd);
	void doSendFile		(const UDTSOCKET &writeFd);

	void doRecvFile		(const UDTSOCKET &readFd) ;
	void deleteSession	(const UDTSOCKET &);
	//回调方法
	std::function<void(const UDTSOCKET & ,std::shared_ptr<session>)> AcceptCallBack;
	std::function<void(const UDTSOCKET &)> DeletCallBack;
	std::function<std::shared_ptr<session>(const UDTSOCKET &)> searchSession;
	std::function<int(const std::string &msgHead ,
					   const std::string &msgBody,
					   const UDTSOCKET &clientSock,
					   std::string &dealResult)> dealMsg;

private:
	volatile bool start;
	std::map<std::shared_ptr<session>,std::future<int>> asyncTask;
	UDTSOCKET localSock;
	int epollFd;
	std::set<UDTSOCKET> readfds;
	std::set<UDTSOCKET> writefds;
	std::shared_ptr<rsa_encrypt> pRsaEncrypt;
	/*private member function*/
	void epollCtl(int fd, int op, int state);
	void epollWait();
	void doRead(const UDTSOCKET &readFd);
	void handleEvent(const int &num);
	void doAccept();
	void doRecvPubKey(const UDTSOCKET &readFd);
	void doSendPubKey(const UDTSOCKET &writeFd);
};

#endif // NWMANAGERUDT_H
