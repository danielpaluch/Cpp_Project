#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>

using namespace std;

class User {
public:
    int id = -1;
    string login = "";
};
User user;

void menu(sqlite3* db);
void loggedUserMenu(sqlite3* db) {
    int option;
    cout << "Witaj " + user.login + ", co chcesz zrobic?";
    cout << "1. Wyswietl witryny \n";
    cout << "2. Dodaj witryne \n";
    cout << "3. Wyloguj \n";

    cout << "Wybierz opcje: ";
    cin >> option;

    switch (option) {
    case 1: { // Wyswietl witryny
        system("CLS");
        logIn(db);
    }
          break;
    case 2: { // Dodaj witryne
        system("CLS");
        signUp(db);
    }
          break;
    case 3: system("exit"); // Wyloguj
        break;
    default: {
        system("CLS");
        cout << "Nie ma takiej opcji, sprobuj ponownie.. \n";
        menu(db);
    }
    }

};
void logIn(sqlite3* db) {
    string login, pswd;
    sqlite3_stmt* stmt;
    cout << "Login: ";
    cin >> login;
    cout << "Hasło: ";
    cin >> pswd;

    string sql_query = "SELECT * FROM users WHERE (name = ? AND password = ?)";
    sqlite3_prepare_v2(db, sql_query.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pswd.c_str(), -1, SQLITE_TRANSIENT);

    int result_code = sqlite3_step(stmt);
    if (result_code == SQLITE_ROW) { // ZNALEZIONO
        int id = sqlite3_column_int(stmt, 0);
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        user.id = id;
        user.login = name; // "zalogowanie uzytkownika";
        sqlite3_finalize(stmt);
        loggedUserMenu(db);
    }
    else {
        cout << "Bledne dane, sprobuj jeszcze raz: ";
        logIn(db);
    }
}
void signUp(sqlite3* db) {
    sqlite3_stmt* stmt;
    char* err_msg;
    string login, pswd;
    cout << "Podaj login: ";
    cin >> login;
    cout << "Podaj haslo: ";
    cin >> pswd;

    if (login == "" || pswd == "") { //50znakow
        cout << "Niepoprawne hasło lub login.. \n";
        menu(db);
    }
    string sql_query = "SELECT * FROM users WHERE name = ?";
    sqlite3_prepare_v2(db, sql_query.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);

    int result_code = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result_code != SQLITE_ROW && result_code == SQLITE_DONE) {
        // brak uzytkownika o takim loginie
        sql_query = "INSERT INTO users (name, password) VALUES ('" + login + "', '" + pswd + "')";
        int rc = sqlite3_exec(db, sql_query.c_str(), nullptr, nullptr, &err_msg);
        cout << rc;
        if (rc != SQLITE_OK) {
            cout << "Blad podczas dodawania: " << err_msg << endl;
            sqlite3_free(err_msg);
            sqlite3_close(db);
        }
        system("CLS");
        cout << "Pomyslnie zarejestrowano! \n";
        menu(db);
    }
    else {
        system("CLS");
        cout << "Taki użytkownik już istnieje, sprobuj ponownie \n";
        signUp(db);
    }
}

void menu(sqlite3* db) {
    int option;
    cout << "1. Zaloguj sie \n";
    cout << "2. Zarejestruj sie \n";
    cout << "3. Wyjdz \n";

    string sql_query = "SELECT * FROM users";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql_query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cout << "Error preparing SELECT statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* pswd = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        cout << "ID: " << id << ", LOGIN: " << name << ", PASSWORD: " << pswd << endl;
    }
    sqlite3_finalize(stmt);

    cout << "Wybierz opcje: ";
    cin >> option;

    switch (option) {
    case 1: {
        system("CLS");
        logIn(db);
    }
          break;
    case 2: {
        system("CLS");
        signUp(db);
    }
          break;
    case 3: system("exit");
        break;
    default: {
        system("CLS");
        cout << "Nie ma takiej opcji, sprobuj ponownie.. \n";
        menu(db);
    }
    }
}

int main() {
    sqlite3* db;
    char* err_msg = nullptr;
    int rc = sqlite3_open("test.db", &db);

    if (rc) {
        cout << "Blad podczas polaczenia z baza danych: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return 1;
    }
    string sql_query = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, password TEXT NOT NULL)";
    rc = sqlite3_exec(db, sql_query.c_str(), nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK) {
        cout << "Tworzenie tabeli: " << err_msg << endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    /* DODAWANIE
    sql_query = "INSERT INTO users (name, age) VALUES ('John', 30)";
    rc = sqlite3_exec(db, sql_query.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "Error inserting record: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    */
    menu(db);
    /*
    sql_query = "SELECT * FROM users";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql_query.c_str(), -1, &stmt, nullptr);
    cout << rc;
    if (rc != SQLITE_OK) {
        cout << "Error preparing SELECT statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return 1;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* passwd = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)); // chagne type
        cout << "ID: " << id << ", LOGIN: " << name << ", PASSWORD: " << passwd << endl;
    }
    sqlite3_finalize(stmt);
    */
    sqlite3_close(db);

    return 0;
}