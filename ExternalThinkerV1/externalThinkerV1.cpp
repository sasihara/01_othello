//
//  ExternalThinkerV1.cpp
//  External thinker for othello version 1.00. 
//  

#include <iostream>
#include <WinSock2.h>		// Need to include before including "Windows.h", because it seems to include older version "winsock.h"
#include "ExternalThinkerMessages.hpp"
#include "othello.hpp"
#include "externalThinkerV1.hpp"
#include "think.hpp"
#include "messageGenerator.hpp"
#include "messageParser.hpp"

#pragma warning(disable:4996 6031 6305)

//
//	Function Name: main
//	Summary: Check commandline parameters, prepare the socket, wait for message and then handle the received message.
//
int main(int argc, char **argv)
{
    WSAData wsaData;
    SOCKET sock;
    struct sockaddr_in addr;
    struct sockaddr_in from;
    int sockaddr_in_size = sizeof(struct sockaddr_in);
    int port = 60001;
    char buf[4096];
    int messageLen;

    // Parameter check
    if (argc == 2) {
        port = atoi(argv[1]);
    }
    else if (argc > 2) {
        printf("\n\nUsage: ExternalThinkerV1 [port]\n\n");
    }

    if (port <= 1024) {
        printf("[ERROR] port number must be larger than 1024");
        return -1;
    }

    // Initialize winsock
    WSAStartup(MAKEWORD(2, 0), &wsaData);

	sock = socket(AF_INET, SOCK_DGRAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(sock, (struct sockaddr*) & addr, sizeof(addr));

    // Start to wait receiving requests
    printf("Othello thinker version 1.00.\n");
    printf("Waiting requests at port = %d...\n", port);

    // Receive and handle messages until QUIT message is received
    for (;;) {
        memset(buf, 0, sizeof(buf));
        messageLen = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&from, &sockaddr_in_size);

        // Message length check
        if (messageLen < sizeof(MESSAGEHEADER)) continue;

        // Parse the received message
        MessageParser messageParser;
        if (messageParser.SetParam(buf, messageLen) != 0) continue;

        // Get the message type
        MESSAGETYPE messageType;
        if (messageParser.getMessageType(&messageType) != 0) continue;

        // Go out this loop if QUIT message is received
        if (messageType == MESSAGETYPE::QUIT) break;

        // Handle the received message according to the message type
        switch (messageType) {
        case MESSAGETYPE::INFORMATION_REQUEST:  // Information Request
            HandleInformationRequest(messageParser, sock, from, sockaddr_in_size);
            break;
        case MESSAGETYPE::THINK_REQUEST:        // Think Request
            HandleThinkRequest(messageParser, sock, from, sockaddr_in_size);
            break;
        case MESSAGETYPE::THINK_STOP_REQUEST:   // Think Stop Request
            // Don't support this message.
            break;
        default:
            break;
        }

        // Release the message
        messageParser.free();
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}

//
//	Function Name: HandleInformationRequest
//	Summary: Handle Information Request message
//	
//	In:
//		messageParser       MessageParser instance of the received Information Request message
//      sock                socket to send the response message
//      from                The parameter to set the 5th parameter in sendto function to transmit the response message.  
//      sockaddr_in_size    The parameter to set the 6th parameter in sendto function to transmit the response message.
//
//	Return:
//      None
//
void HandleInformationRequest(MessageParser messageParser, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size)
{
    char respMessage[MAX_MESSAGE_LENGTH];

    // Generate Information Response message
    int messageLength;
    MessageGenerator messageGenerator;
    messageGenerator.SetParams(respMessage, MAX_MESSAGE_LENGTH);
    messageGenerator.makeMessageHeader(MESSAGETYPE::INFORMATION_RESPONSE);

    // Check if building the message finished successfully
    if ((messageLength = messageGenerator.getSize()) < 0) return;

    // Send INFORMATION_RESPONSE to the peer
    sendto(sock, respMessage, messageLength, 0, (struct sockaddr*)&from, sockaddr_in_size);
}

//
//	Function Name: HandleThinkRequest
//	Summary: Handle Think Request message
//	
//	In:
//		messageParser       MessageParser instance of the received Information Request message
//      sock                socket to send the response message
//      from                The parameter to set the 5th parameter in sendto function to transmit the response message.  
//      sockaddr_in_size    The parameter to set the 6th parameter in sendto function to transmit the response message.
//
//	Return:
//      None
//
void HandleThinkRequest(MessageParser messageParser, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size)
{
    int tlvHead = sizeof(MESSAGEHEADER);       // tlvHead: TLV head pos to be processed 
    DISKCOLORS board[8][8];
    int turn = 0;
    int id = 0;
    Thinker thinker;
    int ret;

    // Get data from received message
    if (messageParser.getTLVParamsBoard(board) != 0) return;        // Get the board data
    if (messageParser.getTLVParamsTurn(&turn) != 0) return;         // Get the turn value
    if (messageParser.getTLVParamsID(&id) != 0) return;             // Get the ID value

    // Send Think Accept message
    ret = SendThinkAccept(id, sock, from, sockaddr_in_size);

    // Think
    thinker.SetParams(turn, board);
    ret = thinker.think();

    // Send Think Response message
    ret = SendThinkResponse(id, (unsigned char) ret / 10, (unsigned char) ret % 10, sock, from, sockaddr_in_size);
}

//
//	Function Name: SendThinkAccept
//	Summary: Send Think Accept message
//	
//	In:
//		id                  Transaction ID received in Think Request message
//      sock                socket to send the response message
//      from                The parameter to set the 5th parameter in sendto function to transmit the response message.  
//      sockaddr_in_size    The parameter to set the 6th parameter in sendto function to transmit the response message.
//
//	Return:
//      0                   Succeed
//      -1                  Failed to build Think Accept message
//
int SendThinkAccept(int id, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size)
{
    char message[4096];
    MessageGenerator messageGenerator;

    // Build Think Accpest message
    messageGenerator.SetParams(message, sizeof(message));
    messageGenerator.makeMessageHeader(MESSAGETYPE::THINK_ACCEPT);
    messageGenerator.addTLVID(id);

    // Check if building the message has succeeded or not
    if (messageGenerator.getSize() <= 0) return -1;

    // Send the message
    sendto(sock, message, messageGenerator.getSize(), 0, (struct sockaddr*)&from, sockaddr_in_size);

    return 0;
}

//
//	Function Name: SendThinkResponse
//	Summary: Send Think Response message
//	
//	In:
//		id                  Transaction ID received in Think Request message
//      x                   The place to put the disk in x-axis
//      y                   The place to put the disk in y-axis
//      sock                socket to send the response message
//      from                The parameter to set the 5th parameter in sendto function to transmit the response message.  
//      sockaddr_in_size    The parameter to set the 6th parameter in sendto function to transmit the response message.
//
//	Return:
//      0                   Succeed
//      -1                  Failed to build Think Response message
//
int SendThinkResponse(int id, unsigned char x, unsigned char y, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size)
{
    char message[4096];
    MessageGenerator messageGenerator;

    // Build Think Response message
    messageGenerator.SetParams(message, sizeof(message));
    messageGenerator.makeMessageHeader(MESSAGETYPE::THINK_RESPONSE);
    messageGenerator.addTLVID(id);
    messageGenerator.addTLVPlace(x, y);

    // Check if building the message has succeeded or not
    if (messageGenerator.getSize() <= 0) return -1;

    // Send message
    sendto(sock, message, messageGenerator.getSize(), 0, (struct sockaddr*)&from, sockaddr_in_size);

    return 0;
}