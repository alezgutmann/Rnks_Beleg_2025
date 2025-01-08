#pragma once
#include <Windows.h>
#include <stdlib.h>



//die beiden bösen Buben hier sind direkt aus den Hinweisfolien
long m_secs_now();
int timeval_subtract(struct timeval* result, struct timeval* x, struct timeval* y);//funktioniert wohl nur auf LINUX
//der Rest sind eigene teuflische Schöpfungen
void timer_add(int, int);
void timer_del(int);
long zeit_verbleibend(int nr); // gibt die zeit, die einem Timer noch verbleibt in Millisekunden zurück#pragma once
