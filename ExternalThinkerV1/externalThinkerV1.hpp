#pragma once
#include "messageParser.hpp"

void HandleInformationRequest(MessageParser messageParser, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size);
void HandleThinkRequest(MessageParser messageParser, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size);
int SendThinkAccept(int id, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size);
int SendThinkResponse(int id, unsigned char x, unsigned char y, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size);