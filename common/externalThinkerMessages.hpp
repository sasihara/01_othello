#pragma once
#include "othello.hpp"

// define
#define	PROTOCOL_VERSION	1

// Constant
#define BOARDSIZE_IN_BYTE	(sizeof(DISKCOLORS) * 64)

// Macros

// enum
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
	GAME_FINISHED,
	MESSAGETYPE_LIMIT
};

enum class TYPE {
	ID = 1,
	BOARD,
	TURN,
	PLACE,
	REASON,
	VERSION,
	TEXTINFO,
	TLVID_GAMEID,
	RESULT,
	DISKCOLOR
};

enum class RESULT {
	EVEN = 0,
	WIN,
	LOSE,
	RESULT_LIMIT
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

typedef struct _TLV_ID {
	unsigned _int16 id;
} TLV_ID;

typedef struct _TLV_BOARD {
	unsigned _int8 board[64];
} TLV_BOARD;

typedef struct _TLV_TURN {
	unsigned _int16 turn;
} TLV_TURN;

typedef struct _TLV_PLACE {
	unsigned _int8 x;
	unsigned _int8 y;
} TLV_PLACE;

typedef struct _TLV_REASON {
	unsigned _int8 reason;
} TLV_REASON;

typedef struct _TLV_VERSION {
	unsigned _int8 version;
} TLV_VERSION;

typedef struct _TLV_TEXTINFO {
	char textHead;
} TLV_TEXTINFO;

typedef struct _TLV_GAMEID {
	GameId gameId;
} TLV_GAMEID;

typedef struct _TLV_RESULT {
	RESULT result;
} TLV_RESULT;

typedef struct _TLV_DISKCOLOR {
	DISKCOLORS diskcolor;
} TLV_DISKCOLOR;
#pragma pack(pop)
