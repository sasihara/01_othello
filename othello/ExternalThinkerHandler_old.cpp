#include <WinSock2.h>		// Need to include before including "Windows.h", because it seems to include older version "winsock.h"
#include <ws2tcpip.h>		// Need to include for using getaddrinfo
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include "othello.hpp"
#include "ExternalThinkerHandler.hpp"
#include "MessageGenerator.hpp"
#include "MessageParser.hpp"

#pragma warning(disable:4996 6305)

extern Gaming gaming;

ExternalThinkerHandler::ExternalThinkerHandler()
{
	// Initialize private variables
	state = PROTOCOLSTATES::INIT;
	hostname[0] = NULL;
	port = 0;
	sock = -1;
	nextReqId = 0;
	TimerIdWaitThinkAccept = 0;
	currentReqId = nextReqId = 0;

	memset(board, 0, sizeof(board));

	// Initialization for socket
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) return;
}

ExternalThinkerHandler::~ExternalThinkerHandler()
{
	freeaddrinfo(res0);		// ★ここで無条件に解放して良いのか考えること。コンストラクタでは確保しないので不要なケースもありうる。
}


//
//	Function Name: init
//	Summary: Initialize ExternalThinkerHandler.
//	
//	In:
//		No parameters.
//
//	Return:
//		0:	Success
//
int ExternalThinkerHandler::init()
{
	state = PROTOCOLSTATES::INIT;

	return 0;
}

//
//	Function Name: sendInformationRequest
//	Summary: Send information request to external thinker.
//	
//	In:
//		_hostname:	Host name in string which external thinker is running on.
//		_port:		Port number in string which external thinker is waiting for to receive. 
//
//	Return:
//		0:	Success
//		-1:	Host name is wrong.
//		-2:	Failed to create socket. Maybe port is already used.
//		-3:	External thinker is not ready.
//		-4:	Internal error.
//		-5:	Received message was wrong.
//		-6:	Received message was wrong.
//		-7:	Received message was wrong.
//
int ExternalThinkerHandler::sendInformationRequest(char* _hostname, int _port, HWND _hWnd)
{
	fd_set fds, readfds;

	// Store specified hostname and port
	strcpy_s(hostname, 256, _hostname);
	port = _port;

	// Get IP address from hostname
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	// For supporting IPv6, getaddrinfo should be used instead of gethostbyname()
	if (getaddrinfo(hostname, LOCAL_PORT_NUM_STR, &hints, &res0) != 0) {
		return -1;
	}

	// Prepare socket using possible parameter sets
	for (res = res0 ; res ; res = res->ai_next) {
		sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

		if (sock >= 0) break;
	}

	if (sock < 0) return -2;		// Failed to prepare socket

	// Set to receive ivents for the socket as WSA messages
	if (WSAAsyncSelect(sock, _hWnd, WSOCK_SELECT, FD_READ) == SOCKET_ERROR) {
		return -1;
	}

	// Generate message
	MESSAGEHEADER message;
	message.ProtocolVersion = PROTOCOL_VERSION;
	message.MessageLength = sizeof(MESSAGEHEADER);
	message.MessageType = (unsigned _int8)MESSAGETYPE::INFORMATION_REQUEST;

	// Send Information Request message to external thinker
	int nSize;
	nSize = sendto(sock, (const char *)&message, message.MessageLength, 0, res->ai_addr, (int)res->ai_addrlen);

	state = PROTOCOLSTATES::WAITING_INFORMATION_RESP;

	return 0;
	//// Set readfds to wait the response
	//FD_ZERO(&readfds);
	//FD_SET(sock, &readfds);

	//// Set timer for select
	//struct timeval tv;
	//tv.tv_sec = 5;
	//tv.tv_usec = 0;

	//// Wait to receive a valid message
	//memcpy(&fds, &readfds, sizeof(fd_set));

	//// Wait for 5 seconds
	//int nSockets;
	//nSockets = select(0, &fds, NULL, NULL, &tv);

	//// If timeout occur, initialization failed and return false
	//if (nSockets == 0) {
	//	return -3;
	//}
	//// No data received for sock, return false
	//else if (FD_ISSET(sock, &fds) == 0) {
	//	return -4;
	//}
	//else {
	//	char buf[1024];
	//	int nBytesReceived;

	//	// Receive data
	//	nBytesReceived = recv(sock, buf, sizeof(buf), 0);

	//	// Data size check
	//	if (nBytesReceived < sizeof(MESSAGEHEADER)) return false;

	//	// Received message check
	//	// Protocol version check
	//	if (((MESSAGEHEADER*)buf)->ProtocolVersion != PROTOCOL_VERSION) {
	//		return -5;
	//	}

	//	// Message type check
	//	if ((MESSAGETYPE)((MESSAGEHEADER*)buf)->MessageType != MESSAGETYPE::INFORMATION_RESPONSE) {
	//		return -6;
	//	}

	//	// Message length check
	//	if (((MESSAGEHEADER*)buf)->MessageLength != sizeof(MESSAGEHEADER)) {
	//		return -7;
	//	}

	//	// After checking to receive Information Response, return true.
	//	// Currently, other information (Version, Text Information) are not utilized.
	//	return 0;
	//}
}

//
//	Function Name: SetParams
//	Summary: Set parameters for thinker.
//	
//	In:
//		_turn:	The turn value.
//		_board:	The board data.
//
//	Return:
//		0: Succeed
//
//int ExternalThinkerHandler::SetParams(HWND _hWnd)
//{
//	hWnd = _hWnd;
//	return 0;
//}

