#pragma once
#include <WinSock2.h>
#include "externalThinkerMessages.hpp"
#include "messageGenerator.hpp"

#define LOCAL_PORT_NUM_STR		"60001"
#define	WAIT_TIME_THINK_ACCEPT	5	// seconds	
#define	MAX_RETRANS				3

// enum
enum class PROTOCOLSTATES {
	INIT = 0,
	SOCKET_READY,
	WAITING_INFORMATION_RESP,
	THINKER_AVAILABLE,
	WAITING_THINK_STOP_RESP,
	WAITING_THINK_ACCEPT_RESP,
	WAITING_THINK_RESP,
	PROTOCOLSTATES_LIMIT
};

// Class
class ExternalThinkerHandler {
private:
	PROTOCOLSTATES state;
	char hostname[256];
	int port;
	WSAData wsaData;
	SOCKET sock;
	struct addrinfo hints, *res0, *res;
	DISKCOLORS board[8][8];
	int turn;
	int currentReqId;
	HWND hWnd;
	UINT_PTR TimerIdWaitThinkAccept;
	UINT_PTR TimerIdWaitThinkResponse;
	int messageLength;
	char message[MAX_MESSAGE_LENGTH];
	int numRetrans = 0;

public:
	ExternalThinkerHandler();
	~ExternalThinkerHandler();
	int init();
	int setParam(char* _hostname, int _port, HWND _hWnd);
	int setWindowHandle(HWND _hwnd);
	int sendInformationRequest();
	int sendThinkRequest(int turn, DISKCOLORS board[8][8], HWND _hWnd);
	int receiveMessages();
	int recendMessage();
};