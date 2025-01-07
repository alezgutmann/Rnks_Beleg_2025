#include <stdio.h>
#include <Windows.h>
#include "timer.h"

#define mc_address 123456
#define MAX_MC_RECEIVER 3


void test() {//random funktion um durch printf festzustellen und auszugeben, ob andere funktionen funktionieren
	printf("Millisekunden: %ld\n", m_secs_now());
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
	int i = 0;
	int hello_ack_received = 0; //muss später noch durch logik auf true gesetzt werden
	printf("MC-Adresse: %d\n", mcadd);
	connection_prepare(mcadd);
	while (i < MAX_MC_RECEIVER) {
		printf("Warten...\t%d/%d\n", i, MAX_MC_RECEIVER);
		i++;
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
		//idle();
		test();
		if (send_hello(mc_address))
			;
	}

	
}

void test_timer() {
	for (int i = 1; i < 8; i++) {
		timer_add(i, i );
	}
	timer_del(6);
	for (int i = 0; i < 1000000000; i++);//warten um zeit zu vertreiben
	timer_display();
	//provozieren von Fehlermeldungen
	timer_add(1, 1);
	timer_del(60);
	//showcase von timer_check()
	for (int i = 1; i < 8; i++) {
		if (timer_check(i))
			printf("Timer Nr: %d ist abgelaufen :) \n", i);
	}
}

void main() {
	printf("Textausgabe !\n");
	test_timer();
	//zustandsdiagramm_sender();
}