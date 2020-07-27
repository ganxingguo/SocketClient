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
// ���ܻص���������[inBuf, inSize, outBuf, outSize] 
using DECODE_FUN = std::function<void(char*, int, char*, int)>;

#pragma comment(lib,"ws2_32.lib")

//���涨��
#define SED_SIZE				60000					// ��������С
#define RCV_SIZE				30000					// ��������С

//״̬����
#define NO_CONNECT				0						// û������
#define CONNECTING				1						// ��������
#define CONNECTED				2						// �ɹ�����

//��˵��
class CTCPClientSocket;
interface IClientSocketService
{
public:
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket) = 0;
	virtual bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket) = 0;
	virtual bool OnSocketCloseEvent() = 0;
};

//�ͻ��� SOKET
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
	// ���������߳�
	void RecvProcess();

private:
	BYTE                                m_bConnectState;                // ����״̬
	SOCKET                              m_hSocket;                      // SOCKET ���
	IClientSocketService*				m_pIService;					// ����ӿ�
	BYTE                                m_szBuffer[RCV_SIZE];           // ���ݻ�����

	int                                 m_iCheckCode;
	RC4Engine							m_RcEncode;						// ���ݼ���
	RC4Engine							m_RcDecode;						// ���ݽ���

	std::thread							m_RecvThread;					// ���������߳�
	DECODE_FUN*							m_cbDecode;						// ���ܻص�
};

