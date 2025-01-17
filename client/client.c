#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "spezifikationen.h"

#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

//**konsoleneingabe:

//mögliche IP Adressen (auf MEINEM lokalem Rechner): "fe80::6e44:abc5:336b:c156", "fe80::6e44:abc5:336b:c155", "0::0"

char* ipv6_adresse;
char* multicast_gruppe;
int port;
char* filename;
int fenstergroeße;
int fehlerquote;
int identifier;

//**




//Globale Variablen für einfacheres Programmieren und einfachere Umsetzung von "Talking Code"
WSADATA wsaData;
SOCKET sock;
struct sockaddr_in6 multicastAddr, serverAddr, clientAddr;
struct ipv6_mreq multicastRequest;
char message[MESSAGE_SIZE];
int serverAddrSize = sizeof(serverAddr);
int clientAddrSize = sizeof(clientAddr);
FILE* file;
struct package buffer[MAX_BUFFER];
int geschrieben_bis = 0;

void setnonblocking(SOCKET sock) {
	u_long iMode = 1;
	int iResult;
	iResult = ioctlsocket(sock, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
		printf("ioctlsocket failed with error: %ld\n", iResult);
	return;
}

void socket_setup() {
	int error_result;

	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	// Create the UDP socket for receiving data
	sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) {
		fprintf(stderr, "Socket creation failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	int trueValue = 1; //setsockopt loopback
	/* Reusing port for several server listening to same multicast addr and port (testing on local machine only) */
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&trueValue, sizeof(trueValue));


	// Set up the local address to listen on the multicast group
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin6_family = AF_INET6;
	serverAddr.sin6_port = htons(port);

	inet_pton(AF_INET6, ipv6_adresse, &serverAddr.sin6_addr);//client address


	// Bind the socket to the multicast port
	error_result = bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (error_result == SOCKET_ERROR) {
		fprintf(stderr, "bind failed with error: %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// Set up the multicast group address to join
	inet_pton(AF_INET6, multicast_gruppe, &multicastRequest.ipv6mr_multiaddr);
	multicastRequest.ipv6mr_interface = 0;  // 0 means all interfaces

	// Join the multicast group
	error_result = setsockopt(sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char*)&multicastRequest, sizeof(multicastRequest));
	if (error_result == SOCKET_ERROR) {
		fprintf(stderr, "setsockopt IPV6_ADD_MEMBERSHIP failed with error: %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return 1;
	}



	printf("Receiver is listening for multicast messages...\n");
}

void file_setup() {
	file = fopen(filename, "w");
	if (file == NULL) {
		// Check if the file was opened successfully
		printf("Error opening the file!\n");
		return 1;  // Exit with error code
	}
}

void file_write(char* txt) {
	fprintf(file, txt);
}

void print_myAddress() {
	// Convert the IPv6 address to a string
	char str[INET6_ADDRSTRLEN];
	if (inet_ntop(AF_INET6, &serverAddr.sin6_addr, str, sizeof(str)) == NULL) {
		printf("inet_ntop failed\n");
		WSACleanup();
		return 1;
	}
	printf("Meine IPv6 Adresse: %s\n", str);
}

void cleanup() {
	// Clean up and close the socket
	closesocket(sock);
	WSACleanup();
	fclose(file);
}

int parameterÜbernahme(int argc, char** argv) {
	//DAU Absicherung hier einbauen !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	ipv6_adresse = argv[1];
	multicast_gruppe = argv[2];
	port = atoi(argv[3]);
	filename = argv[4];
	fenstergroeße = atoi(argv[5]);
	fehlerquote = atoi(argv[6]);
	identifier = atoi(argv[7]);
}

struct package pack;
int empfange() {
	int bytesReceived = recvfrom(sock, &pack, sizeof(struct package), 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
	if (buffer_findSN(pack.seqNummer)) return 0;//um duplizierte pakete zu vermeiden

	if (bytesReceived > 0 && (pack.identifier == 0 || pack.identifier == identifier) /*&& (strcmp(pack.message, NACK_MESSAGE) != 0)*/) { //strcmp mit NACK message, da Problem aufgetrtetn ist, dass sie ihre eigenen NACKS aufgenommen haben
		//message[bytesReceived] = '\0';  // Null terminate the received data
		printf("Sequenznummer: %d\n", pack.seqNummer);
		printf("Erhaltene Nachricht: %s\n", pack.message);
		return 1;
	}
	else if (bytesReceived == SOCKET_ERROR) {
		fprintf(stderr, "recvfrom failed with error: %d\n", WSAGetLastError());
		return 0;
	}
	else {
		return 0;
	}
}

void sendNACK(int seqn) {
	int error_result;
	pack.identifier = identifier;
	pack.seqNummer = seqn;
	strcpy(pack.message, NACK_MESSAGE);
	// Send ACK back to sender
	error_result = sendto(sock, (char*)&pack, sizeof(struct package), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
	if (error_result == SOCKET_ERROR) {
		fprintf(stderr, "sendto failed with error: %d\n", WSAGetLastError());
		return;
	}
	printf("Sent NACK to sender. Mit SN = %d\n", pack.seqNummer);
}

struct package pack;

int base = 1;
int SN = 0;
int buffer_Anz = 0;//wie viele elemente in buffer gepuffert sind
void ConnectionPrepare() {
	base = 1;
	SN = 1;
	buffer_Anz = 0;
	int bytesReceived = recvfrom(sock, &pack, sizeof(struct package), 0, (struct sockaddr*)&clientAddr, &clientAddrSize);

	if (bytesReceived > 0) {
		if (strcmp(pack.message, HELLO_ACK_MESSAGE) == 0) {
			printf("Hello-ACK-Request erhalten, sende Hello-ACK !\n");
			int error_result;
			// Send ACK back to sender
			error_result = sendto(sock, HELLO_ACK_MESSAGE, strlen(HELLO_ACK_MESSAGE), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
			if (error_result == SOCKET_ERROR) {
				fprintf(stderr, "sendto failed with error: %d\n", WSAGetLastError());
				return;
			}
			printf("Sent HelloACK to sender.\n");
		}
	}
	else if (bytesReceived == SOCKET_ERROR) {
		if (WSAGetLastError() == 10035)
			;//dieser error rührt nur daher, dass keine Daten zur ABholung bereit liegen 
		else
			fprintf(stderr, "Connection_Prepare() failed mit error: %d\n", WSAGetLastError());
		return 0;
	}

	setnonblocking(sock);//erst jetzt auf nonblocking setzen
}

void buffer_add(struct package p) {
	if (buffer_findSN(p.seqNummer)) //wenn das paket schon gespeichert wurde
		return;
	if (buffer_Anz == MAX_BUFFER) {
		//rotieren des Puffers um 1 nach links
		for (int i = 0; i < buffer_Anz; i++) {
			buffer[i] = buffer[i + 1];
		}
	}
	else
	{
		buffer[buffer_Anz] = p;
		buffer_Anz++;
	}
}

int buffer_findSN(int sn) {
	for (int i = 0; i < buffer_Anz; i++) {
		if (buffer[i].seqNummer == sn)
			return 1;
	}
	return 0;
}

//void buffer_remove(int sn) {
//	for (int i = 0; i < buffer_Anz; i++) {
//		if (buffer[i].seqNummer == sn) {
//			for (int j = i; j < buffer_Anz - 1; j++)
//			{
//				buffer[j]= buffer[j + 1];
//			}
//			buffer_Anz--;
//		}
//	}
//}

void buffer_write() {//falls der buffer in die datei geschrieben werden kann, tut er das und löscht die gschreibenen daten raus
	for (int i = 0; i < buffer_Anz; i++) {
		if (base == buffer[i].seqNummer) {
			file_write(buffer[i].message);
			//buffer_remove(buffer[i].seqNummer);
			base++;
			i = 0;
		}
	}
}

int lowest_unreceived_sn() {
	//hier hocharbeiten,bis sn gefunden, die nicht erhalten wurde
	for (int i = base; i < MAX_BUFFER + base; i++) {
		if (!buffer_findSN(i))//bricht sofort ab, wenn die inkrementierte sn nicht gefundn wurde, also die erste ist, die nicht received wurde
			return i;
	}

}

void ConnectionEstablished() {
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(sock, &rfds); // ConnSocket: socket-descriptor -> return value of socket() - call
	static struct timeval hardware_timer;
	static struct timeval* ptrHWtimer = NULL;
	int sel_res = 0;

	while (1) {

		//buffer_write();//periodisch wird veruscht buffer in file zu schreiben, falls die gepufferten SN dies zulassen

		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);
		hardware_timer.tv_sec = 0;
		hardware_timer.tv_usec = 3 * (TIMEOUT_INT) * 1000;
		ptrHWtimer = &hardware_timer;
		sel_res = select(sock + 1, &rfds, NULL, NULL, ptrHWtimer); // according to sel_res a packet is received (then get it with recvfrom()), or a timeout occured

		if (sel_res > 0) {
			//printf("An der Socket liegt etwas an!\n");
			if (!empfange())//falls die anliegende Nachricht nicht für mich bestimmt war
				continue;
			SN = pack.seqNummer;
			if (base <= SN <= base + fenstergroeße) {
				//zuerst checken, ob es ein Closerequest ist:
				if (strcmp(pack.message, CLOSE_ACK_MESSAGE) == 0) {
					printf("Close-ACK-Request erhalten!\n");
					int error_result;
					// Send ACK back to sender
					if (pack.seqNummer != base)
					{
						sendNACK(base);
						continue;
					}
					else
					{
						error_result = sendto(sock, &pack, sizeof(struct package), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
						if (error_result == SOCKET_ERROR) {
							fprintf(stderr, "sendto failed with error: %d\n", WSAGetLastError());
							return;
						}
						printf("Sent CloseACK to sender.\n");
						return;
					}
				}
				buffer_add(pack);
				if (base != SN) {
					sendNACK(base);
				}
				else {
					buffer_write();
					//base = lowest_unreceived_sn();
				}
			}
		}
		else if (sel_res == SOCKET_ERROR)
		{
			printf("select() Error in ConnectionEstablished()\n");
			fprintf(stderr, "select() failed mit error: %d\n", WSAGetLastError());
		}
		else {
			sendNACK(base);
		}
	}
}



void zustandsdiagramm() {
	//while (1) {
	ConnectionPrepare();
	ConnectionEstablished();
	//}
}

int main(int argc, char* argv) {
	parameterÜbernahme(argc, argv);
	socket_setup();
	file_setup();
	print_myAddress();

	zustandsdiagramm();

	cleanup();

	return 0;
}
