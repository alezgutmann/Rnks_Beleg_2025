#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h> 

//die beiden Zeilen machen, dass alles iwie funktioniert
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define PORT 12345 // The port to send data to
const char* ipv6addr = "2a02:3100:2594:bb00:1c46:49fc:32d4:f72b"; //später wird diese variable noch als konsoleneingabe übernommen


//globale variablen:
WSADATA wsaData;
SOCKET sockfd;
struct sockaddr_in6 server_addr;
FILE* file;


void socket_setup() {
    
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return;
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
    // Server's IPv6 address
    if (inet_pton(AF_INET6, ipv6addr, &server_addr.sin6_addr) != 1) {
        printf("Invalid address format\n");
        closesocket(sockfd);
        WSACleanup();
        return ;
    }

}

void client_senden(char* daten) {
    sendto(sockfd, daten, strlen(daten), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void cleanup() {
    fclose(file);
    closesocket(sockfd);
    WSACleanup();
}

int main() {    
    char daten[1024];//eventuell noch datenlänge von 1kb auf tatsächliche ipv6 länge oder sonstige spezifikation anpassen

    socket_setup();
    
    // Open the file for reading
    file = fopen("file.txt", "r");
    if (file == NULL) {
        perror("fopen failed");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Read the file and send the contents
    while (fgets(daten, sizeof(daten), file)) {
        client_senden(daten);
    }

    printf("File contents sent to server.\n");

    cleanup();
    return 0;
}
