#include <memory.h>
#include "externalThinkerMessages.hpp"
#include "othello.hpp"
#include "messageGenerator.hpp"

//
//	Function Name: SetParams
//	Summary: Set the address of the work space to make the message and its maximum size, and initialize private variables.
//	
//	In:
//		_sendData			Work space to make the message
//		_sendDataMaxSize	Maximum size of the work space
//
//	Return:
//		0					Success
//
int MessageGenerator::SetParams(char* _sendData, int _sendDataMaxSize)
{
	sendData = _sendData;
	sendDataMaxSize = _sendDataMaxSize;
	head = 0;
	initilized = true;
	failed = false;

	return 0;
}

//
//	Function Name: makeMessageHeader
//	Summary: Add the message header to the work space
//	
//	In:
//		type		Message type
//
//	Return:
//		0:	Success
//		-1: SetParams() is not called yet
//		-2:	Not enough buffer size
//
int MessageGenerator::makeMessageHeader(MESSAGETYPE type)
{
	if (initilized == false) return -1;

	if (sizeof(MESSAGEHEADER) < sendDataMaxSize) {
		((MESSAGEHEADER*)(&sendData[0]))->ProtocolVersion = PROTOCOL_VERSION;
		((MESSAGEHEADER*)(&sendData[0]))->MessageType = (unsigned _int8)type;
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength = sizeof(MESSAGEHEADER);
		head = sizeof(MESSAGEHEADER);
	}
	else {
		failed = true;
		return -2;
	}

	return 0;
}

//
//	Function Name: makeTLVID
//	Summary: Add ID TLV to specified message
//	
//	In:
//		id		ID value to store the TLV
//
//	Return:
//		0		Succeed
//		-1		Work space is not enough
//
int MessageGenerator::addTLVID(int id)
{
	if (initilized == false) return -1;

	if (head + sizeof(TLV_HEADER) + sizeof(TLV_ID) < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::ID;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + sizeof(TLV_ID);
		head += sizeof(TLV_HEADER);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);

		((TLV_ID*)(&sendData[head]))->id = (unsigned _int16)id;
		head += sizeof(TLV_ID);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_ID);
	}
	else {
		failed = true;
		return -1;
	}

	return 0;
}

//
//	Function Name: addTLVPlace
//	Summary: Add Place TLV to specified message
//	
//	In:
//		_x			The place in x-axis
//		_y			The place in y-axis
//
//	Return:
//		0		Succeed
//		-1		Work space is not enough
//
int MessageGenerator::addTLVPlace(unsigned _int8 x, unsigned _int8 y)
{
	if (initilized == false) return -1;

	if (head + sizeof(TLV_HEADER) + sizeof(TLV_PLACE) < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::PLACE;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + sizeof(TLV_PLACE);
		head += sizeof(TLV_HEADER);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);

		((TLV_PLACE*)(&sendData[head]))->x = x;
		((TLV_PLACE*)(&sendData[head]))->y = y;
		head += sizeof(TLV_PLACE);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_PLACE);
	}
	else {
		failed = true;
		return -1;
	}

	return 0;
}

//
//	Function Name: addTLVBoard
//	Summary: Add Board TLV to specified message
//	
//	In:
//		board[8][8]		board data to transmit
//
//	Return:
//		0		Succeed
//		-1		Work space is not enough
//
int MessageGenerator::addTLVBoard(DISKCOLORS board[64])
{
	if (initilized == false) return -1;

	if (head + sizeof(TLV_HEADER) + sizeof(DISKCOLORS) * 64 < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::BOARD;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + sizeof(DISKCOLORS) * 64;
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);
		head += sizeof(TLV_HEADER);

		memcpy(((TLV_BOARD*)(&sendData[head]))->board, board, BOARDSIZE_IN_BYTE);
		head += BOARDSIZE_IN_BYTE;
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += BOARDSIZE_IN_BYTE;
	}
	else {
		failed = true;
		return -1;
	}

	return 0;
}

