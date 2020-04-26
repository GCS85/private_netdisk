#include "nwmanagerudtclient.h"


#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <cstdio>

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

static const std::string H_CLOSESESSION("closeSession");

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
	bool opt = false;
	int ret = -1;
	ret = UDT::setsockopt(setSock, 0  , UDT_SNDSYN , (char*)&opt , sizeof(opt));
	if(ret)
		throw  std::runtime_error(UDT::getlasterror().getErrorMessage());

	ret = UDT::setsockopt(setSock, 0  , UDT_RCVSYN , (char*)&opt , sizeof(opt));
	if(ret)
		throw  std::runtime_error(UDT::getlasterror().getErrorMessage());
}
NWmanagerUDTClient::NWmanagerUDTClient():
	headOver(false),haveRead(0),bodyLen(0),
	SPLCHAR('#')
{
	UDT::startup();
	this->localSock = UDT::socket(AF_INET,SOCK_STREAM,0);
	if( UDT::INVALID_SOCK == this->localSock)
		throw std::runtime_error(UDT::getlasterror().getErrorMessage());		//构造函数中抛出异常必须保证申请的内存有效释放
	setReuse(this->localSock);

	this->rsaEncrypt.generate_RSAKey();

}

NWmanagerUDTClient::~NWmanagerUDTClient()
{
	if( UDT::getsockstate(localSock) == UDTSTATUS::CONNECTED  )
		this->doStop();
	UDT::close(localSock);
	UDT::cleanup();
}

int NWmanagerUDTClient::doStart(const std::string &destIP, const unsigned short &destPort)
{
	sockaddr_in serverAddr;
	serverAddr.sin_addr.s_addr = inet_addr(destIP.c_str());
	serverAddr.sin_port = htons( destPort );
	serverAddr.sin_family = AF_INET;

	/* TEST */
	{
		std::cout <<"\n\nremotePort: "<< ntohs(serverAddr.sin_port)<<std::endl;
		std::cout <<"remote IP : "<< destIP <<std::endl;
		sockaddr_in addrlocal;
		bzero(&addrlocal,sizeof(sockaddr));
		int addrlen = sizeof(sockaddr);
		UDT::getsockname(this->localSock,(sockaddr*)&addrlocal,&addrlen);
		std::cout <<"local Port: "<< ntohs(addrlocal.sin_port)<<std::endl;
		std::cout << std::endl;
	}
	/* END */
	if(   UDT::ERROR == UDT::connect(this->localSock ,(sockaddr*)&serverAddr,sizeof(sockaddr)) )
	{
		int errsv = UDT::getlasterror_code();
		if(  errsv == ECONNREJ || errsv == 1002	|| errsv == 1001)
			return -1;
		else
			throw std::runtime_error( UDT::getlasterror().getErrorMessage());
	}

	std::cout <<"connect success"<<std::endl;
	sockaddr_in myAddr;
	bzero(&myAddr,sizeof(sockaddr));
	int len = sizeof(sockaddr);

	if( UDT::ERROR == UDT::getsockname(this->localSock,(sockaddr*)&myAddr,&len))
		throw std::runtime_error(UDT::getlasterror().getErrorMessage());

	this->doWritePubKey();
	this->doReadPubKey();
	return ntohs(myAddr.sin_port);
}
//return value  -1 : system error   -2 : remote error
int NWmanagerUDTClient::doRead()
{
	int ret = 0;
	//读取头部
	while( haveRead < MSGHEADLEN)
	{
		ret = UDT::recv(localSock , this->msg+haveRead ,MSGHEADLEN - haveRead ,0);
		if( ret < 0)
		{
			return -1;
		}
		haveRead += ret;
		//		std::cout<< msg<<std::endl;
	}
	if( haveRead == MSGHEADLEN )
	{
		this->msg[MSGHEADLEN] = '\0';

		for( auto itor = this->msg ; (*itor)!='\0' ; itor ++)
			if( !( (*itor) >= '0' && (*itor) <= '9'))
			{
				return -1;
			}
		this->bodyLen = std::atoi(this->msg);
		this->headOver = true;
		haveRead = 0;
	}

	//读取body
	if( this->headOver )
	{
		while( haveRead < bodyLen)
		{
			ret = UDT::recv(this->localSock , this->msg+haveRead , bodyLen - haveRead ,0);
			if( ret <= 0)
			{
				//				doStop();
				return -1;
			}
			haveRead += ret;
		}
		if( haveRead == bodyLen)
		{
			this->headOver = false;
			this->msg[haveRead] = '\0';
			haveRead = 0;
			const std::string cipher(this->msg , bodyLen );
#ifdef Test
			displayBit(cipher);
#endif
			const std::string &clearContent(
						this->rsaEncrypt.decrypt_from_memory(cipher));
			std::cout << "msg recv : "<< clearContent<<std::endl;
			memcpy(this->msg , clearContent.c_str() ,clearContent.length());
			this->msg[clearContent.length()]='\0';
			return splitMsg();
		}
	}
	return 0;
}

