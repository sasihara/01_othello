#pragma once
#include "othello.hpp"

// Bit for disk character
#define DISKCHARFLAG_EXISTENCE	0x01		// Indicates disk exsits or not
#define DISKCHARFLAG_CHANGABLE	0x02		// Indicates disk can be changed or not

// Parameters for thinker
#define SEARCH_DEPTH		6
#define	FIXED_DISK_WEIGHT	24

enum class GAMESTATE {
	GAMESTATE_EARLY_STAGE = 0,
	GAMESTATE_MIDFIELD,
	GAMESTATE_END
};

class Thinker {
public:
	int SetParams(int turn, DISKCOLORS board[8][8]);
	int think();
	void analyzeDiskCharacter(DISKCOLORS board[8][8], int result[8][8]);

private:
	DISKCOLORS board[8][8];
	int turn;
	GAMESTATE thinkerState = GAMESTATE::GAMESTATE_EARLY_STAGE;
	DISKCOLORS currentPlayer, opponent;
	int CheckPosX[60] = {
		0, 0, 7, 7,
		2, 2, 3, 4, 5, 5, 3, 4,
		1, 1, 3, 4, 6, 6, 3, 4,
		2, 2, 5, 5,
		0, 0, 2, 5, 7, 7, 2, 5,
		1, 1, 2, 5, 6, 6, 2, 5,
		0, 0, 3, 4, 7, 7, 3, 4,
		0, 0, 1, 6, 7, 7, 1, 6,
		1, 1, 6, 6
	};
	
	int CheckPosY[60] = {
		0, 7, 0, 7,
		3, 4, 5, 5, 3, 4, 2, 2,
		3, 4, 6, 6, 3, 4, 1, 1,
		2, 5, 5, 2,
		2, 5, 7, 7, 2, 5, 0, 0,
		2, 5, 6, 6, 2, 5, 1, 1,
		3, 4, 7, 7, 3, 4, 0, 0,
		1, 6, 7, 7, 1, 6, 0, 0,
		1, 6, 1, 6
	};

	int weight[2][8][8] = {
		// Weight for Early stage (1 - 16)
		{
			 32,-24,  8,  2,  2,  8,-24, 32,
			-24,-24,-12, -4, -4,-12,-24,-24,
			  8,-12,  4,  1,  1,  4,-12,  8,
			  2, -4,  1,  1,  1,  1, -4,  2,
			  2, -4,  1,  1,  1,  1, -4,  2,
			  8,-12,  4,  1,  1,  4,-12,  8,
			-24,-24,-12, -4, -4,-12,-24,-24,
			 32,-24,  8,  2,  2,  8,-24, 32
		},
		// Weight for Mid stage (17 - 36)
		{
			 32,-24,  8,  2,  2,  8,-24, 32,
			-24,-24, -8, -4, -4, -8,-24,-24,
			  8, -8,  1,  1,  1,  1, -8,  8,
			  2, -4,  1,  4,  4,  1, -4,  2,
			  2, -4,  1,  4,  4,  1, -4,  2,
			  8, -8,  1,  1,  1,  1, -8,  8,
			-24,-24, -8, -4, -4, -8,-24,-24,
			 32,-24,  8,  2,  2,  8,-24, 32
		}
	};
	
	int CountDisk(DISKCOLORS color, DISKCOLORS _board[8][8]);
	int findBestPlaceForCurrentPlayer(int lv);
	int MaxLevel(int lv, int f, int beta, DISKCOLORS _board[8][8]);
	int MinLevel(int lv, int f, int alpha, DISKCOLORS _board[8][8]);
	int evcal(DISKCOLORS _board[8][8]);
	//void analyzeDiskCharacter(DISKCOLORS board[8][8], int result[8][8]);
	bool isFixed(int x, int y, DISKCOLORS board[8][8]);
	bool isFixedOneDir(int x, int y, DISKCOLORS board[8][8], int dx, int dy);
	bool isPatternToFix(int code);
	int check(DISKCOLORS board[8][8], int xPos, int yPos, DISKCOLORS color);
	int checkOneDir(DISKCOLORS board[8][8], int xPos, int yPos, DISKCOLORS color, int xStep, int yStep);
	int turnDisk(DISKCOLORS board[8][8], int xPos, int yPos, DISKCOLORS color, int flag);
	int turnDiskOneDir(DISKCOLORS board[8][8], int xPos, int yPos, DISKCOLORS color, int xStep, int yStep);

};