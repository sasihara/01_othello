#pragma once
#include <WinSock2.h>

// define
#define	PROTOCOL_VERSION	1
#define LOCAL_PORT_NUM_STR	"60001"
#define	WAIT_TIME_THINK_ACCEPT	5	// seconds	

// enum
enum class PROTOCOLSTATES {
	INIT = 0,
	SOCKET_READY,
	WAITING_INFORMATION_RESP,
	THINKER_AVAILABLE,
	WAITING_THINK_STOP_RESP,
	WAITING_THINK_ACCEPT_RESP,
	WAITING_THINK_RESP,
	PROTOCOLSTATES_LIMIT
};

enum class MESSAGETYPE {
	THINK_REQUEST = 1,
	THINK_ACCEPT,
	THINK_REJECT,
	THINK_RESPONSE,
	THINK_STOP_REQUEST,
	THINK_INDICATION,
	QUIT,
	INFORMATION_REQUEST,
	INFORMATION_RESPONSE,
	MESSAGETYPE_LIMIT
};

enum class TYPE {
	ID = 1,
	BOARD,
	TURN,
	PLACE,
	REASON,
	VERSION,
	TEXTINFO
};

// struct
#pragma pack(push, 1)
typedef struct _MESSAGEHEADER {
	unsigned _int8 ProtocolVersion;
	unsigned _int8 MessageType;
	unsigned _int16 MessageLength;
} MESSAGEHEADER;

typedef struct _TLV_HEADER {
	unsigned _int8 Type;
	unsigned _int16 Length;
} TLV_HEADER;

typedef struct _ID {
	unsigned _int16 id;
} ID;

typedef struct _BOARD {
	unsigned _int8 board[64];
} BOARD;

typedef struct _TURN {
	unsigned _int16 turn;
} TURN;

typedef struct _PLACE {
	unsigned _int8 x;
	unsigned _int8 y;
} PLACE;

typedef struct _REASON {
	unsigned _int8 reason;
} REASON;

typedef struct _VERSION {
	unsigned _int8 version;
} VERSION;

typedef struct _TEXTINFO {
	char textHead;
} TEXTINFO;
#pragma pack(pop)
