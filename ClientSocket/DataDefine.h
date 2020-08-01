#pragma once

#pragma pack(push, 1)

using UINT = unsigned int;

#define MAX_RECV_SIZE					4096 * 10	//һ��recv��󳤶�
#define MAX_SEND_SIZE					4000		//�����Ϣ��

#define SECRET_KEY			20160407				//����
#define NOENCRYPTION		20190722				//������

// ��Ϣ
#define MDM_CONNECT                     1           //������Ϣ����
#define ASS_CONNECT_SUCCESS             3           //���ӳɹ�

//�������ݰ��ṹͷ
struct NetMessageHead
{
	UINT				uMessageSize;                       //���ݰ���С
	UINT				bMainID;                            //����������
	UINT				bAssistantID;                       //������������ ID
	UINT				bHandleCode;                        //���ݰ��������
	UINT				bReserve;                           //�����ֶ�
};

//�������ݰ��ṹͷ
struct NetGateHead
{
public:
	int					nSocketIndex;
	int					nRoomId;
	int					nDeskId; 
	int					uBetchId;
	UINT				uDataSize;
	NetGateHead(UINT uSize = 0) {
		uDataSize = uSize;
		nSocketIndex = -1;
		nRoomId = -1;
		nDeskId = -1;
		uBetchId = -1;
	}
};

struct MSG_S_ConnectSuccess
{
	BYTE                        bMaxVer;                            ///���°汾����
	BYTE                        bLessVer;                           ///��Ͱ汾����
	BYTE                        bReserve[2];                        ///�����ֶ�
	UINT                        i64CheckCode;                       ///���ܺ��У���룬�ɿͻ��˽����ڰ�ͷ�з���
};

#pragma pack(pop)
