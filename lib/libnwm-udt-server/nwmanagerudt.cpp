#include "nwmanagerudt.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/sendfile.h>
#include <fcntl.h>

static void setReuse(UDTSOCKET &setSock)
{
	bool opt = true;
	int ret = -1;
	ret = UDT::setsockopt(setSock, 0  , UDT_REUSEADDR , (char*)&opt , sizeof(opt));
	if(ret)
		throw  std::runtime_error(UDT::getlasterror().getErrorMessage());
}

static void setNONBlock(UDTSOCKET &setSock)
{
	//不能设置 发送 为非阻塞 , 当发送文件时,文件过大数据没有准备好,非阻塞sendfile()会阻塞并一直等待数据
	//	if(UDT::ERROR == UDT::setsockopt(setSock, 0  , UDT_SNDSYN , new bool(false) , sizeof(bool)))
	//		throw  std::runtime_error(UDT::getlasterror().getErrorMessage());

	//	if(UDT::ERROR == UDT::setsockopt(setSock, 0  , UDT_RCVSYN , new bool(false) , sizeof(bool)))
	//		throw  std::runtime_error(UDT::getlasterror().getErrorMessage());
}

NWmanagerUDT::NWmanagerUDT():start(false)
{
	/* set socket fd*/
	UDT::startup();
	this->localSock = UDT::socket(AF_INET,SOCK_STREAM,0);
	setReuse(this->localSock);

	this->pRsaEncrypt = std::make_shared<rsa_encrypt>();
	this->pRsaEncrypt->generate_RSAKey();
}

NWmanagerUDT::~NWmanagerUDT()
{
	UDT::close(this->localSock);
	UDT::epoll_release(this->epollFd);
	UDT::cleanup();
}

/*服务端绑定本地地址*/
void NWmanagerUDT::bindLocal(const unsigned short &Port)
{
	sockaddr_in serverAddr;
	serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	serverAddr.sin_port = htons( Port );
	serverAddr.sin_family = AF_INET;
	if( UDT::ERROR == (UDT::bind(this->localSock,(sockaddr*)&serverAddr,sizeof(sockaddr))) )
		throw std::runtime_error( UDT::getlasterror().getErrorMessage());
}

void NWmanagerUDT::doStart()
{
	/* switch on*/
	this->start = true;
	/* set epoll fd*/
	this->epollFd = UDT::epoll_create();
	UDT::listen(this->localSock,LISTENSIZE);
	//将监听套接字加入读检测事件中
	UDT::epoll_add_usock(epollFd,localSock);
	std::cout << "start listen...." <<std::endl;
	epollWait();
}

void NWmanagerUDT::doStop()
{
	this->start = false;
}

void NWmanagerUDT::epollWait()
{
	while(this->start)
	{
		for( auto itor = asyncTask.begin() ; itor!= asyncTask.end() ; /*itor++*/)
		{
			if( !(*itor).second.valid())
			{
				itor++;
				continue;
			}
			int ret = (*itor).second.get();
			if( ret == 0)
			{
				//				(*itor).first->fileHaveSend = 0;
				asyncTask.erase(itor++);
				std::cout <<"File Over" <<std::endl;
			}
			else if( ret == EAGAIN)
			{
				//doSendFile(itor->first->clientSock);
				++itor;
			}
			else
				asyncTask.erase(itor++);
		}
		int ret = UDT::epoll_wait(this->epollFd,&this->readfds,NULL,-1);
		handleEvent(ret);
	}
}

void NWmanagerUDT::handleEvent(const int& num)
{
	if( !readfds.empty())
		for(auto &x : this->readfds)
		{
			if(x != this->localSock)
				doRead(x);
			else
				doAccept();

		}
}