//
//	Function Name: think
//	Summary: Send message to external thinker to find the best place for the current player.
//	
//	In:
//		No parameters.
//
//	Return:
//		0	Success
//		-1	Failed to set socket to WSA socket.
//		-2	Failed to set timer.
//
int ExternalThinkerHandler::think(int turn, DISKCOLORS board[8][8], HWND _hWnd)
{
	switch (state) {
	case PROTOCOLSTATES::INIT:
	{
		// Get new ID, transmit Think Request, start timer and then 
		// transit to WAITING_THINK_ACCEPT_RESP to wait for Think Accept.
		// At first, prepare memory for the sending message.
		int messageLength;
		char message[MAX_MESSAGE_LENGTH];
		MessageGenerator messageGenerator;

		messageGenerator.SetParams(message, MAX_MESSAGE_LENGTH);
		messageGenerator.makeMessageHeader(MESSAGETYPE::THINK_REQUEST);		// Header
		messageGenerator.addTLVID(nextReqId);								// ID TLV
		nextReqId++;
		messageGenerator.addTLVBoard(board);								// BOARD TLV
		messageGenerator.addTLVTURN(turn);									// TURN TLV

		// Check if the message was built successfully
		if ((messageLength = messageGenerator.getSize()) < 0) return -1;

		// Set timer for waiting the response
		TimerIdWaitThinkAccept = SetTimer(hWnd, (INT_PTR)TIMERID::WAIT_THINK_ACCEPT, WAIT_TIME_THINK_ACCEPT * 1000, NULL);

		if (TimerIdWaitThinkAccept == 0) {
			return -2;
		}

		// Set to receive ivents for the socket as WSA messages
		if (WSAAsyncSelect(sock, _hWnd, WSOCK_SELECT, FD_READ) == SOCKET_ERROR) {
			return -3;
		}

		// Send Think Request
		//int nSize = sendto(sock, (const char*)message, messageLength, 0, res->ai_addr, res->ai_addrlen);
		int nSize = sendto(sock, (const char*)message, messageLength, 0, res->ai_addr, res->ai_addrlen);

		// Check the result
		if (nSize == SOCKET_ERROR) {
			return -4;
		}

		// Transit to WAITING_THINK_ACCEPT_RESP
		state = PROTOCOLSTATES::WAITING_THINK_ACCEPT_RESP;

		break;
	}
	case PROTOCOLSTATES::WAITING_THINK_ACCEPT_RESP:
	case PROTOCOLSTATES::WAITING_THINK_RESP:
		// Cancel the timer running,
		// get new ID, transmit Think Request, start timer and then 
		// transit to WAITING_THINK_ACCEPT_RESP to wait for Think Accept.
		break;
	default:
		break;
	}

	return 0;
}

//
//	Function Name: think
//	Summary: Receive and process received message.
//	
//	In:
//		No parameters.
//
//	Return:
//
int ExternalThinkerHandler::receiveMessage()
{
	char buf[1024];
	int nBytesReceived;

	// Receive data
	nBytesReceived = recv(sock, buf, sizeof(buf), 0);

	// Data size check
	if (nBytesReceived < (int)sizeof(MESSAGEHEADER)) {
		return -1;
	}

	// Parse the message
	MessageParser messageParser;

	try {
		if (messageParser.SetParam(buf, nBytesReceived) != 0) {
			throw -2;
		}

		// Get the message type
		MESSAGETYPE messageType;
		if (messageParser.getMessageType(&messageType) != 0) {
			throw -3;
		}

		// Handle the received message according to the meesage type and the current state
		switch (messageType) {
		case MESSAGETYPE::THINK_ACCEPT:
			switch (state) {
			case PROTOCOLSTATES::WAITING_THINK_ACCEPT_RESP:
				// Transit to WAITING_THINK_RESP
				state = PROTOCOLSTATES::WAITING_THINK_RESP;
				break;
			default:
				break;
			}
			break;
		case MESSAGETYPE::THINK_REJECT:
			switch (state) {
			case PROTOCOLSTATES::WAITING_THINK_ACCEPT_RESP:
				// ★要実装
				break;
			default:
				break;
			}
			break;
		case MESSAGETYPE::THINK_RESPONSE:
			switch (state) {
			case PROTOCOLSTATES::WAITING_THINK_RESP:
				// Check the ID
				// ★要実装

				// Get the position
				int xPos, yPos;
				if (messageParser.getTLVParamsPlace(&xPos, &yPos) != 0) {
					throw -4;
				}

				// Transit to INIT state
				state = PROTOCOLSTATES::INIT;

				// Update board.
				gaming.PutDisk(xPos, yPos);

				// Check gameover and trigger thinker if necessary.
				switchToNextPlayer(hWnd);

				break;
			default:
				break;
			}
			break;
		case MESSAGETYPE::INFORMATION_RESPONSE:
			switch (state) {
			case PROTOCOLSTATES::WAITING_INFORMATION_RESP:
				// Transit to INIT state
				state = PROTOCOLSTATES::INIT;
				break;
			default:
				// Illegal message received
				throw -5;
				break;
			}
			break;
		case MESSAGETYPE::THINK_INDICATION:
			switch (state) {
			case PROTOCOLSTATES::WAITING_THINK_RESP:
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
	catch (int code)
	{
		messageParser.free();
		return code;
	}

	return 0;
}