int NWmanagerUDTClient::doReadFile(const std::string &filePath , const int64_t &fileSize)
{
	int64_t offset = 0;
	if( UDT::ERROR == UDT::recvfile2(this->localSock, filePath.c_str(),&offset,fileSize))
		throw std::runtime_error(UDT::getlasterror().getErrorMessage());
	return 0;
}

int NWmanagerUDTClient::doWrite()
{
	int ret = -1;
	if( (ret = UDT::send(this->localSock,this->msg,/*strlen(this->msg)*/this->cipherLength,0)) == UDT::ERROR)
	{
		if( ret != this->cipherLength )
		{
			/*....*/
			std::cout << UDT::getlasterror().getErrorMessage() <<std::endl;
			std::cout << "Client Send ERROR"<< this->msg <<std::endl;
		}
	}
	return 0;
}

int NWmanagerUDTClient::doWriteFile(const std::string &filePath, const int64_t &fileSize)
{
	int64_t offset = 0;
	sleep(2);
	if( UDT::ERROR == UDT::sendfile2(this->localSock,
									 filePath.c_str(),
									 &offset,
									 fileSize))
		throw  std::runtime_error(UDT::getlasterror().getErrorMessage());
	if( offset != fileSize )
	{
		std::cout << "NOT Complete"<<std::endl;
		return -1;
	}
	std::cout << "Have Send Over"<<std::endl;
	return 1;
}

int NWmanagerUDTClient::doStop()
{
	this->setMsg(H_CLOSESESSION + "#");
	doWrite();
}

int NWmanagerUDTClient::sendHolePacket(const std::string &OclientIP, const unsigned short &OclientPort ,
										const unsigned &localPort)
{
	// 关闭当前连接
	this->doStop();
	UDT::close(localSock);
	UDT::cleanup();

	//新建  UDP  套接字  发送打洞包
	int toClientSock = -1;
	struct sockaddr_in remoteAddr;
	struct sockaddr_in localAddr;

	toClientSock = ::socket(AF_INET,SOCK_DGRAM,0);
	if( toClientSock == -1)
	{
		std::cout << "send hole Packets Failed!"<<std::endl;
		return -1;
	}
	remoteAddr.sin_family		= AF_INET;
	remoteAddr.sin_addr.s_addr	= inet_addr(OclientIP.c_str());
	remoteAddr.sin_port			= htons( OclientPort );

	localAddr.sin_family		= AF_INET;
	localAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	localAddr.sin_port			= htons(localPort);

	socklen_t sockAddrLen = sizeof( sockaddr );
	if( -1 == ::bind(toClientSock , (struct sockaddr*)&localAddr , sockAddrLen ))
	{
		std::cout << ::strerror(errno) << std::endl;
		return -1;
	}

	sockaddr_in tempAddr;
	for( int i = 0 ; i < 5 ; ++i )
	{
		if( -1 == ::sendto(toClientSock , "Hole" , sizeof("Hole") , 0 ,
				 (struct sockaddr*)&remoteAddr , sockAddrLen))
		{
			std::cout << strerror(errno)<<std::endl;
			continue;
		}
		std::cout << "\nsend Hole Msg!\n";
//		if( -1 != ::getpeername(toClientSock , (struct sockaddr*)&tempAddr,&sockAddrLen))
//		{
//			std::cout << "IP: " << inet_ntoa(tempAddr.sin_addr) << "  Port: " << ntohs(tempAddr.sin_port)<<std::endl;
//		}
	}
	::close(toClientSock);
}

