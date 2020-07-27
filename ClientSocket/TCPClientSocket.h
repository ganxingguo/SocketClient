#pragma once

#include <stdio.h>
#include <WinSock2.h>
#include <string>
#include <thread>
#include <functional>

#include "RC4Engine.h"
#include "DataDefine.h"

using namespace std;
using BYTE = unsigned char;
// 加密回调函数类型[inBuf, inSize, outBuf, outSize] 
using DECODE_FUN = std::function<void(char*, int, char*, int)>;

#pragma comment(lib,"ws2_32.lib")

//缓存定义
#define SED_SIZE				60000					// 缓冲区大小
#define RCV_SIZE				30000					// 缓冲区大小

//状态定义
#define NO_CONNECT				0						// 没有连接
#define CONNECTING				1						// 正在连接
#define CONNECTED				2						// 成功连接

//类说明
class CTCPClientSocket;
interface IClientSocketService
{
public:
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket) = 0;
	virtual bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket) = 0;
	virtual bool OnSocketCloseEvent() = 0;
};

//客户端 SOKET
class CTCPClientSocket
{
public:
	CTCPClientSocket(IClientSocketService * pIService, DECODE_FUN* cbDecode = nullptr);
	virtual ~CTCPClientSocket();

	BYTE GetSocketState() { return m_bConnectState; }

	bool ConnectServer(const char* szServerIP, UINT uPort);
	bool ConnectServer(const wchar_t* dwServerIP, UINT uPort);
	bool ConnectServer(long int dwServerIP, UINT uPort);
	int SendData(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, UINT bHandleCode);
	int SendData(BYTE bMainID, BYTE bAssistantID, UINT bHandleCode);
	bool CloseConnect(bool bNotify = true);
	void SetEncodeKey(const BYTE* pKey, int nLen);

private:
	// 接收数据线程
	void RecvProcess();

private:
	BYTE                                m_bConnectState;                // 连接状态
	SOCKET                              m_hSocket;                      // SOCKET 句柄
	IClientSocketService*				m_pIService;					// 处理接口
	BYTE                                m_szBuffer[RCV_SIZE];           // 数据缓存区

	int                                 m_iCheckCode;
	RC4Engine							m_RcEncode;						// 数据加密
	RC4Engine							m_RcDecode;						// 数据解密

	std::thread							m_RecvThread;					// 接收数据线程
	DECODE_FUN*							m_cbDecode;						// 解密回调
};

