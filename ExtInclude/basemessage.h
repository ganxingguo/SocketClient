#pragma once

#ifndef AFC_MESSAGE_HEAD_FILE
#define AFC_MESSAGE_HEAD_FILE

/********************************************************************************************/
//出包用的端口号
//端口定义
#define FLASH_SERVER_PORT       843                             ///页游认证服务器端口//国际标准端口号

#define GAME_LOGON_PORT         3015                            ///游戏登陆端口

#define CENTER_SERVER_PORT      13025                       ///中心服务端口
#define CENTER_DISTRI_PORT      13026                          ///中心服务中转端口

#define Z_W_CONNECT_PORT        13027                          ///主服务器与游戏服务器连接端口

/********************************************************************************************/

#define NUM_DESK_LOCK_PASS              1						//房间密码位数

///开发库版本
#define DEV_LIB_VER             1                                   ///开发库版本
///全局版权，固定在程序里，无法修改
#define TMLcopyright            TEXT("开发:")             ///版权

///网络数据包结构头
struct NetMessageHead
{
	UINT                        uMessageSize;                       ///数据包大小
	UINT                        bMainID;                            ///处理主类型
	UINT                        bAssistantID;                       ///辅助处理类型 ID
	UINT                        bHandleCode;                        ///数据包处理代码
	UINT                        bReserve;                           ///保留字段
};

///网络数据包结构头
struct NetGateHead
{
public:
	int              nSocketIndex;
	int              nRoomId;
	int              nDeskId;
	int				 uBetchId;
	UINT            uDataSize;
	NetGateHead(UINT uSize = 0) {
		uDataSize = uSize;
		nSocketIndex = -1;
		nRoomId = -1;
		nDeskId = -1;
		uBetchId = -1;
	}
};
static const char*  CompressHead = "LZOCompress";
///常量定义
#define MAX_PEOPLE              180                                 ///最大游戏人数 百家乐 modify 8 -> 180 by wlr 20090714
#define INVALID_CHAIR			255									///无效座位
#define MAX_SEND_SIZE           4000                                ///最大消息包
#define MAX_SEND_SIZE_NEW           4080                                ///最大消息包
#define MAX_TALK_LEN            500                                 ///最大聊天数据长度
#define NORMAL_TALK_LEN         200                                 ///普通聊天数据长度
#define US_PLAY_GAME            20                                  ///用户游戏状态
#define NET_HEAD_SIZE           sizeof(NetMessageHead)              ///数据包头
#define NET_GATEHEAD_SIZE       sizeof(NetGateHead)              //网管包头
#define SOCKET_BUFFER           MAX_SEND_SIZE + NET_HEAD_SIZE + NET_GATEHEAD_SIZE      ///发送数据包
#define SOCKET_Compress_Size    strlen(CompressHead)
///游戏类型定义
#define TY_NORMAL_GAME          1                                   ///普通游戏
#define TY_MATCH_GAME           2                                   ///比赛游戏
#define TY_MONEY_GAME           3                                   ///金币游戏
#define TY_DRILLING_GAME        4                                   ///演练场游戏(开发中)
#define TY_FASCINATION_GAME     5                                   ///魅力场游戏(开发中)
///
///Kylin 20090105 添加赛币场
#define TY_SHABI_GAME       6                                   ///赛币场游戏
///
///支持类型定义
#define SUP_NORMAL_GAME         0x01                                ///普通游戏
#define SUP_MATCH_GAME          0x02                                ///比赛游戏
#define SUP_MONEY_GAME          0x04                                ///金币游戏

///显示位置
#define SHS_TALK_MESSAGE        0x01                                ///聊天信息
#define SHS_MESSAGE_BOX         0x02                                ///弹出信息
#define SHS_SYSTEM_SHOW         0x04                                ///系统消息
#define CodeSocketConnetID(nSocketId,nConnetId)(UINT64(nSocketId)*10000+(nConnetId)) //创建GateScoketID
//解析
#define DeCodeSocketID(nCode)((nCode)/10000)
#define DeCodeConnetID(nCode) (nCode%10000)
#pragma pack(1)
///网络数据包结构头

class NetGateMessage
{
public:
	NetGateMessage(const NetGateHead *head = nullptr, const void* pData = nullptr,const UINT uSize = 0) {
		ZeroMemory(m_data, sizeof(m_data));
		m_uSize = 0;
		MakeData(head, pData,uSize);
	}
	~NetGateMessage() {
	}
	inline void MakeData(const NetGateHead *head, const void* pData,const UINT nBuffSize) {	
		if (head != nullptr)
		{
			memcpy(m_data, head, NET_GATEHEAD_SIZE);
			m_uSize += NET_GATEHEAD_SIZE;
		}
		
		if (pData != nullptr)
		{
			memcpy(m_data + m_uSize, pData,nBuffSize);
			m_uSize += nBuffSize;
		}
		
	}
	inline void* GetMessage() const {
		
		return m_uSize == 0 ? nullptr:(void *)m_data;
	}
	inline UINT GetSize() const { return m_uSize; }
private:
	BYTE m_data[MAX_SEND_SIZE_NEW];
	UINT m_uSize;
};

//此处为密钥由客户自己管理，设置在整数范围
#define SECRET_KEY  20160407
//不加密判断
#define NOENCRYPTION  20190722
///连接成功消息
struct MSG_S_ConnectSuccess
{
    BYTE                        bMaxVer;                            ///最新版本号码
    BYTE                        bLessVer;                           ///最低版本号码
    BYTE                        bReserve[2];                        ///保留字段
    UINT                        i64CheckCode;                       ///加密后的校验码，由客户端解密在包头中返回
};
#pragma pack()

/********************************************************************************************/
/// 系统内核使用 （MAINID 使用 0 - 49 ）
/// 系统服务通信 （MAINID 使用 50 - 99 ）
/// 游戏大厅服务 （MAINID 使用 100 - 149 ）
/// 附加游戏通信 （MAINID 使用 151 - 199 ）
/// 保留通信标志 （MAINID 使用 200 - 255 ）

/********************************************************************************************/

///特殊消息
#define MDM_NO_FAILE                    0           ///不失败消息

///连接消息
#define MDM_CONNECT                     1           ///连接消息类型

#define MDM_GP_CONNECT                  88           ///连接消息类型
#define MDM_USER_OFFLLINE               89 //玩家掉线
#define MDM_USER_SUB_SIT                90 //玩家坐下
#define MDM_USER_SUB_UP                 91//玩家离开
#define MDM_USER_KICKOUT                92//踢人
#define MDM_GATECONNECT                 98           ///连接消息类型
#define MDM_G_SQLMESSAGE                      97
#define MDM_SQLMESSAGE                      99
///辅助处理消息标志
#define ASS_NET_TEST                    1           ///网络测试
#define ASS_CONNECT_SUCCESS             3           ///连接成功
#define ASS_NET_END                     4           ///网络连接结束消息
#define ASS_NET_GET                     5           ///网络请求秘钥
#define ASS_NET_CLOSE_SOCKET			6			//服务器客户端对象网络关闭消息
#define ASS_NET_CLOSE_SOCKETEX			7			//服务器客户端对象网络关闭消息
//#define ASS_CONNECT_SUCCESSSERVER       6           ///连接成功
/********************************************************************************************/

#endif