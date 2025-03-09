#pragma once
#include "externalThinkerMessages.hpp"

class MessageParser {
public:
	int SetParam(char *message, int length);
	int getMessageType(MESSAGETYPE* _messageType);
	int getTLVParamsID(int* _id);
	int getTLVParamsBoard(DISKCOLORS _board[64]);
	int getTLVParamsTurn(int* _turn);
	int getTLVParamsPlace(int* _x, int* _y);
	int getTLVParamsReason();
	int getTLVParamsVersion(unsigned _int8 *_version);
	int getTLVParamsTextInfo(char *_textInfo, size_t _maxLength);
	int getTLVParamsGameId(GameId* _gameId);
	int getTLVParamsResult(RESULT* _result);
	int getTLVParamsDiskColor(DISKCOLORS* _diskcolor);

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

	bool isGameIdAvailable = false;
	TLV_GAMEID tlvGameid;

	bool isResultAvailable = false;
	RESULT result;

	bool isDiskColorAvailable = false;
	DISKCOLORS diskcolor;

	bool isVersionAvailable = false;
	unsigned _int8 version;

	bool isTextInfoAvailable = false;
	char* textInfoHead;
	size_t textInfoLengh;
};