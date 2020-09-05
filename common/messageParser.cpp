#include <stdio.h>
#include "externalThinkerMessages.hpp"
#include "messageParser.hpp"

//
//	Function Name: SetParam
//	Summary: Set the address of received message and then parse the message
//	
//	In:
//      _message        Received message
//      _length         Length of the received message
//
//	Return:
//		0		Succeed
//		-1		Received message is too small
//      -2      Protocol version of received message is illegal
//
int MessageParser::SetParam(char* _message, int _length)
{
	// Store the parameters
	message = _message;
	length = _length;

	// Length check
	if (length < sizeof(MESSAGEHEADER)) return -1;

	// Protocol version check
	if (((MESSAGEHEADER*)message)->ProtocolVersion != PROTOCOL_VERSION) return -2;

	// Store the address of the message type
	messageType = &((MESSAGEHEADER*)message)->MessageType;

	// Parse TLVs
	int tlvHead = sizeof(MESSAGEHEADER), tlvTail;       // tlvHead: TLV head pos to be processed, tlvTail: TLV tail to be processed 
	int valueLen;

	// Read TLVs
	while (tlvHead + sizeof(TLV_HEADER) < length) {     // Check if another TLV exists or not
		// Get TLV data length and calculate the position of the tail of this TLV
		valueLen = (int)(TYPE)((TLV_HEADER*)&message[tlvHead])->Length;
		tlvTail = tlvHead + valueLen - 1;

        // Handle TLVs according to the type
        switch ((TYPE)((TLV_HEADER*)&message[tlvHead])->Type) {
        case TYPE::BOARD:
            // Check TLV data length
            if (valueLen != sizeof(TLV_HEADER) + 64) {
                continue;
            }

            // Store the board address
            board = &message[tlvHead + sizeof(TLV_HEADER)];
            isBoadDataAvailable = true;
            break;
        case TYPE::TURN:
            // Check TLV data length
            if (valueLen != sizeof(TLV_HEADER) + sizeof(TURN)) {
                continue;
            }

            // Store the address
            turn = (int*)&message[tlvHead + sizeof(TLV_HEADER)];
            isTurnDataAvailable = true;
            break;
        case TYPE::ID:
            // Check TLV data length
            if (valueLen != sizeof(TLV_HEADER) + sizeof(ID)) {
                continue;
            }

            // Store the address
            id = (unsigned _int16*)&message[tlvHead + sizeof(TLV_HEADER)];
            isIdDataAvailable = true;
            break;
        case TYPE::PLACE:
            // Check TLV data length
            if (valueLen != sizeof(TLV_HEADER) + sizeof(PLACE)) {
                continue;
            }

            // Store the place
            PLACE* place;
            place = (PLACE*)&message[tlvHead + sizeof(TLV_HEADER)];

            x = (unsigned _int8*)&place->x;
            y = (unsigned _int8*)&place->y;

            isPlaceDataAvailable = true;
            break;
        default:
            break;
        }

		// Move tlvHead
		tlvHead = tlvTail + 1;
	}

	// If the message is valid, set messageAvailable to true
    isMessageDataAvailable = true;

	return 0;
}

//
//	Function Name: getMessageType
//	Summary: Get the message type of the received message
//	
//	In:
//      _messageType        Address to store the received message type
//
//	Return:
//		0		Succeed
//		-1		Received message is not set yet.
//
int MessageParser::getMessageType(MESSAGETYPE *_messageType)
{
    // Availability check
    if (isMessageDataAvailable == false) return -1;

    *_messageType = (MESSAGETYPE)*messageType;

    return 0;
}

//
//	Function Name: getTLVParamsID
//	Summary: Get the ID of the received message
//	
//	In:
//      _id     Address to store the id
//
//	Return:
//		0		Succeed
//		-1		Received message is not set yet.
//      -2      ID TLV is not stored in the received message
//
int MessageParser::getTLVParamsID(int *_id)
{
    // Availability check
    if (isMessageDataAvailable == false) return -1;
    if (isIdDataAvailable == false) return -2;

    // Store the data
    *_id = (int)*id;

    // Return
    return 0;
}

//
//	Function Name: getTLVParamsBoard
//	Summary: Get the board data in the received message
//	
//	In:
//      _board  Address to store the board data
//
//	Return:
//		0		Succeed
//		-1		Received message is not set yet.
//      -2      ID TLV is not stored in the received message
//
int MessageParser::getTLVParamsBoard(DISKCOLORS _board[8][8])
{
    // Availability check
    if (isMessageDataAvailable == false) return -1;
    if (isBoadDataAvailable == false) return -2;

    // Store the data
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            _board[i][j] = (DISKCOLORS)board[i * 8 + j];
        }
    }

    // Return
    return 0;
}

//
//	Function Name: getTLVParamsTurn
//	Summary: Get the turn value in the received message
//	
//	In:
//      turn    Address to store the turn value
//
//	Return:
//		0		Succeed
//		-1		Received message is not set yet.
//      -2      ID TLV is not stored in the received message
//
int MessageParser::getTLVParamsTurn(int *_turn)
{
    // Availability check
    if (isMessageDataAvailable == false) return -1;
    if (isTurnDataAvailable == false) return -2;

    // Store the data
    *_turn = *turn;

    // Return
    return 0;
}

//
//	Function Name: getTLVParamsPlace
//	Summary: Get the place in the received message
//	
//	In:
//      _x      Address to store the place of x-axis
//      _y      Address to store the place of y-axis
//
//	Return:
//		0		Succeed
//		-1		Received message is not set yet.
//      -2      ID TLV is not stored in the received message
//
int MessageParser::getTLVParamsPlace(int *_x, int *_y)
{
    // Availability check
    if (isMessageDataAvailable == false) return -1;
    if (isPlaceDataAvailable == false) return -2;

    // Store the data
    *_x = (int)*x;
    *_y = (int)*y;

    // Return
    return 0;
}

//
//	Function Name: getTLVParamsReason (Currently unsupported)
//	Summary: Get the Reason code in the received message
//	
//	In:
//      None
//
//	Return:
//		0		Succeed
//		-1		Received message is not set yet.
//
int MessageParser::getTLVParamsReason()
{
    // Availability check
    if (isMessageDataAvailable == false) return -1;


    // Return
    return 0;
}

//
//	Function Name: getTLVParamsVersion (Currently unsupported)
//	Summary: Get the version data in the received message
//	
//	In:
//      None
//
//	Return:
//		0		Succeed
//		-1		Received message is not set yet.
//
int MessageParser::getTLVParamsVersion()
{
    // Availability check
    if (isMessageDataAvailable == false) return -1;


    // Return
    return 0;
}

//
//	Function Name: getTLVParamsTextInfo (Currently unsupported)
//	Summary: Get the text info data in the received message
//	
//	In:
//      None
//
//	Return:
//		0		Succeed
//		-1		Received message is not set yet.
//
int MessageParser::getTLVParamsTextInfo()
{
    // Availability check
    if (isMessageDataAvailable == false) return -1;


    // Return
    return 0;
}

//
//	Function Name: free
//	Summary: Free the received message
//	
//	In:
//      None
//
//	Return:
//		0		Succeed
//
int MessageParser::free()
{
    isMessageDataAvailable = false;
    isBoadDataAvailable = false;
    isTurnDataAvailable = false;
    isIdDataAvailable = false;
    isPlaceDataAvailable = false;

    return 0;
}