# Rnks_Beleg_2025
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

Nehmt NUR die Inhalte des Ordners Rnks_Beleg, der Rest funktioniert nicht, weiß aber auch nicht wie ich ihn löschen kann >:(

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Ladet euch einfach den Ordner runter und dann wieder hoch, nachdem ihr ihn bearbeitet habt.
Wenn jemand ganz lieb ist kann er die 'sender2' und 'client2' in 'server' (hab ausversehen falsch bennnt upsi) und 'client' umbenennen, hatte das jz noch nicht gemacht, aus angst dass es das ganze visual studio zerschießt.


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
* TODO: parameter über kommandozeile übernehmen, aber das erst mal aussen vor, erst prüfen, dass es korrekt funktioniert, danach erst kommandozeileneingabe implementieren
* TODO: eventuell noch datenlänge von 1kb auf tatsächliche ipv6 länge oder sonstige spezifikation anpassen
