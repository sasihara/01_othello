#include <memory.h>
#include "externalThinkerMessages.hpp"
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

	if (head + sizeof(TLV_HEADER) + sizeof(ID) < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::ID;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + sizeof(ID);
		head += sizeof(TLV_HEADER);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);

		((ID*)(&sendData[head]))->id = (unsigned _int16)id;
		head += sizeof(ID);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(ID);
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

	if (head + sizeof(TLV_HEADER) + sizeof(PLACE) < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::PLACE;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + sizeof(PLACE);
		head += sizeof(TLV_HEADER);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);

		((PLACE*)(&sendData[head]))->x = x;
		((PLACE*)(&sendData[head]))->y = y;
		head += sizeof(PLACE);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(PLACE);
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

		memcpy(((BOARD*)(&sendData[head]))->board, board, BOARDSIZE_IN_BYTE);
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

	if (head + sizeof(TLV_HEADER) + sizeof(TURN) < sendDataMaxSize) {
		((TLV_HEADER*)(&sendData[head]))->Type = (unsigned _int8)TYPE::TURN;
		((TLV_HEADER*)(&sendData[head]))->Length = sizeof(TLV_HEADER) + sizeof(TURN);
		head += sizeof(TLV_HEADER);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TLV_HEADER);

		((TURN*)(&sendData[head]))->turn = turn;
		head += sizeof(TURN);
		((MESSAGEHEADER*)(&sendData[0]))->MessageLength += sizeof(TURN);
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