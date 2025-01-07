#include "timer.h"
#define SECS_TO_MSECS 1000

//TODO:
//		y	testen, ob man malloc() nutzen muss, damit TIMER structs nicht verschwinden -> MUSS MAN!
//		y	timer liste testen -> funktioniert
//		y	timer_check() einbauen um zu prüfen, ob timer abgelaufen
//		y	timer_del() bauen -> funktioniert

//Idee des Timer struct:	einfach verkettete Liste, es wird jedes Mal neu berechnet, anhand der current time(durch m_secs_now()),
//							wie lange es her ist, seit dem der Timer gestartet wurde und dann gecheckt, ob der timer abgelaufen ist,
//							anhand seiner ablaufzeit

struct TIMER {
	long startzeit; //in millisekunden	
	long dauer;		//in millisekunden
	int nr;
	int* next;
};

struct TIMER kopf = { 0,0,0,0 };
//Timer-Cursor:
struct TIMER* current;
//Kopf-Timer:
struct TIMER* head = &kopf;

int timer_check(int nr) {// API um zu überprüfen, ob ein Timer abgelaufen ist, kann in if statement als wahrheitswert genutzt werden
	if (zeit_abgelaufen(nr))
		return nr;
	else return 0;
}

void timer_display() {
	current = head;
	current = current->next;//um das head-element nicht mit auszugeben
	while (current->next != 0) {
		printf("Timer Nr: %d\n", current->nr);
		printf("Startzeit: %ld\n", current->startzeit);
		printf("Dauer: %ld\n", current->dauer);
		printf("Zeit vergangen: %ld\n", zeit_gelaufen(current->nr));
		printf("Zeit uebrig: %ld\n\n", current->dauer - zeit_gelaufen(current->nr));

		current = current->next;
	}
	//um auch letztes Element auszugeben:
	printf("Timer Nr: %d\n", current->nr);
	printf("Startzeit: %ld\n", current->startzeit);
	printf("Dauer: %ld\n", current->dauer);
	printf("Zeit vergangen: %ld\n", zeit_gelaufen(current->nr));
	printf("Zeit uebrig: %ld\n\n", current->dauer - zeit_gelaufen(current->nr));
}

void timer_add(int nr, int dauer) {
	struct TIMER* tmp;
	
	int error = 0;
	
	current = head;
	while (current->next != 0) {
		if (current->nr == nr) {
			error = 1;
			printf("\n\n-----------------------\nDAP hat zugeschlagen,\n TIMER mit Nr: %d existiert bereits !\n-----------------------\n\n", nr);
			break;
		}
		current = current->next;//spulen bis zum ende
	}
	if (error) return;

	tmp = (struct TIMER*)malloc(sizeof(struct TIMER));

	tmp->nr = nr;
	tmp->dauer = dauer * SECS_TO_MSECS;
	tmp->startzeit = m_secs_now();
	tmp->next = 0;

	current->next = tmp;
}

void timer_del(int nr) {
	current = head;
	
	struct TIMER* prev = head; //temporärer Timer um einen Timer vor dem zu löschenden zu erhalten
	current = current->next;//eins vorspulen, falls dann schon richtiger Timer gefunden wird while schleife gar nicht betreten

	while (current->next != 0 && current->nr != nr) {
		current = current->next; //geht alle Timer durch, solange bis richtige nr gefunden
		prev = prev->next; //läuft immer eins hinterher
	}
		

	if (current->nr == nr)
	{
		//Umketten der Liste und frei machen des Speichers
		prev->next = current->next;
		free(current);
	}
	else {
		printf("\n\n-----------------------\nTimer mit Nr: %d existiert nicht ! -> DAP !\n-----------------------\n\n", nr);
	}

}

long zeit_gelaufen(int nr) {// gibt die zeit, die ein Timer schon existiert in Millisekunden zurück
	current = head;
	while (current->next != 0 && current->nr != nr) {
		current = current->next; //geht alle Timer durch, solange bis richtige nr gefunden
	}

	if (current->nr == nr)
	{
		return(m_secs_now() - current->startzeit);
	}
	else {
		printf("\n\n-----------------------\nTimer mit Nr: %d existiert nicht ! -> DAP !\n-----------------------\n\n", nr);
		return 0;
	}
	
}

int zeit_abgelaufen(int nr) {
	current = head;
	while (current->next != 0 && current->nr != nr) {
		current = current->next; //geht alle Timer durch, solange bis richtige nr gefunden
	}

	if (current->nr != nr){
		printf("\n\n-----------------------\nTimer mit Nr: %d existiert nicht ! -> DAP !\n-----------------------\n\n", nr);
	}

	if (zeit_gelaufen(nr) >= current->dauer)
	{
		return 1;
	}
	else {
		return 0;
	}

}


void timer_showcase() {
	for (int i = 1; i < 8; i++) {
		timer_add(i, i);
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

long m_secs_now() { // gibt Millissekunden zurück
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc;
	s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}




