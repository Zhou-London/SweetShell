#include <iostream>
#include <unordered_map>
#include <termcolor.hpp>

#include "Util/safe_access.hpp"
#include "TerminalInstance/TerminalInstance.hpp"
#include "SweetShell_unit/SweetShell_unit.hpp"
#include "SweetShell_database/SweetShell_database.hpp"
#include "Preset/Preset.hpp"
#include "Project_data/Project_data.hpp"


using namespace std;

int main(void){
    swsh_database sd;
    std::vector<Project_data> projects = sd.iterate_projects();
    std::optional<Preset> target = sd.loadPreset(projects[0]);
    if(target.has_value()){
        Preset& loaded_preset = target.value();
        std::cout << termcolor::bold << loaded_preset.project_name << std::endl;
        for(Command cmd: loaded_preset.commands){
            std::cout << termcolor::bold << termcolor::color<170>
            << "Command Name: " 
            << termcolor::reset
            << cmd.name << std::endl
            << termcolor::bold << termcolor::color<170>
            << "Command: "
            <<termcolor::reset
            <<cmd.command << std::endl;
        }
        return 0;
    } else {
        std::cerr << "Not found" << std::endl;
    }
    return 0;
}