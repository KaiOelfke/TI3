/*
Bei UDP werden gleich große Datenpakete verschickt, waehrend bei TCP erstmal nur drei kleine Header geschickt werden.
Es handelt sich dabei vermutlich um den 3 Way Handshake. Ausserdem gibt es bei TCP viel mehr Optionen
und Konfigurationen im Header. Bei UDP werden fast nur die Daten geschickt und bei TCP wird viel mehr uberprueft.
Der Grund dafuer ist, dass es bei UDP der Client einfach die Daten schickt. Es gibt keine Ueberpruefung 
ob der Server die Daten bekommt. UDP ist also effizienter bei der Datenuebertragung, 
aber nicht zwangslaeufig bei der Informationsuebertragung, weil natuerlich Probleme auftreten koennen.
TCP hat mehr Overhead, aber ist sicherer und zuverlaessiger.


TCP - Headers:

No.     Time           Source                Destination           Protocol Length Info
     69 5.110015000    172.20.10.2           172.20.10.4           TCP      78     61333 > commplex-link [SYN] Seq=0 Win=65535 

No.     Time           Source                Destination           Protocol Length Info
     78 5.461905000    172.20.10.2           172.20.10.4           TCP      66     61333 > commplex-link [ACK] Seq=1 Ack=1 

No.     Time           Source                Destination           Protocol Length Info
     79 5.462009000    172.20.10.2           172.20.10.4           TCP      90     61333 > commplex-link [PSH, ACK] Seq=1 Ack=1 

TCP - Daten:

No.     Time           Source                Destination           Protocol Length Info
     81 5.462137000    172.20.10.2           172.20.10.4           TCP      1514   61333 > commplex-link [ACK] Seq=1473 Ack=1 

UDP - Daten:

No.     Time           Source                Destination           Protocol Length Info
     14 9.933255000    172.20.10.2           172.20.10.4           UDP      1512   Source port: 57606  Destination port: commplex-link


*/