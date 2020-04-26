#include <iostream>
#include <errno.h>
#include <cstring>

#include "prinetdiskclientpi.h"

using namespace std;

int main(int argc, char *argv[])
{

	while(1)
	{
		priNetDiskClientPI  test;
//		test.startClient("123.206.196.128" , 8888);
		test.startClient("127.0.0.1" , 8888 , "IadminHome:adminHome123");		//链接服务器
		test.waitForTer();											//等待服务端发送消息,阻塞
//		test.startClient(test.OclientIP,test.OclientPort,"");		//发送打洞包,注意:需要绑定与服务器通信的本地端口
//		test.startServer();
		test.udp_through();
		test.startServer();
	}
	return 0;
}