void NWmanagerUDTClient::setMsg(std::string Msg)
{
	std::string cipher(this->rsaEncrypt.encrypt_from_memory(Msg , this->serverPubKey));
	std::cout << "msg send : " << Msg <<std::endl;
	addMsgHead(cipher);  // cipher + 4
	memcpy(this->msg , cipher.c_str() , cipher.length());
	this->msg[ cipher.length() ] = '\0';
	this->cipherLength = cipher.length()/*+1*/;
}

void NWmanagerUDTClient::bindLocal(const unsigned short &localPort)
{
	sockaddr_in serverAddr;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons( localPort );
	serverAddr.sin_family = AF_INET;
	if( UDT::ERROR == UDT::bind(this->localSock,(sockaddr*)&serverAddr,sizeof(sockaddr)) )
		throw std::runtime_error(UDT::getlasterror().getErrorMessage());
}

int NWmanagerUDTClient::splitMsg()
{
	auto itor = std::find(msg,msg+bodyLen,SPLCHAR);
	if( itor - msg == bodyLen)
		return -1;
	else
		return dealMsg(std::string(msg,itor),std::string(itor+1));
}

void NWmanagerUDTClient::addMsgHead(std::string &Msg)
{
	std::string Head = std::to_string(Msg.length());
	while( Head.length() != MSGHEADLEN)
		Head.insert(Head.begin(),'0');
	Msg.insert(0,Head);
}

int NWmanagerUDTClient::doReadPubKey()
{
	int ret = 0;
	//读取头部
	while( haveRead < MSGHEADLEN)
	{
		ret = UDT::recv(localSock , this->msg+haveRead ,MSGHEADLEN - haveRead ,0);
		if( ret < 0)
		{
			return -1;
		}
		haveRead += ret;
	}

	if( haveRead == MSGHEADLEN )
	{
		this->msg[MSGHEADLEN] = '\0';

		for( auto itor = this->msg ; (*itor)!='\0' ; itor ++)
			if( !( (*itor) >= '0' && (*itor) <= '9'))
			{
				return -1;
			}
		this->bodyLen = std::atoi(this->msg);
		this->headOver = true;
		haveRead = 0;
	}

	//读取body
	if( this->headOver )
	{
		while( haveRead < bodyLen)
		{
			ret = UDT::recv(this->localSock , this->msg+haveRead , bodyLen - haveRead ,0);
			if( ret <= 0)
			{
				//				doStop();
				return -1;
			}
			haveRead += ret;
		}
		if( haveRead == bodyLen)
		{
			this->headOver = false;
			this->msg[haveRead] = '\0';
			haveRead = 0;
			this->serverPubKey.assign(this->msg);
			std::cout << "SERVER PubKey : " << this->serverPubKey <<std::endl;
		}
	}
	return 0;
}

int NWmanagerUDTClient::doWritePubKey()
{
	std::string clientPubKey(this->rsaEncrypt.get_self_pubKey());
	addMsgHead(clientPubKey);
	memcpy(this->msg , clientPubKey.c_str() , clientPubKey.length());
	this->msg[ clientPubKey.length() ] = '\0';
	if( UDT::ERROR == UDT::send(this->localSock,this->msg,/*strlen(this->msg)*/clientPubKey.length(),0))
	{
		std::cout << "Send PubKey failed"<<std::endl;
	}
}


