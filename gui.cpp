// Password Manager with GUI and Functionality
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "encryption.h"


struct Account {
    std::string site;
    std::string password;
};

std::unordered_map<std::string, std::string> users;
std::unordered_map<std::string, std::vector<Account>> userPasswords;
std::string currentUser = "";

void loadUsers() {
    std::ifstream file("users.dat");
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string user, pass;
        if (std::getline(iss, user, '|') && std::getline(iss, pass)) {
            users[xorEncryptDecrypt(user)] = xorEncryptDecrypt(pass);
        }
    }
    file.close();
}

void saveUsers() {
    std::ofstream file("users.dat");
    for (auto& pair : users) {
        file << xorEncryptDecrypt(pair.first) << "|" << xorEncryptDecrypt(pair.second) << "\n";
    }
    file.close();
}

void savePasswords() {
    std::ofstream file("passwords.dat");
    for (auto& userEntry : userPasswords) {
        for (auto& acc : userEntry.second) {
            file << xorEncryptDecrypt(userEntry.first) << "|"
                 << xorEncryptDecrypt(acc.site) << "|"
                 << xorEncryptDecrypt(acc.password) << "\n";
        }
    }
    file.close();
}

void loadPasswords() {
    std::ifstream file("passwords.dat");
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string user, site, pass;
        if (std::getline(iss, user, '|') && std::getline(iss, site, '|') && std::getline(iss, pass)) {
            userPasswords[xorEncryptDecrypt(user)].push_back({xorEncryptDecrypt(site), xorEncryptDecrypt(pass)});
        }
    }
    file.close();
}