void NWmanagerUDT::doAccept()
{
	UDTSOCKET clifd;
	struct sockaddr_in cliaddr;
	int  cliaddrlen = sizeof(sockaddr);

	clifd = UDT::accept(this->localSock,(struct sockaddr*)&cliaddr,&cliaddrlen);
	if (clifd == UDT::INVALID_SOCK)
	{
		std::cerr<< (UDT::getlasterror().getErrorMessage()) ;
		return ;
	}
	else
	{
		std::cout <<"accept a new client:"<< inet_ntoa(cliaddr.sin_addr)<<":" << ntohs(cliaddr.sin_port)<<std::endl;
		AcceptCallBack(clifd , std::make_shared<session>(clifd,this->dealMsg , this->pRsaEncrypt));
		this->doRecvPubKey(clifd);
		this->doSendPubKey(clifd);
		UDT::epoll_add_usock(this->epollFd,clifd);
	}
}

void NWmanagerUDT::doRecvPubKey(const UDTSOCKET &readFd)
{
	std::shared_ptr<session> pCliSession = searchSession(readFd);
	int ret = 0;
	if( !pCliSession->headOver )
	{
		int &haveRead = pCliSession->haveRead;
		int &bodyLen = pCliSession->bodyLen;
		std::cout << "RECV Client PubKey :"<<std::endl;

		ret = UDT::recv(readFd , pCliSession->msg+haveRead ,session::MSGHEADLEN - haveRead ,0);
		if( ret <= 0)
		{
			deleteSession(readFd);
			return ;
		}
		haveRead += ret;
		if( haveRead == session::MSGHEADLEN )
		{
			pCliSession->msg[session::MSGHEADLEN] = '\0';

			for( auto itor = pCliSession->msg ; (*itor)!='\0' ; itor ++)
				if( !( (*itor) >= '0' && (*itor) <= '9'))
				{
					strcpy(pCliSession->msg,"0013ERROR:msgHead");
					doWrite(readFd);
					return ;
				}
			pCliSession->bodyLen = std::atoi(pCliSession->msg);
			pCliSession->headOver = true;
			haveRead = 0;

			ret = UDT::recv(readFd , pCliSession->msg+haveRead , bodyLen - haveRead ,0);
			if( ret <= 0)
			{
				deleteSession(readFd);
				return ;
			}
			haveRead += ret;
			if( haveRead == bodyLen)
			{
				pCliSession->headOver = false;
				pCliSession->msg[haveRead] = '\0';
				haveRead = 0;
				std::cout << "PUBKEY_RECV " << pCliSession->msg <<std::endl;
				pCliSession->clientPubKey.assign(pCliSession->msg);
			}
		}
	}
}

void NWmanagerUDT::doSendPubKey(const UDTSOCKET &writeFd)
{
	std::shared_ptr<session> pCliSession = searchSession(writeFd);
	/* ....  */
	std::string serverPubKey(this->pRsaEncrypt->get_self_pubKey());
	pCliSession->addMsgHead(serverPubKey);
	memcpy(pCliSession->msg , serverPubKey.c_str() ,serverPubKey.length() );
	int ret = UDT::send(writeFd, pCliSession->msg,/*strlen(pCliSession->msg)*/serverPubKey.length() ,0);
}

