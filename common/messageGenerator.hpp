#pragma once
#include "othello.hpp"

constexpr auto MAX_MESSAGE_LENGTH = 1024;

class MessageGenerator {
public:
	int SetParams(char* sendData, int sendDataMaxSize);
	int makeMessageHeader(MESSAGETYPE type);
	int addTLVID(int id);
	int addTLVPlace(unsigned _int8 x, unsigned _int8 y);
	int addTLVBoard(DISKCOLORS board[8][8]);
	int addTLVTURN(int turn);
	int getSize();
private:
	bool initilized = false;
	bool failed = false;
	char* sendData = NULL;
	int sendDataMaxSize = 0;
	int head = 0;
};