#include <iostream>
#include <stdio.h>
#include "sqlite/sqlite3.h"

using namespace std;


void menu() {
    int option;
    cout << "1. Zaloguj sie \n";
    cout << "2. Zarejestruj sie \n";
    cout << "3. Wyjdz \n";

    cout << "Wybierz opcje: ";
    cin >> option;
}


int main()
{
    sqlite3 *db;
    int rc = sqlite3_open("test.db", &db);
    if (rc) {
        cout << "Brak polaczenia z baza, zamykam program.";
        sqlite3_close(db);
    }
    menu();
    sqlite3_close(db);
}