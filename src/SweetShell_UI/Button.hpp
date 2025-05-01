#pragma once

#include <string>
#include <termcolor.hpp>
#include "Signal/Signal.hpp"
#include "SweetShell_unit/SweetShell_unit.hpp"

// * Abstract Button

class Button{
protected:
    std::string buttonLabel;

public:

    Button(const std::string& lbl): buttonLabel(lbl){}
    virtual ~Button() = default;

    virtual void unselected_print() const;
    virtual void selected_print() const;
    virtual Signal onClick() = 0;

    std::string getLabel() const;
};

// * Test Button

class TestButton : public Button{
private:
    int testData;
public:
    TestButton(const std::string& lbl, int data): Button(lbl) ,testData(data){}
    Signal onClick() override;
};

// * Quit Button

class QuitButton : public Button{
public:
    QuitButton(const std::string& lbl): Button(lbl){}
    Signal onClick() override;
};

// * Projects Button

class ProjsButton : public Button{
public:
    ProjsButton(const std::string& lbl): Button(lbl){}
    Signal onClick() override;
};

// * Projects-Info Button

class ProjsInfoButton : public Button{
private:
    Project_data data;
public:
    ProjsInfoButton(const std::string& lbl, 
                    const Project_data& project_data): 
                    Button(lbl),
                    data(project_data)
                    {}
    Signal onClick() override;
};

// * Run Preset Button
class RunPresetButton : public Button{
private:
    swsh_unit su;
    int click_count;
public:
    RunPresetButton(const std::string& lbl,
                    const Preset* ptr):
                    Button(lbl),
                    su(ptr)
                    {}
    ~RunPresetButton(){
        su.close_all();
    };
    Signal onClick() override;
};

// * Exit Project Info Button
class ExitInfoButton : public Button{
public:
    ExitInfoButton(const std::string& lbl):
                    Button(lbl)
                    {}
    Signal onClick() override;
};

// * Back Button
class BackButton : public Button{
private:
    int signal_code;
public:
    BackButton(const std::string& lbl,
               int s):
                Button(lbl),
                signal_code(s)
                {}
    Signal onClick() override;
};
