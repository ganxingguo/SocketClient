#include <stdio.h>
#include "TCPClientSocket.h"

#pragma comment(lib, "ClientSocket.lib")

class CTestClient : public IClientSocketService
{
public:
	CTestClient()
		: tcpSocket(this)
	{

	};

	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
	{
		printf("读取事件%d - %d\n", pNetHead->bMainID, pNetHead->bAssistantID);
		switch (pNetHead->bMainID)
		{
		case MDM_CONNECT:		// 连接事件
		{
			switch (pNetHead->bAssistantID)
			{
			case ASS_CONNECT_SUCCESS:		// 连接成功
			{
				MSG_S_ConnectSuccess* pMsg = (MSG_S_ConnectSuccess*)pNetData;
				if (pMsg)
				{
					BYTE szTmp[32] = { 0 };
					sprintf((char*)szTmp, "%d", pMsg->i64CheckCode);
					tcpSocket.SetEncodeKey(pMsg->bReserve, 2);

					sprintf((char*)szTmp, "%d", pNetHead->bReserve);
					tcpSocket.SetEncodeKey(pMsg->bReserve, 2);
				}
			}
			break;
			}
		}
		break;
		}
		return true;
	};
	virtual bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket)
	{
		printf("连接事件\n");
		int i = 0;
		tcpSocket.SendData(&i, sizeof(int), MDM_CONNECT, 5, NOENCRYPTION);
		return true;
	};
	virtual bool OnSocketCloseEvent()
	{
		return true;
	};

	CTCPClientSocket tcpSocket;
};

int main()
{
	CTestClient tmpClient;
	tmpClient.tcpSocket.ConnectServer("127.0.0.1", 37115);

	system("pause");

	return 0;
}