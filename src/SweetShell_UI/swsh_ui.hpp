#pragma once

#include <iostream>
#include <termios.h> // For tcgetattr()
#include <unistd.h> // For STDIN_
#include <cstdlib>
#include <vector>
#include <termcolor.hpp>
#include <memory>

#include "SweetShell_UI/Button.hpp"

#define CLEAR_SCREEN "clear"
#define GET_CHAR getch
#define KEY_UP 'A'
#define KEY_DOWN 'B'
#define KEY_ENTER 10
#define SPECIAL_KEY_PREFIX 27
#define ARROW_KEY_INDICATOR 91

class swsh_ui{
private:
    int getch();
    
    void displayMenu(const std::vector<std::unique_ptr<Button>>& options, int selectedIndex);
    void displayText();


    
public:
    swsh_ui(){}
    ~swsh_ui() = default;

    std::vector<std::string> static_texts;
    std::vector<std::unique_ptr<Button>> menuOptions;

    Signal run();
    
};