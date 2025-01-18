#define NACK_MESSAGE "NACK!"
#define HELLO_ACK_MESSAGE "Hello-ACK!"
#define CLOSE_ACK_MESSAGE "Close-ACK!"
#define MESSAGE_SIZE 1024
#define MAX_MC_RECEIVER 1

#define TIMEOUT_INT 300 // in milliseconds
#define TIMEOUT 3 // must be a multiple of TIMEOUT_INT
#define MAX_WINDOW 10 // maximum windows size
#define MAX_SEQNR 2*MAX_WINDOW-1 // maximum sequence number --> real maximum sequence //number is 2 times of the choosen real window size!!
#define MAX_BUFFER 2*MAX_WINDOW // packets must be stored for retransmission

struct package {
	int identifier;
	int seqNummer;
	char message[MESSAGE_SIZE];
}
;

#pragma once
