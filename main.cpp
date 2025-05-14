#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

#include "encryption.h"


// Base class for User
class User {
protected:
    string username;
    string password;
public:
    User() {}
    virtual void setCredentials(const string& u, const string& p) {
        username = u;
        password = p;
    }
    string getUsername() { return username; }
    virtual bool authenticate(const string& u, const string& p) {
        return (username == u && password == p);
    }
};

// Derived Admin class (for example of inheritance & polymorphism)
class Admin : public User {
public:
    void setCredentials(const string& u, const string& p) override {
        username = u;
        password = p;
    }
};

// Password Manager class
class PasswordManager {
private:
    string userFile;
    string passwordFile;
    string currentUser;

public:
    PasswordManager(const string& uf, const string& pf) {
        userFile = uf;
        passwordFile = pf;
    }

    // Check if user exists, else create new
 bool createAccount() {
    string u, p;
    cout << "\n--- Create Master Account ---\n";
    cout << "Enter master username: ";
    getline(cin, u);
    cout << "Enter master password: ";
    getline(cin, p);

    ofstream fout(userFile);
    fout << xorEncryptDecrypt(u) << endl;
    fout << xorEncryptDecrypt(p) << endl;
    fout.close();
    cout << "Account created successfully!\n\n";
    return true;
}


    // Login existing user
    bool login() {
        string fileU, fileP, u, p;
        ifstream fin(userFile);
        if (!fin) {
            cout << "No account found. Please create one first.\n";
            return false;
        }
        getline(fin, fileU);
        getline(fin, fileP);
        fin.close();

        cout << "\n--- Login ---\n";
        cout << "Enter username: ";
        getline(cin, u);
        cout << "Enter password: ";
        getline(cin, p);

        if (xorEncryptDecrypt(fileU) == u && xorEncryptDecrypt(fileP) == p) {
            cout << "Login successful!\n\n";
            currentUser = u;
            return true;
        } else {
            cout << "Invalid credentials!\n\n";
            return false;
        }
    }

    // Check if password is weak
    bool isWeakPassword(const string& pass) {
        return pass.length() < 6;
    }

    // Generate random strong password suggestion
    string generateStrongPassword() {
        string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%";
        string suggestion;
        for (int i = 0; i < 10; ++i)
            suggestion += chars[rand() % chars.size()];
        return suggestion;
    }

    // Add a new password
    void addPassword() {
        string app, pass;
        cout << "\n--- Add New Password ---\n";
        cout << "Enter application/website name: ";
        getline(cin, app);
        cout << "Enter password: ";
        getline(cin, pass);

        while (isWeakPassword(pass)) {
    cout << "Password is weak. Would you like a suggested strong password? (y/n): ";
    char choice;
    cin >> choice;
    cin.ignore();
    if (choice == 'y' || choice == 'Y') {
        pass = generateStrongPassword();
        cout << "Suggested Password: " << pass << endl;
    } else {
        cout << "Enter new password: ";
        getline(cin, pass);
    }
}


        ofstream fout(passwordFile, ios::app);
        fout << xorEncryptDecrypt(currentUser) << " "
             << xorEncryptDecrypt(app) << " "
             << xorEncryptDecrypt(pass) << endl;
        fout.close();

        cout << "Password saved successfully!\n\n";
    }

    // Show passwords
    void showPasswords() {
        cout << "\n--- Stored Passwords ---\n";
        ifstream fin(passwordFile);
        if (!fin) {
            cout << "No passwords found.\n\n";
            return;
        }

        string u, app, pass;
        bool found = false;
        while (fin >> u >> app >> pass) {
            if (xorEncryptDecrypt(u) == currentUser) {
                cout << "App: " << xorEncryptDecrypt(app)
                     << " | Password: " << xorEncryptDecrypt(pass) << endl;
                found = true;
            }
        }
        fin.close();

        if (!found) cout << "No passwords saved yet.\n";
        cout << endl;
    }
};

// Main program
int main() {
    srand(time(0));
    PasswordManager manager("user.dat", "passwords.dat");

    int choice;
    bool loggedIn = false;
    Admin admin;

    // Create account if not exists
    ifstream fin("user.dat");
    if (!fin)
        manager.createAccount();
    fin.close();

    // Login loop
    while (!loggedIn) {
        loggedIn = manager.login();
    }

    // Main menu loop
    do {
        cout << "=============================\n";
        cout << "      PASSWORD MANAGER       \n";
        cout << "=============================\n";
        cout << "1. Add Password\n";
        cout << "2. Show Passwords\n";
        cout << "3. Exit\n";
        cout << "Choose option: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            manager.addPassword();
            break;
        case 2:
            manager.showPasswords();
            break;
        case 3:
            cout << "Exiting program. Goodbye!\n";
            break;
        default:
            cout << "Invalid option. Try again.\n\n";
        }
    } while (choice != 3);

    return 0;
}