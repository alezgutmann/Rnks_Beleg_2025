#include <string.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "timer.h"
#include "spezifikationen.h"



#pragma comment(lib, "ws2_32.lib")  // diese line macht dunkle magie, ohne sie geht es einfach nicht


//**konsoleneingabe:

//mögliche IP Adressen (auf MEINEM lokalem Rechner): "fe80::6e44:abc5:336b:c156", "fe80::6e44:abc5:336b:c155", "0::0"

char* ipv6_adresse;
char* multicast_gruppe;
int port;
char* filename;
int fenstergroeße;
int fehlerstelle1 = 10000;
int fehlerstelle2 = 10000;//hohe zahl, sodass bei keiner eingabe von fehlerstellen, die fehlerstellen nie erreicht werden

//**

//Globale Variablen für einfacheres Programmieren und einfachere Umsetzung von "Talking Code"
WSADATA wsaData;
SOCKET sock;
struct sockaddr_in6 multicastAddr, unicastAddr[MAX_MC_RECEIVER];//stores unicast adressen
int unicastAddrAnz = 0;

struct sockaddr_in6 clientAddr;
char message[MESSAGE_SIZE];
int myAddrSize = sizeof(clientAddr);
int error_result;
FILE* file;
struct package buffer[MAX_BUFFER];
struct package pack;
struct sockaddr nackAddr;
int nackAddrSize = sizeof(nackAddr);
int unicastAddrSize[MAX_MC_RECEIVER];

int base = 1;
int nextSeq = 0;


