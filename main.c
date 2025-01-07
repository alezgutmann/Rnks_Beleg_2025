#include <stdio.h>
#include <Windows.h>
#include <winsock.h>
#include "timer.h"
#include "udp.h"

#define mc_address 123456
#define MAX_MC_RECEIVER 3


void test() {//random funktion um durch printf festzustellen und auszugeben, ob andere funktionen funktionieren
	
}


int dau_jn() {//returns non zero wenn korrekte eingabe, als antwort auf Ja-Nein Frage
	char adresse;
	
	scanf("%c", &adresse);
	if (adresse == 'j') {
		while (fgetc(stdin) != '\n')
			;
		return 1;
	}
	while (fgetc(stdin) != '\n')
		;
	return 0;
}

int connection_prepare(int mcadd) {
	
};

int send_hello(int mcadd) {
	int i = 1;
	int hello_ack_received = 0; //muss später noch durch logik auf true gesetzt werden
	printf("MC-Adresse: %d\n", mcadd);
	
	connection_prepare(mcadd);
	
	while (i <= MAX_MC_RECEIVER) {
		timer_add(2, 2);
		printf("Warten...\t%d/%d\n", i, MAX_MC_RECEIVER);


		//hier noch iwie warten, dass jmd antwortet und seine unicast adresse speichern


		while (!timer_check(2));//wartet solange bis 2-sekunden-timer abläuft
		i++;
		timer_del(2);
	}

	if (hello_ack_received) {

	}
	else {
		printf("kein Hello-ACK erhalten :(\n");
	}
}

void idle() {
	while (1) {
		printf("Wollen Sie senden ?(j/n)\n");
		if (dau_jn()) break;
	}
		
}

void zustandsdiagramm_sender() {
	while (1) {
		idle();
		test();
		if (send_hello(mc_address))
			;
	}

	
}


void main() {
	printf("Textausgabe !\n");
	//timer_showcase();
	zustandsdiagramm_sender();
}