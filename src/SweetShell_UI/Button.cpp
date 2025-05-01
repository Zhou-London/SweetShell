#include "Button.hpp"

// * Abstract Button

void Button::unselected_print() const {
    std::cout 
    << termcolor::bold
    << buttonLabel << termcolor::reset << std::endl;
}

void Button::selected_print() const {
    std::cout 
    << termcolor::bold << termcolor::color<170> <<termcolor::underline
    << buttonLabel << termcolor::reset << std::endl;
}

std::string Button::getLabel() const {
    return buttonLabel;
}

// * Test Button
Signal TestButton::onClick(){
    std::cout << std::endl 
    << "This Button has been clicked!"
    << std::endl;
    Signal s;
    s.code = -1;
    return s;
}

// * Quit Button
Signal QuitButton::onClick(){
    Signal s;
    s.code = 1000;
    return s;
}

// * Projects Button
Signal ProjsButton::onClick(){
    Signal s;
    s.code = 1001;
    return s;
}

// * Projects Info Button
Signal ProjsInfoButton::onClick(){
    Signal s;
    s.project_data = data;
    s.code = 1002;
    return s;
}

// * Run Preset Button
Signal RunPresetButton::onClick(){
    Signal s;
    if(click_count > 0){
        click_count = 0;
        su.close_all();
    } else {
        click_count++;
        su.execute_preset();
    }
    s.code = -1000;
    return s;
}

// * Delete Preset Button
Signal DeletePresetButton::onClick(){
    Signal s;
    swsh_database sd;
    sd.deletePresetById(id);
    s.code = 1001;
    return s;
}

// * Exit Info Button
Signal ExitInfoButton::onClick(){
    Signal s;
    s.code = 10020;
    return s;
}

// * Back Button
Signal BackButton::onClick(){
    Signal s;
    s.code = signal_code;
    return s;
}