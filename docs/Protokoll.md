# Prototyp. Sitzung 2.4.2025
## Unser Prototyp soll können:
Kommunikation vorerst nur auf demselben Computer (localhost), alles ganz simpel
Auf Webseite:
* Knopf: der mittels Server kommunikation die aktuelle Zeit zurück gibt (um Webseite zu Server kommunikation zu etablieren)

### Infos für Prototyp
* Anfrage für Webseite: IP Adresse und Port eingeben in Browser, dann wird HTML zurückgeschickt.

### Filestruktur (für später)
* Server package (Server handling, main klasse),
* Webseite package (html file für struktur, dieses ruft methode aus JavaScript file (so kinda client) auf (dieses baut connection mit back end auf), css file für design(falls wir eine schöne webseite machen wollen),

# Nächste Schritte
Die die Zeit haben (Helene, Luis, bischen Cielle?), parallel/selbstständig Prototyp machen und dann Lösungen zusammensetzen. Nächste Sitzung 9.4.25 falls für alle möglich während der Übungsstunde, ansonsten wieder am morgen.

# Prototyp. Sitzung 9.4.2025
## Recap:
### Helene:
- Laufender lokaler Server aufgesetzt, aber connection über den Socket funktioniert noch nicht.
### Luis:
- Gute Vorlage auf Whatsapp
- Noch nicht auf git hochgeladen, aber funktioniert! -> Website wird angezeigt, Knopf-Druck zeigt Zeit auf Website
- Prototyp besteht aus: Javascript File, C File, HTML file
## Diskussionen
Makefiles benutzen!
### Helene: Kann alles in C geschrieben werden?
-> Browser können nicht C als Client Side websiten implementation interpretieren!  
Wir brauchen also HTML und JavaScript
### Enya: Wir müssen uns noch um den Raspberry Pi kümmern/nachfragen.
-> Enya übernimmt Kommunikation.
### Cielle: Was sind die nächsten Schritte?
Helene: Enya und Cielle arbeiten an nächsten Schritten?  
Luis: Grober Prototyp steht, das nächste Grundkonstrukt sollten Threads sein.  
Helene: Ist das Header-File wichtig?  
Konsens: Ja  
Helene: Dann sollten wir die beiden Prototypen kombinieren   
Luis: Habe die 2. Übungen noch nicht gemacht -> Da sind viele Infos über Threads drin die helfen könnten  
Helene: Ich würde gerne bereits dieses Wochenende daran arbeiten da ich danach ja nicht mehr verfügbar bin  
Luis: Dann schaue ich, dass ich es bis Freitag fertig schreibe und auf git pushe.  
Helene & Luis: Bisheriges System ist gut, hilft sehr die komplexität des Projekts herunterzubrechen  
Luis: Event-Driven -> Nicht ein Thread pro Client, sondern ein Thread der alle Clients handled.  
Helene: Ich hätte mich zuerst gerne an den Thread-Pool gesetzt  
Luis: Wir können ja vorerst an verschiedenen Aspekten arbeiten.  
Helene: Dann warten wir bis Luis den Prototypen hochgeladen hat, und dann arbeiten wir an unterschiedlichen Aspekten bis zur nächsten Sitzung  
Enya: 30. ist etwas spät für prototyp, was wenn etwas schiefgeht? Evt sollten wir uns vorher schon treffen und an etwasem Arbeiten.  
Helene: Dann trefft ihr euch doch in einer Woche um die nächsten Schritte zu definieren und auch evt schon daran zu arbeiten.  
Enya: Regelmässige Updates helfen sehr!  
Helene: Branches und pushes benutzen!  

### Nächste Sitzungen:
23.4.2025 - 10:00  
30.4.2025 - 10:00

## Nächste Aufgaben
- Prototyp auf git pushen (Luis)
- Thread-Pool (Helene & Enya)
- Event-Driven threading (Luis)
- Chat-Messages zu sich selber senden (Cielle)

# Sitzung 23.4.25
### Update Runde:
* Cielle: hat herausgefunden was in der Response ist, hat implementiert das HTML und JavaScript über selben Port gesendet wird.
* Luis: hat event driven approach umgesetzte
* Enya: hat ewigkeiten mit ihrem Setup verbracht, klappt immernoch nicht -> braucht hilfe.

### next Steps:
* Cielle: informiert sich zum thema one vs two ports benützen, http antworten extrahieren
* Enya: schaut dass ihr Setup funktioniert, machts sich dann an thread pooling
* Luis: linked lists für chatrooms thread save implementieren, (zb linked list mit name, zeit, id)

# Sitzung 30.4.25
### Recap
- Enya: Hat an Threadpool weitergearbeitet. Idee: Switchcase implementieren welcher Buffer-Inhalt überprüft und abhandelt,Buffer enthält Tasks
  - Vorschlag: Thread ruft Methode auf, Methode beinhalted Buffer und Switchcase -> Methode separat ausserhalb des Thread-Pools und server definieren.
  - Thread Anzahl wird alle 5 Sekunden überprüft und angepasst. Wenn ein Thread nichts zu tun hat, beendet er.
  - Threads starten und beenden sich selbst! Jedoch gibt es manchmal eine komische Verzögerung.
  - Vorschlag: Struct implementieren um Auslastung zu überprüfen (evt sogar an Website schicken?)
- Luis: Hat Threadsichere Linked-List erstellt.
### next steps
- Vorbereitung auf merge nächste Woche! (Alle)
- Auslastung auf Website anzeigen
- Playit.gg ausprobieren (Luis)
- HTML + JavaScript aufsetzen (Cielle)


# Sitzung 9.5.2025
## Beschlüsse
* momentan noch nicht unique user names, implementieren wenn noch wollen/zeit am ende
* Threadpool mementant einfach immer 1 neuer thread, potentiell später ändern dass mehrere neue erstellt werden

## Next steps
### things to do:
* merge erfolgreich machen
* nachrichten empfangen und senden können
* thread status an html senden
* einzelne chat rooms auswahl, mit pre string (evtl verschlüsselt)
* in chatrooms gehen können
* chats abspeichern können
* Domain name

- Helene: Chat prefix
- Enya: status von linked list schicken, quasi antwort für "hey schick mir aktuelle nachrichtn"
- Cielle: Nachrichten empfangen und in linked list

# 10.5.2025
TODO: Kommunikation zwischen request handler und html
* Helene: Rasperypi ausprobieren

# 4.6.2025 - Zwischenbilanz für Enspurt
## Wünsche für zusammen bearbeiten
* Klassendiagramm erstellen um verständnis zu fördern  ->  Update: haben wir erstellt, ist in resources zu finden
* Threadpool funktion zu momentanem Zeitpubkt besprechen für mehr Verständnis

## Aufgaben in Bearbeitung
* Nachrichten werden empfangen, Chat auf Nachricht extrahiert (Prefix) um Nachricht zu speichern, oder abzurufen @Cielle @Enya
* Server auf Pi @Helene

## Aufgaben todo
* regex Eingabe, username unique?, regex ohne ':' 
* Listen als Objekte während runtime @Cielle und , @Enya
* Option: Neue chats erstellen 
* Testing in the large

## Aufgaben prio zwei
* Webdesign (inkl. threadpool stats?)
* Präsentation vorbereiten
* Projektbericht schreiben
* Code aufräumen, Methoden und Klassennamen einheitlich
* Demo vorbereiten
* Buttom um chats zu speichern
* Benachrichtigungen wenn Person chat verlässt

# 5.6.2025
## Progress Report
* Enya: Messages werden isoliert geupdatet, heute soll dies integriert werden. Option neue Chats generieren ist als Funktion auf Website implementiert. Heute noch Defaultnachricht "has joind the chat" um automatisch logisch chat zu generieren. Plus Errornachricht implementieren, falls Chat in Logik generieren nicht funktioniert (Errorhandling).
* Cielle: Reader-Writer Problem genauer informiert um Locks sinnvoll zu zu implementieren. Neue Funktionen gecoded, die über ganzes Programm verwendet werden können.
* Helene: Raspberry pi mit OS läuft über Helene's Hotspot. Server läuft noch nicht darauf, wegen Kompliationsfehler. Wird am Freitag weiter erprobt. Heute Testing-Konzept und Programm erste prio 

# 6.6.2025
## Progress Report
* Enya: Repetitives Client connecting Fehler gefunden und gelöst. liste von chats kann ausgelesen und angezeigt werden. Mache jetzt bei der Funktion chat neu generieren und chat joinen.
* Cielle: Bin an Dokumentation von all meinen Befehlen und heute für eine neue Aufgabe zu haben, z.B.
* Helene: UNittests für LinkedList und Threadpool erstellt. Kleine Fehler in Threadpool entdeckt, möchte ich mit Enya anschauen (Update: war nur schriebfehler, nevermind). Habt ihr noch wichtige Komponenten die in Unit-tests getestet werden sollen? -> Cielle: chatList Methoden sind relevant. -> Helene wird also noch chatList Unittests erstellen und später an Integrated testing um Server zu populaten arbeiten

Gemeinsamer Entscheid: Wir machen keinen automatischen default Main chat. \\
Current Fehler: Cielle ist aufgefallen, dass Server abstürzt wenn 2. Website geöffnet und Chatlist ausgelesen werden soll \\


## Aufteilung Präsentation 18. min Präsi plus Demo
Pro person ca. 3min:
* Intro und Problem Statement @Cielle
* Server-Client Communication @Luis
* Threadpool und Queue (Step-by-Step Message folgen) @Helene
* dinamic Threadpool @Enya
* RequestHandler (Schnittstelle für Tasks) und task-Handler (Ent- und Verpackung, Weiterverarbeitung) @Enya
* Taskhandling: HTTP Requests @Cielle
* Taskhandling: ChatList @Cielle
* Taskhandling: LinkedList (Speicherung von Chats) @Luis
* Testing Populate Server with many Requests (Scalability) @Helene
* UI: Website, Hosting @Luis
* Server auf Raspberry-Pi @Helene (Übergang zu Demo)
* Demo zu UI und Threadpoolstats kurz was sagen @Enya Backup @Cielle 
* Dicussion and Lessons Learned @Helene 
\\
--> Präsentation Üben Donnerstag 12.6.2025 8:30 per Discord

## Nächstes Meeting 9.6.2025 13:00 Uhr und bis dahin:
* Offene Aufgaben erledigen
* Report zugeteilte Teile schreiben
* Präsentation Slides individuelle Teile schreiben
* 

# 9.6.2025
## Wünsche:
* Präsi und Report auf Overleaf mühsam wegen nicht alle gleichzeitig bearbeiten können. Bitte auf Google dock -> Helene fügt dann am Dienstag Abend alles zusammen.
* 
## Progress Report
* Luis: Wir haben schon mega viel, hab mal etwas getestet und funktioniert schon mega gut. -> Alle: Juhuii! \\
  Frage: Sockets offen halten nach Request funktioniert nicht ganz mit unserer implementation. Hat sich noch informiert über Websocket usw. Findet ihr, dass das noch viel Zeit wert ist? -> Funktionierende Basis ist relevanter, Projekt ist auch zeitlich terminiert. Einfach evtl. möchten im Report transparent kommunizieren, dass das unser nächster Schritt wäre mit offenen Sockets. \\
  Idee: Zurück Knopf zu Loginfenster und Threadstats anzeige richtig noch wichtig. \\
  Idee: Console output evtl. noch schöner organisieren -> bitte alle Code clean up machen. und Javadoc! \\
  Info: https ist mir relevanter, ich werde das nochmals versuchen. \\
  Info: Arbeite morgen an Report und Präsi und bin heute Nachmittag erreichbar wenn jemensch auf Pi testen will.

* Helene: Create chatraum immer noch möglich auch wenn name invalid und info über invalid Name stimmt? Bitte nachher gemeinsam testen \\
Frage: Was ist mit Bilder senden und so? Cielle: Ist implementiert als Messenge-Fenster Bild senden automatisch abgehandelt als nicht funktionierend. \\
Info: Ich Kümmere mich noch um Threadstats, die ein Probem zu haben scheinen (siehe Testsession unten)

* Enya Codingsachen sind erledigt, ist momentan am Report. Weitere Idee: wenn Chat verlassen wird Nachricht senden. Enya schaut sich an wenn Zeit. \\
Frage: Initial Threads und Intervall von dynamic Threads okay so? -> Ja hat sich is jetzt bewährt. \\
Frage: Update von Messenges auf einer Sekunde finde ich zu lange, können wir dies runter stellen? -> Ja testen wir gleich noch. \\
Info: Ich habe heute keine Zeit mehr ich arbeite morgen weiter an Präsi und Report. 

* Cielle: Habe morgen Dienstag keine Zeit. Finalisiere heute noch Präsi und Report. Mit Code bin ich somit zufrieden, werde aber entsprechend Luis auch noch Code aufräumen.

## Testsession
* Bei chat kreieren, error bei Messagebereich und keine test senden, auch nach Browser neu laden nicht funktioniert @Enya versucht dies zu korrigieren (evtl Cache und schon gelöst)
* Bodylength Error bei vielen Nachrichten. @Cielle hat schon herausgefunden wie gefixt wird, Bodlength wird in neueren Browser ausgelesen und muss nicht definiert werden.
* Tasks werden in Threadstats nur bei \#0 kumuliert. @Helene Threadstats müssen überprüft werden und notfalls nur im Server angezeigt werden nicht auf Website verfügbar.
* Chat kann auch mit nicht validem Chatname erstellt werden. Kleiner fix in html von @Helene
