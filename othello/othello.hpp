//
//	Othello(*1).hpp
//  Copyright(C) 2019 T.Sashihara
//	
//  (*1) TM＆© Othello,Co. and MegaHouse
//

#pragma once

#include "resource.h"

// Constant
#define BOARDSIZE_IN_BYTE	(sizeof(DISKCOLORS) * 64)

// Messages
#define	WM_USER_TRIGGER_THINKER	(WM_USER + 1)

// Macros
#define	CURRENTPLAYER(turn) (DISKCOLORS)((turn & 1) + 1)			// turn = even : 1 (COLOR_BLACK), turn = odd : 2 (COLOR_WHITE)
#define OPPONENT(diskcolor)	(DISKCOLORS)(((int)diskcolor & 1) + 1)	// diskcolor = COLOR_BLACK : 2 (COLOR_WHITE), diskcolor = COLOR_WHITE : 1 (COLOR_BLACK)
#define ColorToIndex(color)	(int)(color - 1)

// Enum
enum class GAME_STATES {
	STATE_INIT = 0,		// Game is not ongoing, user need to setup game conditions.
	STATE_GAMING		// Game is ongoing.
};

enum class PLAYERINDEX {
	PLAYERINDEX_BLACK = 0,
	PLAYERINDEX_WHITE,
	PLAYERINDEX_LIMIT
};

enum class DISKCOLORS {
	COLOR_NONE = 0,		// No disk
	COLOR_BLACK,		// Black disk
	COLOR_WHITE,		// White disk
	COLOR_OUTOFBOAD		// Out of the board (used only in think.cpp)
};

enum class PLAYERTYPE {
	PLAYERTYPE_ERROR = -1,
	PLAYERTYPE_USER = 0,
	PLAYERTYPE_COMPUTER_EMBEDED,
	PLAYERTYPE_COMPUTER_EXTERNAL,
	PLAYERTYPE_LIMIT
};

typedef struct {
	PLAYERTYPE	PlayerType;
} PLAYERINFO;

// Class
class Display {
private:
	HWND hWnd;
	HMENU hMenu;
	MENUITEMINFO menuInfo;
public:
	int SetParams(HWND hWnd);
	int UpdateBoard(bool playerMustPass = false);
	int DrawBoard(LPCTSTR windowTitle);
};

class Board {
private:
	DISKCOLORS board[8][8];
public:
	Board();
	int PutDisk(int x, int y, DISKCOLORS color);
	DISKCOLORS GetDisk(int x, int y);
	int InitBoard();
	int CopyBoard(DISKCOLORS board[8][8]);
};

class Gaming {
private:
	int turn;
	GAME_STATES state;
	PLAYERINFO	playerInfo[2];
	int check(int xPos, int yPos, DISKCOLORS color);
	int checkOneDir(int xPos, int yPos, DISKCOLORS color, int xStep, int yStep);
	int turnDisk(int xPos, int yPos, int flag);
	int turnDiskOneDir(int xPos, int yPos, int xStep, int yStep);
	inline DISKCOLORS getCurrentColor() {
		return CURRENTPLAYER(turn);
	};

public:
	Gaming();
	int PutDisk(int x, int y);
	int Pass();
	int InitGame();
	int StartGame();
	bool IsPlayerMustPass();
	bool IsNextPlayerMustPass();
	bool IsGameOver();
	GAME_STATES getGameState();
	PLAYERTYPE getPlayerType(PLAYERINDEX player);
	PLAYERTYPE getCurrentPlayerType();
	PLAYERTYPE getOpponentPlayerType();
	int setPlayerType(PLAYERINDEX player, PLAYERTYPE playerType);
	LPCTSTR getWindowTitle();
	int getTurn();
};

// Functions
void displayGameOver(HWND hWnd);
void switchToNextPlayer(HWND hWnd);
