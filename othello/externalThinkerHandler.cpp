#include <WinSock2.h>		// Need to include before including "Windows.h", because it seems to include older version "winsock.h"
#include <ws2tcpip.h>		// Need to include for using getaddrinfo
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include "othello.hpp"
#include "externalThinkerHandler.hpp"
#include "messageGenerator.hpp"
#include "messageParser.hpp"

#pragma warning(disable:4996 6305)

extern Gaming gaming;

//
//	Function Name: ExternalThinkerHandler
//	Summary: Initialize private variables in ExternalThinkerHandler instance.
//	
//	In:
//		No parameters.
//
//	Return:
//		None.
//
ExternalThinkerHandler::ExternalThinkerHandler()
{
	// Initialize private variables
	state = PROTOCOLSTATES::INIT;
	hostname[0] = NULL;
	port = 0;
	sock = -1;
	currentReqId = 0;
	TimerIdWaitThinkAccept = 0;
	currentReqId = currentReqId = 0;

	memset(board, 0, sizeof(board));

	// Initialization for socket
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) return;
}

//
//	Function Name: ~ExternalThinkerHandler
//	Summary: Release resources, etc.
//	
//	In:
//		No parameters.
//
//	Return:
//		None.
//
ExternalThinkerHandler::~ExternalThinkerHandler()
{
	freeaddrinfo(res0);
}


//
//	Function Name: init
//	Summary: Initialize state and prepare UDP socket for external thinker
//	
//	In:
//		_hostname:	Host name in string which external thinker is running on.
//		_port:		Port number in string which external thinker is waiting for to receive. 
//		_hWnd		Window handle to receive messages from external thinker
//
//	Return:
//		0:	Success
//		-1:	Host name is wrong.
//		-2:	Failed to create socket. Maybe port is already used.
//		-3:	Failed in WSAASyncSelect()
//
int ExternalThinkerHandler::init(char* _hostname, int _port, HWND _hWnd)
{
	state = PROTOCOLSTATES::INIT;

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
	for (res = res0; res; res = res->ai_next) {
		sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

		if (sock >= 0) break;
	}

	if (sock < 0) return -2;		// Failed to prepare socket

	// Set to receive ivents for the socket as WSA messages
	if (WSAAsyncSelect(sock, _hWnd, WSOCK_SELECT, FD_READ) == SOCKET_ERROR) {
		return -3;
	}

	return 0;
}

//
//	Function Name: setWindowHandle
//	Summary: Update window handle
//	
//	In:
//		_hWnd		Window handle to receive messages from external thinker
//
//	Return:
//		0:	Success
//		-1:	Failed in WSAASyncSelect()
//
int ExternalThinkerHandler::setWindowHandle(HWND _hWnd)
{
	// Update hWnd private variable
	hWnd = _hWnd;

	// Set to receive ivents for the socket as WSA messages
	if (WSAAsyncSelect(sock, hWnd, WSOCK_SELECT, FD_READ) == SOCKET_ERROR) {
		return -1;
	}

	return 0;
}

//
//	Function Name: sendInformationRequest
//	Summary: Send information request to external thinker.
//	
//	In:
//		No parameters.
//
//	Return:
//		0:	Success
//		-1:	Failed in generating Information Request message.
//
int ExternalThinkerHandler::sendInformationRequest()
{
	// Information Request can be sent in any states, so checking state is not necessary here
	// Build Information Request message
	int messageLength;
	char message[MAX_MESSAGE_LENGTH];
	MessageGenerator messageGenerator;

	messageGenerator.SetParams(message, MAX_MESSAGE_LENGTH);
	messageGenerator.makeMessageHeader(MESSAGETYPE::INFORMATION_REQUEST);		// Header

	// Check if building the message finished successfully
	if ((messageLength = messageGenerator.getSize()) < 0) return -1;

	// Send Information Request message to external thinker
	int nSize;
	nSize = sendto(sock, (const char *)message, messageLength, 0, res->ai_addr, (int)res->ai_addrlen);

	state = PROTOCOLSTATES::WAITING_INFORMATION_RESP;

	return 0;
}

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
//		-3	Failed to set WSASyncSelect
//		-4	Failed to send ThinkRequest message
//
int ExternalThinkerHandler::sendThinkRequest(int turn, DISKCOLORS board[8][8], HWND _hWnd)
{
	switch (state) {
	case PROTOCOLSTATES::INIT:
		break;
	case PROTOCOLSTATES::THINKER_AVAILABLE:
	{
		// Get new ID, transmit Think Request, start timer and then 
		// transit to WAITING_THINK_ACCEPT_RESP to wait for Think Accept.
		// At first, prepare memory for the sending message.
		int messageLength;
		char message[MAX_MESSAGE_LENGTH];
		MessageGenerator messageGenerator;

		messageGenerator.SetParams(message, MAX_MESSAGE_LENGTH);
		messageGenerator.makeMessageHeader(MESSAGETYPE::THINK_REQUEST);		// Header
		messageGenerator.addTLVID(currentReqId);							// ID TLV
		messageGenerator.addTLVBoard(board);								// BOARD TLV
		messageGenerator.addTLVTURN(turn);									// TURN TLV

		// Check if building the message finished successfully
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
		int nSize = sendto(sock, (const char*)message, messageLength, 0, res->ai_addr, (int)res->ai_addrlen);

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
		// Currently Unsupported Feature
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
//		0		Succeed
//		-1		Received message is too small.
//		-2		Parsing the message was failed.
//		-3		Getting the message type was failed.
//		-4		Getting ID was failed.
//		-5		Getting the place was failed.
//		-6		Information Response message was received in an unexpected state
//
int ExternalThinkerHandler::receiveMessages()
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
				// Currently Unsupported Feature
				break;
			default:
				break;
			}
			break;
		case MESSAGETYPE::THINK_RESPONSE:
			switch (state) {
			case PROTOCOLSTATES::WAITING_THINK_RESP:
				// Check the ID
				int respId;
				messageParser.getTLVParamsID(&respId);
				if (respId != currentReqId) {
					throw -4;
				}

				// Get the position
				int xPos, yPos;
				if (messageParser.getTLVParamsPlace(&xPos, &yPos) != 0) {
					throw -5;
				}

				// Transit to INIT state
				state = PROTOCOLSTATES::THINKER_AVAILABLE;

				// Update board.
				if (gaming.PutDisk(xPos, yPos) < 0) {
					MessageBox(hWnd, TEXT("A wrong place has chosen by the thinker. Finish this game."), TEXT("Internal Error"), MB_OK | MB_ICONERROR);
					gaming.InitGame();
				}

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
				// Transit to THINKER_AVAILABLE state
				state = PROTOCOLSTATES::THINKER_AVAILABLE;
				break;
			default:
				// Illegal message received
				throw -6;
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

	messageParser.free();
	return 0;
}