void NWmanagerUDT::doRead(const UDTSOCKET& readFd)
{
	std::shared_ptr<session> pCliSession = searchSession(readFd);
	int ret = 0;
	if( pCliSession->headOver )		//读取body
	{
		int &haveRead = pCliSession->haveRead;
		int &bodyLen = pCliSession->bodyLen;
		//while( haveRead < bodyLen)
		{
			ret = UDT::recv(readFd , pCliSession->msg+haveRead , bodyLen - haveRead ,0);
			if( ret <= 0)
			{
				deleteSession(readFd);
				return ;
			}
			haveRead += ret;
		}
		if( haveRead == bodyLen)
		{
			pCliSession->headOver = false;
			pCliSession->msg[haveRead] = '\0';
			haveRead = 0;

			const std::string &clearContent( this->pRsaEncrypt->decrypt_from_memory(std::string(pCliSession->msg , bodyLen) ));
			memcpy(pCliSession->msg , clearContent.c_str() ,clearContent.length());
			pCliSession->msg[ clearContent.length() ] = '\0';
			std::cout << "msg recv :" << pCliSession->msg <<std::endl;
			ret = pCliSession->splitMsg();
			if( DELETE_IMME == ret )
				deleteSession(readFd);
			else if( DELETE_DELAY == ret )
			{
				doWrite(readFd);
				deleteSession(readFd);
			}
			else if( NOT_DELETE == ret )
				doWrite(readFd);

		}
	}
	else							//读取head
	{
		int &haveRead = pCliSession->haveRead;
		//while( haveRead < session::MSGHEADLEN)
		{
			std::cout << "start recv :"<<std::endl;
			ret = UDT::recv(readFd , pCliSession->msg+haveRead ,session::MSGHEADLEN - haveRead ,0);
			if( ret <= 0)
			{
				deleteSession(readFd);
				return ;
			}
			haveRead += ret;

		}
		if( haveRead == session::MSGHEADLEN )
		{
			pCliSession->msg[session::MSGHEADLEN] = '\0';

			for( auto itor = pCliSession->msg ; (*itor)!='\0' ; itor ++)
				if( !( (*itor) >= '0' && (*itor) <= '9'))
				{
					/* 头部出现错误
					 * 解决方案：
					 * 1. 发送消息给客户端，重发
					 * 2. 清除本地收到的消息，重新接受头部
					 * */
					strcpy(pCliSession->msg,"0013ERROR:msgHead");
					doWrite(readFd);
					return ;
				}
			pCliSession->bodyLen = std::atoi(pCliSession->msg);
			pCliSession->headOver = true;
			haveRead = 0;
		}
	}
}

void NWmanagerUDT::deleteSession(const UDTSOCKET &clientSock)
{
	//	epollCtl(clientSock,EPOLL_CTL_DEL,NULL);
	UDT::epoll_remove_usock(this->epollFd,clientSock);
	DeletCallBack(clientSock);	//释放session
}

void NWmanagerUDT::doWrite(const UDTSOCKET &writeFd)
{
	std::shared_ptr<session> pCliSession = searchSession(writeFd);
	/* ....  */
	int ret = UDT::send(writeFd,pCliSession->msg,/*strlen(pCliSession->msg)*/pCliSession->cipherLength,0);
	if(ret < 0)
	{
		if( errno == EWOULDBLOCK)
		{
			/*....*/
		}
	}
}

void NWmanagerUDT::doSendFile(const UDTSOCKET &writeFd)
{
	std::shared_ptr<session> clientPtr = searchSession(writeFd);
	// std::launch::deferred 该属性的异步任务 到调用 .get()方法才会执行任务
	asyncTask[clientPtr] = std::async(std::launch::deferred,[clientPtr]()
	{
		int64_t &offset = clientPtr->fileHaveSend;
		if( UDT::ERROR == UDT::sendfile2(clientPtr->clientSock,
										 clientPtr->filePath.c_str(),
										 &offset,
										 clientPtr->fileSize))
			throw  std::runtime_error(UDT::getlasterror().getErrorMessage());

		if( offset != clientPtr->fileSize )
			return EAGAIN;
		else
			offset = 0;
		return 0;
	});
}


void NWmanagerUDT::doRecvFile(const UDTSOCKET &readFd)
{
	std::shared_ptr<session> clientPtr = searchSession(readFd);
	asyncTask[clientPtr] = std::async(std::launch::deferred,[clientPtr]()
	{
		int64_t &offset = clientPtr->fileHaveSend;
		std::cout << "Start Recv File"<<std::endl;
		if( UDT::ERROR == UDT::recvfile2(clientPtr->clientSock ,
										 clientPtr->filePath.c_str(),
										 &offset,
										 clientPtr->fileSize))
			throw std::runtime_error(UDT::getlasterror().getErrorMessage());
		std::cout << "Have Recv End"<<std::endl;
		if( offset != clientPtr->fileSize)
			return EAGAIN;
		else
			offset = 0;
		return 0;
	});
}