//
//	Function Name: addTLVTURN
//	Summary: Add Turn TLV to specified message
//	
//	In:
//		turn		turn value
//
//	Return:
//		0		Succeed
//		-1		Work space is not enough
//
int MessageGenerator::addTLVTURN(int turn)
{
	if (initilized == false) return -1;

	if (head + sizeof(TLV_HEADER) + sizeof(TLV_TURN) < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::TURN;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + sizeof(TLV_TURN);
		head += sizeof(TLV_HEADER);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);

		((TLV_TURN*)(&sendData[head]))->turn = turn;
		head += sizeof(TLV_TURN);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_TURN);
	}
	else {
		failed = true;
		return -1;
	}

	return 0;
}

int MessageGenerator::addTLVGameId(GameId _gameId)
{
	if (initilized == false) return -1;

	if (head + sizeof(TLV_HEADER) + sizeof(TLV_GAMEID) < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::TLVID_GAMEID;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + sizeof(TLV_GAMEID);
		head += sizeof(TLV_HEADER);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);
		((TLV_GAMEID*)(&sendData[head]))->gameId.time = _gameId.time;
		((TLV_GAMEID*)(&sendData[head]))->gameId.pid = _gameId.pid;
		head += sizeof(TLV_GAMEID);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_GAMEID);
	}
	else {
		failed = true;
		return -1;
	}

	return 0;
}

int MessageGenerator::addTLVResult(RESULT winner)
{
	if (initilized == false) return -1;

	if (head + sizeof(TLV_HEADER) + sizeof(TLV_RESULT) < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::RESULT;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + sizeof(TLV_RESULT);
		head += sizeof(TLV_HEADER);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);
		((TLV_RESULT*)(&sendData[head]))->result = winner;
		head += sizeof(TLV_RESULT);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_RESULT);
	}
	else {
		failed = true;
		return -1;
	}

	return 0;
}

int MessageGenerator::addTLVDiskColor(DISKCOLORS diskcolor)
{
	if (initilized == false) return -1;

	if (head + sizeof(TLV_HEADER) + sizeof(TLV_DISKCOLOR) < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::DISKCOLOR;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + sizeof(TLV_DISKCOLOR);
		head += sizeof(TLV_HEADER);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);
		((TLV_DISKCOLOR*)(&sendData[head]))->diskcolor = diskcolor;
		head += sizeof(TLV_DISKCOLOR);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_DISKCOLOR);
	}
	else {
		failed = true;
		return -1;
	}

	return 0;
}

int MessageGenerator::addTLVVersion(unsigned _int8 version)
{
	if (initilized == false) return -1;

	if (head + sizeof(TLV_HEADER) + sizeof(TLV_VERSION) < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::VERSION;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + sizeof(TLV_VERSION);
		head += sizeof(TLV_HEADER);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);
		((TLV_VERSION*)(&sendData[head]))->version = version;
		head += sizeof(TLV_VERSION);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_VERSION);
	}
	else {
		failed = true;
		return -1;
	}

	return 0;
}

int MessageGenerator::addTLVTextInfo(const char *textInfo)
{
	if (initilized == false) return -1;

	size_t textInfoLength = strlen(textInfo);

	if (head + sizeof(TLV_HEADER) + textInfoLength < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::TEXTINFO;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + textInfoLength;
		head += sizeof(TLV_HEADER);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);
		strcpy_s(&((TLV_TEXTINFO*)(&sendData[head]))->textHead, sendDataMaxSize - head - sizeof(TLV_HEADER), textInfo);

		head += textInfoLength;
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += textInfoLength;
	}
	else {
		failed = true;
		return -1;
	}

	return 0;
}

//
//	Function Name: getSize
//	Summary: Get the size of the current message
//	
//	In:
//
//	Return:
//		>=0:	The size of the message.
//		-1:		Not initalized yet.
//		-2:		Building the message was failed.
//
int MessageGenerator::getSize()
{
	if (initilized == false) return -1;
	if (failed == true) return -2;

	return ((MESSAGEHEADER*)(&sendData[0]))->MessageLength;
}