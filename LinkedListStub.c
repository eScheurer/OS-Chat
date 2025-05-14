//
// Created by enyas on 12.05.25.
//
#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"  // Wichtig: muss den Pfad zu deiner Header-Datei korrekt angeben

void test_LL() {
    // Liste erstellen
    ThreadSafeList* list = create("TestChat");


    // Testdaten einfügen
    insert(list, "Alice: Hallo zusammen!");
    insert(list, "Bob: Hey Alice :)");
    insert(list, "Charlie: Hi alle!");
    saveToFile(list);


    // Liste ausgeben
    char* messages = formatMessagesForSending("TestChat");
    printf("%s\n", messages);


    // Aufräumen
    freeList(list);
}
