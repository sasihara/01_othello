#pragma once

// define
#define	PROTOCOL_VERSION	1

// Constant
#define BOARDSIZE_IN_BYTE	(sizeof(DISKCOLORS) * 64)

// Macros
#define	CURRENTPLAYER(turn)			(DISKCOLORS)((turn & 1) + 1)			// turn = even : 1 (COLOR_BLACK), turn = odd : 2 (COLOR_WHITE)
#define OPPONENT(diskcolor)			(DISKCOLORS)(((int)diskcolor & 1) + 1)	// diskcolor = COLOR_BLACK : 2 (COLOR_WHITE), diskcolor = COLOR_WHITE : 1 (COLOR_BLACK)

// enum
enum class DISKCOLORS {
	COLOR_NONE = 0,		// No disk
	COLOR_BLACK,		// Black disk
	COLOR_WHITE,		// White disk
	COLOR_OUTOFBOAD		// Out of the board (used only in think.cpp)
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
