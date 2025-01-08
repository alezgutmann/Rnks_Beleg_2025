# Rnks_Beleg_2025
Das hier ist das richtige repository (das andere ist ein hater).
Ihr könnt das Ding hier einfach mit Visual Studio Code klonen und darüber committen und son zeug.

!!! Zum testen müsst ihr auf euren Rechnern erst per
>ipconfig

herausfinden, welche "Temporäre IPv6-Adresse" ihr habt und die müsst ihr dann eingeben in das #define statement.
Somit kann die Verbindung richtig aufgebaut werden. 

> falls ihr "timeval" in ihren Folien seht: das ist vermutlich ihre art, um timer structs zu benennen (bei uns: TIMER), iwie ist timeval nämlich in linux, aber wir sind ja auf windows
>
> bezüglich ihrer Folien zu UDP und Sockets und so bin ich noch nicht schlau geworden -> Chagpt scheint die einfachere Lösung dazu zu sein, einfach nach "winsock" fragen
>


https://learn.microsoft.com/de-de/windows/win32/api/winsock2/nf-winsock2-socket
-> benötigen wir sock_stream oder sock_raw ?

* nicht vergessen: Logik soll im ersten Paket Ticks-To-Go finden und dann alle 300ms einen Tick dekrementieren...siehe Hinweisfolien
