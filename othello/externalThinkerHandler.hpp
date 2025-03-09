#pragma once
#include <WinSock2.h>
#include "externalThinkerMessages.hpp"
#include "messageGenerator.hpp"

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
	bool isVersionAvailable = false;
	unsigned _int8 version;
	bool isTextInfoAvailable = false;
	char textInfo[64];

public:
	ExternalThinkerHandler();
	~ExternalThinkerHandler();
	int init();
	int setParam(char* _hostname, int _port, HWND _hWnd);
	int setWindowHandle(HWND _hwnd);
	int sendInformationRequest();
	int sendThinkRequest(int turn, DISKCOLORS board[64], GameId gameId, HWND _hWnd);
	int receiveMessages();
	int recendMessage();
	int sendGameFinished(GameId gameId, DISKCOLORS diskcolor, RESULT result, HWND _hWnd);
	int getVersion(unsigned _int8* _version) {
		if (isVersionAvailable == false) return -1;
		*_version = version;
		return 0;
	}
	int getTextInfo(char** _textInfo) {
		if (isTextInfoAvailable == false) return -1;
		*_textInfo = textInfo;
		return 0;
	};
};