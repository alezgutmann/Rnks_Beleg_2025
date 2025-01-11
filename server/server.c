#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>


//die beiden Zeilen machen, dass alles iwie funktioniert
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#define PORT 12345 // The port to listen on
const char* ipv6addr = "2a02:3100:2594:bb00:1c46:49fc:32d4:f72b"; //später wird diese variable noch als konsoleneingabe übernommen

//globale variablen:
WSADATA wsaData;
SOCKET sockfd;
struct sockaddr_in6 server_addr;
char daten[1024];

void socket_setup() {
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed\n");
		return ;
	}

	// Create the socket
	sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
	if (sockfd == INVALID_SOCKET) {
		perror("socket failed");
		WSACleanup();
		return ;
	}

	// Clear the sockaddr structure
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin6_family = AF_INET6;
	server_addr.sin6_port = htons(PORT);
	// Bind to a specific IPv6 address (replace with your desired address)

	if (inet_pton(AF_INET6, ipv6addr, &server_addr.sin6_addr) != 1) {
		printf("Invalid address format\n");
		closesocket(sockfd);
		WSACleanup();
		return ;
	}

	// Bind the socket
	if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		perror("bind failed");
		closesocket(sockfd);
		WSACleanup();
		return ;
	}

	printf("Server is listening on Port:%d\n", PORT);
}

void cleanup() {
	closesocket(sockfd);
	WSACleanup();
}

//teuflische funktion entnommen der folien 
void setnonblocking(SOCKET sock) { //SOCKET in Linux int
	u_long iMode = 1;
	int iResult;
	iResult = ioctlsocket(sock, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
		printf("ioctlsocket failed with error: %ld\n", iResult);
	return;
}

void server_listen() {
	size_t num_bytes;

	// Listen for incoming data
	while (1) {
		num_bytes = recvfrom(sockfd, daten, sizeof(daten) - 1, 0, NULL, NULL);
		if (num_bytes == SOCKET_ERROR) {
			//perror("recvfrom failed");
			continue;
		}

		daten[num_bytes] = '\0'; // Null-terminate the string
		printf("Received: %s\n", daten);
	}
}

int main() {
	
	

	socket_setup();
	setnonblocking(sockfd);

	server_listen();

	return 0;
}
