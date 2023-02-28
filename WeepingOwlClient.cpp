#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#define DEFAULT_PORT "23"
#define DEFAULT_BUFLEN 512

void sender(int iSendResult, int ClientSocket, char* sendbuf) {
	for (;;) {
		std::cin.getline(sendbuf, DEFAULT_BUFLEN);
		iSendResult = send(ClientSocket, sendbuf, strlen(sendbuf), 0);
	}

}
void listener(int iResult, int Socket, char* recvbuf, int recvbuflen, char* clientaddr) {
	for (;;) {

		try {
			iResult = recv(Socket, recvbuf, recvbuflen, 0);
			if (iResult == -1) {
				printf("\nConnection lost");
				close(Socket);
				exit(1);
			}
			// printf("\nBytes received: %d\n", iResult);

			if (iResult > 0) {
				for (int i = 0; i < DEFAULT_BUFLEN; i++) {
					if (recvbuf[i] == '\0') {
						// std::cout << std::endl;
						memset(recvbuf, NULL, DEFAULT_BUFLEN);
						break;
					}
					else {
						std::cout << recvbuf[i];
					}
				}

			}



		}

		catch (const std::exception& e) {

		}



	}
}
int main(int argc, char **argv) {
	if (argc < 3) {
		std::cout << "Not enough arguments, specify server and port(default port is 23)";
		return 1;
	}
	std::cout << "You are running WeepingOwl Linux Client release-1.0!\n";
	int iResult;
	struct addrinfo *result = NULL,  *ptr = NULL, hints;
	memset(&hints, NULL, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	iResult = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		return 1;
	}
	int ConnectSocket;
	ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	struct sockaddr_in client_addr;
	socklen_t addrlen = sizeof(client_addr);
	iResult = getpeername(ConnectSocket, (struct sockaddr*)&client_addr, &addrlen);
	char addrbuf[DEFAULT_BUFLEN];
	memset(addrbuf, NULL, DEFAULT_BUFLEN);
	std::cout << "CONNECTION ESTABILISHED TO " << inet_ntop(client_addr.sin_family, &client_addr.sin_addr, addrbuf, DEFAULT_BUFLEN) << std::endl;
	freeaddrinfo(result);
	int recvbuflen = DEFAULT_BUFLEN;
	char sendbuf[DEFAULT_BUFLEN];
	char recvbuf[DEFAULT_BUFLEN];
	memset(&recvbuf, NULL, DEFAULT_BUFLEN);
	int iSendResult;
	memset(&sendbuf, NULL, DEFAULT_BUFLEN);
	std::thread t_listener(&listener, iResult, ConnectSocket, recvbuf, recvbuflen, addrbuf);
	std::thread t_sender(&sender, iSendResult, ConnectSocket, sendbuf);
	t_listener.join();
	t_sender.join();
	close(ConnectSocket);
	return 0;
}
