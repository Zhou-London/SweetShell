#include "swsh_ui.hpp"

int swsh_ui::getch(){
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void swsh_ui::displayText(){
    for(std::string text: static_texts){
        std::cout 
        << termcolor :: bold
        << text
        << termcolor :: reset
        << std::endl;
    }
}

void swsh_ui::displayMenu(const std::vector<std::unique_ptr<Button>>& options, int selectedIndex){
    for (int i = 0; i < options.size(); i++){
        if ( i == selectedIndex){
            options[i]->selected_print();
        } else {
            options[i]->unselected_print();
        }
    }
}

Signal swsh_ui::run(){
    
    int currentSelection = 0;
    int inputChar;
    bool running = true;

    while(running){

        system(CLEAR_SCREEN);

        displayText();
        displayMenu(menuOptions, currentSelection);
        inputChar = GET_CHAR();

        if(inputChar == SPECIAL_KEY_PREFIX){
            int nextChar1 = GET_CHAR();
            if(nextChar1 == ARROW_KEY_INDICATOR){
                int nextChar2 = GET_CHAR();
                if(nextChar2 == KEY_UP){
                    currentSelection = (currentSelection - 1 + menuOptions.size()) % menuOptions.size();
                }else if (nextChar2 == KEY_DOWN){
                    currentSelection = (currentSelection + 1) % menuOptions.size();
                }
            } else {
                ungetc(nextChar1, stdin);
            }
        }else if(inputChar == KEY_ENTER || inputChar == 13){

            system(CLEAR_SCREEN);
            return menuOptions[currentSelection]->onClick();

        }
    }

    system(CLEAR_SCREEN);
    Signal s;
    s.code = 0;
    return s;
}

