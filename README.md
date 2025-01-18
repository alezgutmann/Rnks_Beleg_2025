# Rnks_Beleg_2025
Den Code verstehen:
* WICHTIG: die eingabe.txt darf nicht mehr zeilen besitzen als 2 * MAXWINDOWSIZE
* wir haben in spezifikationen.h quasi "codes" für helloACK, NACK und CloseACK festgelegt, die sind auf server- und client-seite gleich und werden genutzt um per strcmp() (stringcompare) herauszufinden, ob es sich um ein ACK handelt
* der TIMEOUT für receiving auf client seite ist statt 3 * TIMEOUT_INT (timeout intervall) nun 5 * TIMEOUT_INT, da bei nur 3 * schneller receioved wurde, als die Programmlogik des Servers ist
* das Empfangsfenster auf clientseite wird durch buffer_write() inkrementiert bzw gesteuert bzw zum ersten noch nicht empfangenen Paket gesetzt, dieses wird durch die Sequenznummer SN ermittelt
* Begriffe Clientseite: base - ist die SN, die als nächstes erwartet wird; SN ist die aktuell erhaltene SN, wird initialisiert mit 1; identifier - quasi Unicast Adressen, da sich zwei Clients auf die selbe Socket anmelden, müssen sie wissen, welche Nachricht für sie im Falle von selective Repeat bestimmt ist (id = 0 -> multicast; id = 1 -> unicast auf id = 1)
* Begriffe Serverseite: base - schaut in die Hinweisfolien, zum Pufferaufbau; nextSeq - die Sequenznummer, die als nächstes gesendet werden soll wird initialisiert mit 1, um semantische Integität zwischen Server und Client zu sichern.
