#pragma once

class MessageParser {
public:
	int SetParam(char *message, int length);
	int getMessageType(MESSAGETYPE* _messageType);
	int getTLVParamsID(int* _id);
	int getTLVParamsBoard(DISKCOLORS _board[8][8]);
	int getTLVParamsTurn(int* _turn);
	int getTLVParamsPlace(int* _x, int* _y);
	int getTLVParamsReason();
	int getTLVParamsVersion();
	int getTLVParamsTextInfo();
	int free();

private:
	bool isMessageDataAvailable = false;
	char* message;
	int length;
	unsigned _int8 *messageType;

	bool isBoadDataAvailable = false;
	char* board;

	bool isTurnDataAvailable = false;
	int* turn;

	bool isIdDataAvailable = false;
	unsigned _int16* id;

	bool isPlaceDataAvailable = false;
	unsigned _int8* x, * y;
};