int main() {
    loadUsers();
    loadPasswords();
    sf::RenderWindow window(sf::VideoMode(800, 600), "Password Manager");
    sf::Font font;
    font.loadFromFile("assets/arial.ttf");

    enum Screen { LOGIN, MENU, ADD_ACCOUNT, SHOW_PASSWORDS, CREATE_ACCOUNT, ERROR_MSG } screen = LOGIN;

    std::string input1 = "", input2 = "", errorMsg = "";
    std::string siteInput = "", sitePass = "";
    bool inputFocus1 = false, inputFocus2 = false;
    bool siteFocus = false, passFocus = false;

    auto drawInputBox = [&](sf::RenderWindow& window, sf::Font& font, std::string label, std::string& input, sf::Vector2f pos, bool isPassword, bool isFocused) {
        sf::Text text(label, font, 24);
        text.setPosition(pos.x, pos.y);
        window.draw(text);

        sf::RectangleShape box(sf::Vector2f(400, 40));
        box.setFillColor(isFocused ? sf::Color(70, 70, 70) : sf::Color(50, 50, 50));
        box.setPosition(pos.x + 150, pos.y);
        window.draw(box);

        std::string display = isPassword ? std::string(input.length(), '*') : input;
        sf::Text inputText(display, font, 20);
        inputText.setFillColor(sf::Color::White);
        inputText.setPosition(pos.x + 160, pos.y + 5);
        window.draw(inputText);

        return box.getGlobalBounds();
    };

    auto drawButton = [&](sf::RenderWindow& window, sf::Font& font, std::string label, sf::Vector2f pos) {
        sf::RectangleShape button(sf::Vector2f(200, 50));
        button.setFillColor(sf::Color(100, 100, 100));
        button.setPosition(pos);
        window.draw(button);

        sf::Text buttonText(label, font, 20);
        buttonText.setFillColor(sf::Color::White);
        buttonText.setPosition(pos.x + 20, pos.y + 10);
        window.draw(buttonText);

        return button.getGlobalBounds();
    };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                inputFocus1 = inputFocus2 = siteFocus = passFocus = false;

                if (screen == LOGIN || screen == CREATE_ACCOUNT) {
                    if (drawInputBox(window, font, "Username:", input1, {100, 150}, false, true).contains(mousePos)) inputFocus1 = true;
                    if (drawInputBox(window, font, "Password:", input2, {100, 220}, true, true).contains(mousePos)) inputFocus2 = true;

                    if (drawButton(window, font, screen == LOGIN ? "Login" : "Register", {100, 300}).contains(mousePos)) {
                        if (screen == LOGIN) {
                            if (users.count(input1) && users[input1] == input2) {
                                currentUser = input1;
                                screen = MENU;
                                input1 = input2 = "";
                            } else {
                                errorMsg = "Incorrect password.";
                                screen = ERROR_MSG;
                            }
                        } else {
                            users[input1] = input2;
                            saveUsers();
                            screen = LOGIN;
                        }
                    }

                    if (drawButton(window, font, screen == LOGIN ? "Create Account" : "Back", {350, 300}).contains(mousePos)) {
                        screen = (screen == LOGIN ? CREATE_ACCOUNT : LOGIN);
                        input1 = input2 = "";
                    }
                } else if (screen == MENU) {
                    if (drawButton(window, font, "1. Add Account", {100, 150}).contains(mousePos)) screen = ADD_ACCOUNT;
                    if (drawButton(window, font, "2. Show Passwords", {100, 220}).contains(mousePos)) screen = SHOW_PASSWORDS;
                    if (drawButton(window, font, "3. Exit", {100, 290}).contains(mousePos)) window.close();
                } else if (screen == ADD_ACCOUNT) {
                    if (drawInputBox(window, font, "App/Website:", siteInput, {100, 150}, false, true).contains(mousePos)) siteFocus = true;
                    if (drawInputBox(window, font, "Password:", sitePass, {100, 220}, true, true).contains(mousePos)) passFocus = true;
                    if (drawButton(window, font, "Save Password", {100, 300}).contains(mousePos)) {
                        userPasswords[currentUser].push_back({siteInput, sitePass});
                        savePasswords();
                        siteInput = sitePass = "";
                        screen = MENU;
                    }
                } else if (screen == SHOW_PASSWORDS) {
                    if (drawButton(window, font, "Back to Menu", {100, 500}).contains(mousePos)) screen = MENU;
                } else if (screen == ERROR_MSG) {
                    if (drawButton(window, font, "OK", {300, 300}).contains(mousePos)) screen = LOGIN;
                }
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    char typed = static_cast<char>(event.text.unicode);
                    if (typed == 8) {
                        if (inputFocus1 && !input1.empty()) input1.pop_back();
                        if (inputFocus2 && !input2.empty()) input2.pop_back();
                        if (siteFocus && !siteInput.empty()) siteInput.pop_back();
                        if (passFocus && !sitePass.empty()) sitePass.pop_back();
                    } else if (typed >= 32 && typed <= 126) {
                        if (inputFocus1) input1 += typed;
                        if (inputFocus2) input2 += typed;
                        if (siteFocus) siteInput += typed;
                        if (passFocus) sitePass += typed;
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed && screen == MENU) {
                if (event.key.code == sf::Keyboard::Num1) screen = ADD_ACCOUNT;
                if (event.key.code == sf::Keyboard::Num2) screen = SHOW_PASSWORDS;
                if (event.key.code == sf::Keyboard::Num3) window.close();
            }
        }

        window.clear(sf::Color(30, 30, 30));

        if (screen == LOGIN || screen == CREATE_ACCOUNT) {
            drawInputBox(window, font, "Username:", input1, {100, 150}, false, inputFocus1);
            drawInputBox(window, font, "Password:", input2, {100, 220}, true, inputFocus2);
            drawButton(window, font, screen == LOGIN ? "Login" : "Register", {100, 300});
            drawButton(window, font, screen == LOGIN ? "Create Account" : "Back", {350, 300});
        } else if (screen == MENU) {
            drawButton(window, font, "1. Add Account", {100, 150});
            drawButton(window, font, "2. Show Passwords", {100, 220});
            drawButton(window, font, "3. Exit", {100, 290});
        } else if (screen == ADD_ACCOUNT) {
            drawInputBox(window, font, "App/Website:", siteInput, {100, 150}, false, siteFocus);
            drawInputBox(window, font, "Password:", sitePass, {100, 220}, true, passFocus);
            drawButton(window, font, "Save Password", {100, 300});
        } else if (screen == SHOW_PASSWORDS) {
            int y = 150;
            for (auto& acc : userPasswords[currentUser]) {
                sf::Text text(acc.site + " : " + acc.password, font, 20);
                text.setFillColor(sf::Color::White);
                text.setPosition(100, y);
                window.draw(text);
                y += 30;
            }
            drawButton(window, font, "Back to Menu", {100, 500});
        } else if (screen == ERROR_MSG) {
            sf::Text text(errorMsg, font, 24);
            text.setFillColor(sf::Color::Red);
            text.setPosition(100, 150);
            window.draw(text);
            drawButton(window, font, "OK", {300, 300});
        }

        window.display();
    }

    return 0;
}