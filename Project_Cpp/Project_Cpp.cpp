#include <iostream>
#include <stdio.h>
#include "sqlite/sqlite3.h"

using namespace std;

void logIn() {
    cout << "LOGOWANIE";
}
void signUp() {
    system("CLS");
    string login, pswd;
    cout << "Podaj login: ";
    cin >> login;
    cout << "Podaj haslo: ";
    cin >> pswd;
    cout << "PODANO: " << login << ", " << pswd;
}

void menu() {
    int option;
    cout << "1. Zaloguj sie \n";
    cout << "2. Zarejestruj sie \n";
    cout << "3. Wyjdz \n";

    cout << "Wybierz opcje: ";
    cin >> option;

    switch (option) {
        case 1: {
            system("CLS");
            logIn();
        }
        break;
        case 2: {
            system("CLS");
            signUp();
        }
        break;
        case 3: system("exit");
        break;
        default: {
            system("CLS");
            cout << "Nie ma takiej opcji, sprobuj ponownie.. \n";
            menu();
        }
    }
}


int main()
{
    sqlite3 *db;
    char* messageError;
    int rc = sqlite3_open("test.db", &db);

    if (rc) {
        cout << "Brak polaczenia z baza, zamykam program.";
        sqlite3_close(db);
        system("EXIT");
    }
    string sqlUserInit = "CREATE TABLE IF NOT EXISTS USER("
        "USER_ID INT PRIMARY KEY NOT NULL, "
        "NAME CHAR(50) NOT NULL, "
        "PASSWORD CHAR(50) NOT NULL);";
    rc = sqlite3_exec(db, sqlUserInit.c_str(), NULL, 0, &messageError);

    if (rc != SQLITE_OK) {
        cout << "Error with table USER: " << messageError;
        sqlite3_free(messageError);
    }

    string sqlSiteInit = "CREATE TABLE IF NOT EXISTS SITE("
        "SITE_ID INT PRIMARY KEY NOT NULL, "
        "NAME CHAR(50) NOT NULL, "
        "LOGIN CHAR(50) NOT NULL, "
        "PASSWORD CHAR(50) NOT NULL, "
        "USER_ID INT NOT NULL, "
        "FOREIGN KEY(USER_ID) REFERENCES USER(USER_ID));";
    rc = sqlite3_exec(db, sqlSiteInit.c_str(), NULL, 0, &messageError);

    if (rc != SQLITE_OK) {
        cout << "Error with table SITE: " << messageError;
        sqlite3_free(messageError);
    }

    menu();
    sqlite3_close(db);
}