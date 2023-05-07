#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>
#include <conio.h>

using namespace std;

class User {
public:
    int id = -1;
    string login = "";
};
User user;

void menu(sqlite3* db);
void loggedUserMenu(sqlite3* db);

string hasher(string str) {
    string hashed_str;
    for (char c : str) {
        hashed_str += to_string((int)c - 48);
    }
    return hashed_str;
}
string dehash(string hashed_str) {
    string str;
    for (int i = 0; i < hashed_str.length(); i += 2) {
        string code_str = hashed_str.substr(i, 2);
        int code = stoi(code_str) + 48;
        char c = (char)code;
        cout << code << " = " << c << endl;
        str += c;
    }
    return str;
}

void showSites(sqlite3* db) {
    system("CLS");
    cout << "Twoje witryny: \n";
    string sql_query = "SELECT * FROM sites WHERE id_user = " + to_string(user.id);
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql_query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cout << "Error preparing SELECT statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        cout << "WITRYNA: " << name << ", LOGIN: " << login << ", PASSWORD: " << dehash(password) << endl;
    }
    sqlite3_finalize(stmt);
    cout << "\nNacisnij klawisz aby wrocic.." << endl;
    _getch();
    system("CLS");
    loggedUserMenu(db);
}

void addSite(sqlite3* db) {
    string name, password, login, option;
    char* err_msg;
    cout << "Wprowadz nazwe witryny: ";
    cin >> name;
    cout << "Wprowadz login do witryny: ";
    cin >> login;
    cout << "Wprowadz haslo do witryny: ";
    cin >> password;
    string hashedPassword = hasher(password);
    string sql_query = "INSERT INTO sites (name, login, password, id_user) VALUES ('" + name + "', '" + login + "', '" + hashedPassword + "', " + to_string(user.id) + ")";
    int rc = sqlite3_exec(db, sql_query.c_str(), nullptr, nullptr, &err_msg);
    cout << rc;
    if (rc != SQLITE_OK) {
        cout << "Blad podczas dodawania: " << err_msg << endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    system("CLS");
    cout << "\nPomyslnie dodano witryne! Dodac jeszcze? (tak/nie)";
    cin >> option;
    if (option == "tak") {
        system("CLS");
        addSite(db);
    }
    else {
        system("CLS");
        loggedUserMenu(db);
    }
}

void logOut(sqlite3* db) {
    system("CLS");
    user.id = -1;
    user.login = "";
    cout << "Wylogowano \n";
    menu(db);
}
void loggedUserMenu(sqlite3* db) {
    int option;
    cout << "Witaj " + user.login + ", co chcesz zrobic? \n";
    cout << "1. Wyswietl witryny \n";
    cout << "2. Dodaj witryne \n";
    cout << "3. Wyloguj \n \n";

    cout << "Wybierz opcje: ";
    cin >> option;

    switch (option) {
    case 1: { // Wyswietl witryny
        system("CLS");
        showSites(db);
    }
          break;
    case 2: { // Dodaj witryne
        system("CLS");
        addSite(db);
    }
          break;
    case 3: logOut(db); // Wyloguj
        break;
    default: {
        system("CLS");
        cout << "Nie ma takiej opcji, sprobuj ponownie.. \n";
        menu(db);
    }
    }
};
void logIn(sqlite3* db) {
    if (user.id == -1) { // sprawdzanie czy 
        string login, pswd;
        sqlite3_stmt* stmt;
        cout << "Login: ";
        cin >> login;
        cout << "Haslo: ";
        cin >> pswd;

        string sql_query = "SELECT * FROM users WHERE (name = ? AND password = ?)";
        sqlite3_prepare_v2(db, sql_query.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, hasher(pswd).c_str(), -1, SQLITE_TRANSIENT);

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
            system("CLS");
            cout << "Bledne dane, sprobuj jeszcze raz: \n";
            logIn(db);
        }
    }
    else {
        system("CLS");
        cout << "Jestes juz zalogowany!.. \n";
        loggedUserMenu(db);
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

    string regex = "!@#$%^&*()_+=-;'.,/";
    size_t regexPswd;
    size_t regexLogin;
    for (char c : login) {
        regexPswd = regex.find(c);
        if (regexPswd != string::npos) {
            cout << "Niepoprawne znaki.. \n";
            menu(db);
        }
    }
    for (char c : pswd) {
        regexLogin = regex.find(c);
        if (regexLogin != string::npos) {
            cout << "Niepoprawne znaki.. \n";
            menu(db);
        }
    }
    if (login == "" || pswd == "" || pswd.length() > 50 || login.length() > 50) {
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
        sql_query = "INSERT INTO users (name, password) VALUES ('" + login + "', '" + hasher(pswd) + "')";
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
    // WYSWIETLANIE
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
    case 3: {
        sqlite3_close(db);
        system("exit");
    }
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

    sql_query = "CREATE TABLE IF NOT EXISTS sites (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, login TEXT NOT NULL, password TEXT NOT NULL, id_user INTEGER)";
    rc = sqlite3_exec(db, sql_query.c_str(), nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK) {
        cout << "Tworzenie tabeli: " << err_msg << endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    menu(db);
    sqlite3_close(db);

    return 0;
}