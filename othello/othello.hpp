//
//	Othello(*1).hpp
//  Copyright(C) 2019 T.Sashihara
//	
//  (*1) TM＆© Othello,Co. and MegaHouse
//

#pragma once
#include "windows.h"
#include "resource.h"

// Constant
#define WAIT_TIME_INFO_RESP			5
#define WAIT_TIME_THINK_ACCEPT		5
#define WAIT_TIME_THINK_RESPONSE		30
#define	MAX_NUM_TRANSMISSION_THINK_REQUEST	3
#define WAIT_TIME_DISABLING_AUTOREPEAT		5

#define RESULT_FILENAME	"gameResults.csv"

#define	EMBEDED_THINKER_INFOTEXT	"Othello Thinker V1.01(Min-Max Based)"

// Messages
#define	WM_USER_TRIGGER_THINKER				(WM_USER + 1)
#define	WM_USER_TRIGGER_EXTERNAL_THINKER	(WM_USER + 2)
#define WSOCK_SELECT						(WM_USER + 3)

// Macros
#define ColorToPlayerIndex(color)	(PLAYERINDEX)((size_t)(color) - 1)
#define TurnToPlayerIndex(turn)		((turn) % 2)

#define	CURRENTPLAYER(turn)			(DISKCOLORS)(((turn) & 1) + 1)			// turn = even : 1 (COLOR_BLACK), turn = odd : 2 (COLOR_WHITE)
#define OPPONENT(diskcolor)			(DISKCOLORS)((((int)diskcolor) & 1) + 1)	// diskcolor = COLOR_BLACK : 2 (COLOR_WHITE), diskcolor = COLOR_WHITE : 1 (COLOR_BLACK)

// Enum
enum class GAME_STATES {
	STATE_INIT = 0,					// Game is not ongoing, user need to setup game conditions.
	STATE_GAMING,					// Game is ongoing, waiting user's input.
	STATE_GAMING_WAITING_RESP		// Game is ongoing, waiting enternal thinker's response.
};

enum class DIALOG_STATES {
	STATE_INIT = 0,
	STATE_WAITING_INFORESP_BLACK,
	STATE_WAITING_INFORESP_WHITE,
	STATE_END
};

enum class PLAYERINDEX {
	PLAYERINDEX_BLACK = 0,
	PLAYERINDEX_WHITE,
	PLAYERINDEX_LIMIT
};

enum class PLAYERTYPE {
	PLAYERTYPE_ERROR = -1,
	PLAYERTYPE_USER = 0,
	PLAYERTYPE_COMPUTER_EMBEDED,
	PLAYERTYPE_COMPUTER_EXTERNAL,
	PLAYERTYPE_LIMIT
};

enum class TIMERID {
	WAIT_INFO_RESP = 0,
	WAIT_DISABLING_AUTOREPEAT,
	WAIT_THINK_ACCEPT,
	WAIT_THINK_RESPONSE
};

enum class DISKCOLORS : unsigned _int16{
	COLOR_NONE = 0,		// No disk
	COLOR_BLACK,		// Black disk
	COLOR_WHITE,		// White disk
	COLOR_OUTOFBOAD		// Out of the board (used only in think.cpp)
};

typedef struct {
	PLAYERTYPE	PlayerType;
	TCHAR sHostname[256];
	TCHAR sPort[6];
} PLAYERINFO;

// Data type
typedef struct _GAMEID {
	time_t time;
	int pid;

	bool operator==(const _GAMEID& other) {
		if (time == other.time && pid == other.pid) return true;
		else return false;
	}
	bool operator!=(const _GAMEID& other) {
		if (time == other.time && pid == other.pid) return false;
		else return true;
	}
} GameId;

// Class
class Display {
private:
	HWND hWnd;
	HMENU hMenu;
	MENUITEMINFO menuInfo;
public:
	int SetParams(HWND hWnd);
	int UpdateBoard(bool playerMustPass = false);
	int DrawBoard(LPCWSTR windowTitle);
	int setAutoRepeatOnMenu(bool autoRepeat);
};

class Board {
private:
	DISKCOLORS board[64];
public:
	Board();
	int PutDisk(int x, int y, DISKCOLORS color);
	DISKCOLORS GetDisk(int x, int y);
	int InitBoard();
	int CopyBoard(DISKCOLORS board[64]);
};

class Gaming {
private:
	int turn;
	GAME_STATES state;
	int check(int xPos, int yPos, DISKCOLORS color);
	int checkOneDir(int xPos, int yPos, DISKCOLORS color, int xStep, int yStep);
	int turnDisk(int xPos, int yPos, int flag);
	int turnDiskOneDir(int xPos, int yPos, int xStep, int yStep);
	inline DISKCOLORS getCurrentColor() {
		return CURRENTPLAYER(turn);
	};
	GameId gameid;

public:
	bool autoRepeat = false;
	PLAYERINFO	playerInfo[2];

	Gaming();
	int PutDisk(int x, int y);
	int Pass();
	int InitGame();
	//int StartGame();
	int setState(GAME_STATES newState);
	bool IsPlayerMustPass();
	bool IsNextPlayerMustPass();
	bool IsGameOver();
	GAME_STATES getGameState();
	PLAYERTYPE getPlayerType(PLAYERINDEX player);
	PLAYERTYPE getCurrentPlayerType();
	PLAYERTYPE getOpponentPlayerType();
	int setPlayerType(PLAYERINDEX player, PLAYERTYPE playerType);
	LPCWSTR getWindowTitle();
	int getTurn();
	int setGameId();
	int getGameId(GameId* gameId);
	int getWinner(DISKCOLORS* winner, int* numBlack, int* numWhite);
	void incTurn() {
		turn++;
	}
};

// Functions
void displayGameOver(HWND hWnd, DISKCOLORS winner);
int storeGameResult(char* blackName, char* whiteName, DISKCOLORS winner, int numBlack, int numWhite);
void switchToNextPlayer(HWND hWnd);
int checkExternalThinker(HWND hDlg, PLAYERINDEX playerIndex);
void StartGame(HWND hDlg);