#pragma once
#include <stdio.h>
#include "othello.hpp"
#include "externalThinkerMessages.hpp"

constexpr auto MAX_MESSAGE_LENGTH = 4096;

class MessageGenerator {
public:
	int SetParams(char* sendData, int sendDataMaxSize);
	int makeMessageHeader(MESSAGETYPE type);
	int addTLVID(unsigned int id);
	int addTLVPlace(unsigned _int8 x, unsigned _int8 y);
	int addTLVBoard(DISKCOLORS board[64]);
	int addTLVTURN(int turn);
	int addTLVGameId(GameId gameId);
	int addTLVResult(RESULT result);
	int addTLVDiskColor(DISKCOLORS diskcolor);
	int addTLVVersion(unsigned _int8 version);
	int addTLVTextInfo(const char* textInfo);
	int getSize();
private:
	bool initilized = false;
	bool failed = false;
	char* sendData = NULL;
	int sendDataMaxSize = 0;
	int head = 0;
};