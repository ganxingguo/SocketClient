#include "TCPClientSocket.h"

CTCPClientSocket::CTCPClientSocket(IClientSocketService * pIService, DECODE_FUN* cbDecode/*=nullptr*/)
	: m_pIService(pIService)
	, m_cbDecode(cbDecode)
	, m_RecvThread(&CTCPClientSocket::RecvProcess, this)
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

    m_pIService = pIService;
    m_bConnectState = NO_CONNECT;
	m_hSocket = INVALID_SOCKET;

	if (!m_cbDecode)
	{
		static DECODE_FUN s_DecodeFun = [&](char* inBuf, int inSize, char* outBuf, int outSize){
			m_RcDecode.Process((BYTE*)inBuf, (BYTE*)outBuf, inSize);
		};

		m_cbDecode = &s_DecodeFun;
	}
}

CTCPClientSocket::~CTCPClientSocket()
{
    CloseConnect(false);
}

void CTCPClientSocket::RecvProcess()
{
	int nRet = 0;
	int nCurIndex = 0;
	char* szBuf = new char[MAX_RECV_SIZE];
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	while (m_bConnectState == CONNECTED)
	{
		nRet = recv(m_hSocket, szBuf + nCurIndex, MAX_RECV_SIZE - nCurIndex, 0);
		if (nRet == SOCKET_ERROR) {
			break;
		}

		while (nRet + nCurIndex >= sizeof(NetMessageHead)) 
		{
			NetMessageHead* pMsg = (NetMessageHead*)szBuf;
			if (pMsg->uMessageSize > nRet + nCurIndex) {
				break;
			}

			// ������Կ
			UINT check = (UINT)m_RcDecode.GetPerm();
			if (m_RcDecode.IsEncrypt() && check != pMsg->bReserve)
			{
				CloseConnect(true);
				return;
			}


			// ���ݽ���
			char decodeData[65536] = { 0 };
			if (m_cbDecode) {
				(*m_cbDecode)(szBuf + sizeof(NetMessageHead), pMsg->uMessageSize, decodeData, sizeof(decodeData));
			}

			UINT nDataSize = pMsg->uMessageSize - sizeof(NetMessageHead);
			m_pIService->OnSocketReadEvent(pMsg, nDataSize > 0 ? decodeData : nullptr, nDataSize, 0);

			nCurIndex = 0;
			int nTotalSize = pMsg->uMessageSize + sizeof(NetMessageHead);
			memcpy(szBuf, szBuf + nTotalSize, nTotalSize);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	delete[] szBuf;
}

bool CTCPClientSocket::ConnectServer(const char* szServerIP, UINT uPort)
{
    long dwServerIP = inet_addr(szServerIP);
    if(dwServerIP == INADDR_NONE) {
		return false;
    }

    return ConnectServer(dwServerIP, uPort);
}

bool CTCPClientSocket::ConnectServer(const wchar_t* dwServerIP, UINT uPort)
{
	char szTmpIp[64] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, dwServerIP, wcslen(dwServerIP), szTmpIp, sizeof(szTmpIp), NULL, NULL);
	long lIp = inet_addr(szTmpIp);
	return ConnectServer(lIp, uPort);
}

bool CTCPClientSocket::ConnectServer(long int dwServerIP, UINT uPort)
{
	if (dwServerIP == INADDR_NONE) {
		return false;
	}

	if (m_hSocket != INVALID_SOCKET) {
		return false;
	}

    m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_hSocket == INVALID_SOCKET) {
		return false;
	}

	FD_SET fdRead;
	FD_SET fdExpection;
	int nRet = select(m_hSocket + 1, &fdRead, nullptr, &fdExpection, 0);
	if (nRet == SOCKET_ERROR) {
		return false;
	}

    sockaddr_in tmpAddr;
	tmpAddr.sin_family = AF_INET;
	tmpAddr.sin_port = htons(uPort);
	tmpAddr.sin_addr.S_un.S_addr = dwServerIP;

	nRet = connect(m_hSocket, (sockaddr*)&tmpAddr, sizeof(tmpAddr));
	if (nRet == SOCKET_ERROR) {	
		return false;
	}


	m_RcEncode.UnInit();
	m_RcDecode.UnInit();

    //��������
    m_bConnectState = CONNECTING;
    return true;
}


bool CTCPClientSocket::CloseConnect(bool bNotify)
{
	m_RecvThread.joinable();
    if(m_hSocket != INVALID_SOCKET) {
		shutdown(m_hSocket, SD_BOTH);
        closesocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    m_bConnectState = NO_CONNECT;
	if (m_pIService) {
		m_pIService->OnSocketCloseEvent();
	}

	return true;
}

void CTCPClientSocket::SetEncodeKey(const BYTE* pKey, int nLen)
{
	m_RcDecode.Setup(pKey, nLen);
	m_RcEncode.Setup(pKey, nLen);
	m_RcEncode.SetInit(true);
	m_RcDecode.SetInit(true);
}

//���ͺ���
int CTCPClientSocket::SendData(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, UINT bHandleCode)
{
    if((m_hSocket != INVALID_SOCKET) && (uSize <= MAX_SEND_SIZE))
    {
        //��������
        int iErrorCode = 0;
        char bSendBuffer[MAX_SEND_SIZE + sizeof(NetMessageHead)];
        UINT uSendSize = uSize + sizeof(NetMessageHead), uSended = 0;

        //�������
        NetMessageHead * pNetHead = (NetMessageHead *)bSendBuffer;
        pNetHead->uMessageSize = uSendSize;
        pNetHead->bMainID = bMainID;
        pNetHead->bAssistantID = bAssistantID;
        pNetHead->bHandleCode = bHandleCode;
        pNetHead->bReserve = (UINT) m_RcEncode.GetPerm();

		char buf[6]={};
		sprintf(buf,"%d",pNetHead->bReserve);

		unsigned char pDataS[65536]={};
		m_RcEncode.Process((unsigned char * )pData,pDataS,uSize);

		if(uSize > 0) 
			::CopyMemory(bSendBuffer + sizeof(NetMessageHead), pDataS, uSize);

        int iSendCount = 0; //���ʹ���������
        do
        {
            int length = uSendSize - uSended;
            iErrorCode =::send(m_hSocket, bSendBuffer + uSended, uSendSize - uSended, 0);

            int iret = ::GetLastError();
            if(iErrorCode == SOCKET_ERROR)
            {
                if(::WSAGetLastError() == WSAEWOULDBLOCK) //������������ԭ�������ŵĴ����ǣ�return uSize,Fred Huang 2008-05-16
                {
                    if(iSendCount++ > 100) //�ж��ط������Ƿ񳬹�100��,Fred Huang 2008-05-16
                        return SOCKET_ERROR;//̫��ε��ط��ˣ�ֱ�ӷ��� ����,Fred Huang 2008-05-16
                    else
                    {
                        Sleep(10);  //�ȴ� 10 ms����������ط�100�Σ�������1���ӵ�ʱ�����ط�,Fred Huang 2008-05-16
                        continue;   //�ط�����,Fred Huang 2008-05-16
                    }
                }
                else
                    return SOCKET_ERROR;
            }

            uSended += iErrorCode;
            iSendCount = 0; //��Ҫ��������0������һ������
        }
        while(uSended < uSendSize);

        return uSize;
    }

    return SOCKET_ERROR;
}

//������ͺ���
int CTCPClientSocket::SendData(BYTE bMainID, BYTE bAssistantID, UINT bHandleCode)
{
    return SendData(NULL, 0, bMainID, bAssistantID, bHandleCode);
}
