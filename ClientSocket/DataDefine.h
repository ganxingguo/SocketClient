#pragma once

#pragma pack(push, 1)

using UINT = unsigned int;

#define MAX_RECV_SIZE					4096 * 10	//一次recv最大长度
#define MAX_SEND_SIZE					4000		//最大消息包

#define SECRET_KEY			20160407				//加密
#define NOENCRYPTION		20190722				//不加密

// 消息
#define MDM_CONNECT                     1           //连接消息类型
#define ASS_CONNECT_SUCCESS             3           //连接成功

//网络数据包结构头
struct NetMessageHead
{
	UINT				uMessageSize;                       //数据包大小
	UINT				bMainID;                            //处理主类型
	UINT				bAssistantID;                       //辅助处理类型 ID
	UINT				bHandleCode;                        //数据包处理代码
	UINT				bReserve;                           //保留字段
};

//网络数据包结构头
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
	BYTE                        bMaxVer;                            ///最新版本号码
	BYTE                        bLessVer;                           ///最低版本号码
	BYTE                        bReserve[2];                        ///保留字段
	UINT                        i64CheckCode;                       ///加密后的校验码，由客户端解密在包头中返回
};

#pragma pack(pop)