void setnonblocking(SOCKET sock) {
	u_long iMode = 1;
	int iResult;
	iResult = ioctlsocket(sock, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
		printf("ioctlsocket failed with error: %ld\n", iResult);
	return;
}

void socket_setup() {
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	// Create the UDP socket for sending data
	sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) {
		fprintf(stderr, "Socket creation failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}



	// Set up the destination multicast address
	memset(&multicastAddr, 0, sizeof(multicastAddr));
	multicastAddr.sin6_family = AF_INET6;
	multicastAddr.sin6_port = htons(port);
	inet_pton(AF_INET6, multicast_gruppe, &multicastAddr.sin6_addr);  // Multicast address

	memset(&clientAddr, 0, sizeof(clientAddr));
	clientAddr.sin6_family = AF_INET6;
	clientAddr.sin6_port = htons(port);

	if (inet_pton(AF_INET6, ipv6_adresse, &clientAddr.sin6_addr) < 0)
		printf("Konvertierung String zu IPv6 failed! Ungueltige IP-Adresse?\n");  // my address


	bind(sock, (struct sockaddr*)&clientAddr, sizeof(clientAddr));

	setnonblocking(sock);
}

void file_setup() {
	file = fopen(filename, "r");
	if (file == NULL) {
		// Check if the file was opened successfully
		printf("Error opening the file!\n");
		return 1;  // Exit with error code
	}
}

int parameterÜbernahme(int argc, char** argv) {
	//DAU Absicherung hier einbauen !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	if (argc == 0) {
		printf("Nutzung: <IPv6-Adresse Server> <Multicast-Gruppe Clients> <Portnummer> <einzulesende Datei> <Fenstergröße> <provozierte Fehlerstelle 1> <provozierte Fehlerstelle 2> \n");
		exit(0);
	}

	ipv6_adresse = argv[1];
	multicast_gruppe = argv[2];
	port = atoi(argv[3]);
	filename = argv[4];
	fenstergroeße = atoi(argv[5]);
	if (argc == 6)
		return;
	fehlerstelle1 = atoi(argv[6]);
	fehlerstelle2 = atoi(argv[7]);
}

void cleanup() {
	// Clean up and close the socket
	closesocket(sock);
	WSACleanup();
}

void print_myAddress() {
	// Convert the IPv6 address to a string
	char str[INET6_ADDRSTRLEN];
	if (inet_ntop(AF_INET6, &clientAddr.sin6_addr, str, sizeof(str)) == NULL) {
		printf("inet_ntop failed\n");
		WSACleanup();
		return 1;
	}
	printf("Meine IPv6 Adresse: %s\n", str);
}

int send_Packet(char* txt, int seq, int identifier) {//bei identifer = 0 sind alle multicast teilnehmer gemeint

	//vorberiten des paketes
	pack.identifier = identifier;
	pack.seqNummer = seq;
	strcpy(pack.message, txt);

	// Send the message to the multicast group
	error_result = sendto(sock, (char*)&pack, sizeof(struct package), 0, (struct sockaddr*)&multicastAddr, sizeof(multicastAddr));
	if (error_result == SOCKET_ERROR) {
		fprintf(stderr, "sendto failed with error: %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	//printf("Message sent to multicast group: %s, Seq: %d\n", pack.message, pack.seqNummer);
}

void check_Packets() {
	int bytesReceived = recvfrom(sock, message, sizeof(message), 0, NULL, NULL);
	if (bytesReceived > 0) {
		message[bytesReceived] = '\0';  // Null terminate the received data
		printf("Received NACK\n");
		return;  // Exit after receiving one acknowledgment
	}
	else if (bytesReceived == SOCKET_ERROR) {
		if (WSAGetLastError() == 10035)
			;//dieser error rührt nur daher, dass keine Daten zur ABholung bereit liegen 
		else
			fprintf(stderr, "recvfrom failed with error: %d\n", WSAGetLastError());
		return;
	}
}

void Send_Hello() {
	send_Packet(HELLO_ACK_MESSAGE, 0, 0);
}

int Connection_Prepare() {
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(sock, &rfds); // ConnSocket: socket-descriptor -> return value of socket() - call
	static struct timeval hardware_timer;
	static struct timeval* ptrHWtimer = NULL;
	int sel_res = 0;

	base = 1;
	nextSeq = 1;
	for (int i = 0; i < MAX_MC_RECEIVER; i++) {
		timer_del(1);
		timer_add(1, 2000);
		while (1) {
			FD_ZERO(&rfds);
			FD_SET(sock, &rfds);
			hardware_timer.tv_sec = 0;
			hardware_timer.tv_usec = 2000 * 1000;
			ptrHWtimer = &hardware_timer;
			sel_res = select(sock + 1, &rfds, NULL, NULL, ptrHWtimer); // according to sel_res a packet is received (then get it with recvfrom()), or a timeout occured

			if (sel_res > 0) {
				//printf("An der Socket liegt etwas an!\n");
				unicastAddrSize[unicastAddrAnz] = sizeof(unicastAddr[unicastAddrAnz]);
				int bytesReceived = recvfrom(sock, message, sizeof(message), 0, (struct sockaddr*)&unicastAddr[unicastAddrAnz], &unicastAddrSize[unicastAddrAnz]);

				if (bytesReceived > 0) {
					if (!strcmp(message, HELLO_ACK_MESSAGE)) {
						unicastAddrAnz++;
						// Convert the IPv6 address to a string
						char str[INET6_ADDRSTRLEN];
						if (inet_ntop(AF_INET6, &unicastAddr[unicastAddrAnz - 1].sin6_addr, str, sizeof(str)) == NULL) {
							printf("inet_ntop failed\n");
							WSACleanup();
							return 1;
						}
						printf("Received Hello-ACK from %s\n", str);
						break;  // verlassen while schleife und weiterschalten zum receiven des nächsten
					}


				}
				else if (bytesReceived == SOCKET_ERROR) {
					if (WSAGetLastError() == 10035)
						continue;//dieser error rührt nur daher, dass keine Daten zur ABholung bereit liegen 
					else
						fprintf(stderr, "Connection_Prepare() failed mit error: %d\n", WSAGetLastError());
					return 0;
				}
			}
			else if (sel_res == SOCKET_ERROR)
				printf("select() Error in ConnectionPrepare()\n");

			if (timer_check(1) == 1) {
				break;
			}

		}
	}
	timer_del(1);
	if (unicastAddrAnz != 0)
		return 1;
	else
		return 0;
}

void Connection_Established() {
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(sock, &rfds); // ConnSocket: socket-descriptor -> return value of socket() - call
	static struct timeval hardware_timer;
	static struct timeval* ptrHWtimer = NULL;
	int sel_res = 0;


	int timer_count = 0; //damit keine duplizierten timer gespeichert werden
	int fileZuEnde = 0;
	int closing = 0;
	int ACKsavailable = 1;
	while (1) {
		//senden der daten und puffernutzung:

		if (nextSeq < base + fenstergroeße) {
			//übernehmen der nachricht:   
			//FROMAPP quasi
			if (fgets(&pack.message, sizeof(pack.message), file) == NULL)
				fileZuEnde = 1;
			pack.identifier = 0;
			pack.seqNummer = nextSeq;
			if (!closing)
				buffer[nextSeq] = pack;

			//file zu ende gelesen, also nun abbau der verbindung
			if (fileZuEnde) {
				send_Packet(CLOSE_ACK_MESSAGE, nextSeq, 0);
				timer_del(nextSeq);//falls schon ein timer vorliegt, von einem vorherigem CLOSE_ACK
				closing = 1;
			}
			else if (nextSeq != fehlerstelle1 && nextSeq != fehlerstelle2)
				send_Packet(buffer[nextSeq].message, buffer[nextSeq].seqNummer, 0);

			timer_add(nextSeq, TIMEOUT_INT * 3);

			if (!closing)//damit nicht die seqnummern der closeacks hochgerzählt werden
				nextSeq++;
		}
		else if (nextSeq >= base + fenstergroeße)
			//Refuse to app layer
			;


		do
		{
			FD_ZERO(&rfds);
			FD_SET(sock, &rfds);
			//horchen nach NACKs:
			hardware_timer.tv_sec = 0;
			hardware_timer.tv_usec = 3 * (TIMEOUT_INT) * 1000;
			ptrHWtimer = &hardware_timer;
			sel_res = select(sock + 1, &rfds, NULL, NULL, ptrHWtimer); // according to sel_res a packet is received (then get it with recvfrom()), or a timeout occured

			if (sel_res > 0) {
				int bytesReceived = recvfrom(sock, &pack, sizeof(struct package), 0, NULL, NULL);
				if (strcmp(pack.message, CLOSE_ACK_MESSAGE) == 0) {
					printf("Close-ACK erhalten, schliesse jetzt!\n");
					return;
				}
				//printf("An der Socket liegt etwas an!\n");

				if (bytesReceived > 0) {
					//NACK Handling:
					if (!strcmp(pack.message, NACK_MESSAGE)) {
						int nackSN = pack.seqNummer;//die SN, welche ein NACK geschickt hat
						timer_del(nackSN);
						send_Packet(buffer[nackSN].message, buffer[nackSN].seqNummer, pack.identifier);
						timer_add(nackSN, TIMEOUT_INT);
						printf("------\nSende Retransmit an %d mit Seq: %d\n-----\n", pack.identifier, nackSN);
					}


				}
				else if (bytesReceived == SOCKET_ERROR) {
					if (WSAGetLastError() == 10035)
						continue;//dieser error rührt nur daher, dass keine Daten zur ABholung bereit liegen 
					else
						fprintf(stderr, "Connection_Prepare() failed mit error: %d\n", WSAGetLastError());
					return 0;
				}
			}
			else if (sel_res == SOCKET_ERROR) {
				printf("select() Error in ConnectionEstablished()\n");
				fprintf(stderr, "select() failed mit error: %d\n", WSAGetLastError());
			}
			else {
				ACKsavailable = 0;
			}

		} while (ACKsavailable);

		//Timeout check für timer, der zuerst abgelaufen ist

		int sn = 0;
		for (int i = base; i < nextSeq; i++) {
			sn = timer_check(i);
			if (sn == base)
			{
				timer_del(sn);
				base++;
				break;
			}
			else if (sn != -1)
				break;
		}




	}
}

void Send_Close() {
	send_Packet(CLOSE_ACK_MESSAGE, 0, nextSeq - 1);//nextSeq-1 = zu letzter gesendeter SN
}

void zustandsdiagramm() {
	do {
		Send_Hello();
	} while (!Connection_Prepare())
		;//break;
	Connection_Established();
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
