#include <iostream>
#include <unordered_map>
#include <termcolor.hpp>
#include <ios>
#include <limits>

#include "Util/safe_access.hpp"
#include "TerminalInstance/TerminalInstance.hpp"
#include "SweetShell_unit/SweetShell_unit.hpp"
#include "SweetShell_database/SweetShell_database.hpp"
#include "Preset/Preset.hpp"
#include "Project_data/Project_data.hpp"
#include "SweetShell_UI/swsh_ui.hpp"
#include "SweetShell_UI/Button.hpp"


using namespace std;

int main(){


    // ? Signal Code Table
    // ? 0 -> Root Scene (Static Scene)
    // ? 1000 -> Quit
    // ? 1001 -> Projects Scene (Dynamic Scene)
    // ? 1002 -> Projects Infomation Scene (Dynamic Scene)
    // ? 10020 -> Projects Information Scene Exit
    // ? 1003 -> Create New Project
    Signal signal;
    signal.code = 0;


    // * Root Scene
    swsh_ui root_scene;
    std::vector<std::string> swsh_rose =  {                                                                                                                     
    "                          .::::::                                       ",
    "                        :::::::..::::::::.                              ",
    "                   .:::::...        :::::::.                            ",
    "                 ::::::       .::;::  ::::::::::.                       ",
    "                ::::::   ::;;.     :;. ::  .::::::                      ",
    "              .::::::.  :;;: .:;;;; :: :;;.  :;;::                      ",
    "           ::::::::;:  :;:  :;.   .;  ;;;:  .;;;::.                    ",
    "           ::::::: .;;:  :;;.  :;:.  :;;;;.  .;;;;;;:.                  ",
    "           ::::::  .;;;;;: .;;   :;;;;;;:   .;;;;;::;::                 ",
    "           ::::;:   ;;;;;;;;;;;;;;;;;:.   .;;;;;;;: :;;                 ",
    "          ::::;;: .;.             :   :;;;:.:;;;;;. .;;;                ",
    "         :::::;;; ::  ;; .;.       :;;:......;;;;.  .;;;                ",
    "         :::::;;;:::            .;;:.........;;;.   :;;;                ",
    "        .::::..;;;;;;;;;;;;;;;;;;;;;;;;;;;;..;;    :;;;:                ",
    "         ::::. .:;;;                      .;.;:   :;;;:                 ",
    "          :;:.  .;;;    :;                 :;;  .;;;;.                  ",
    "           :;;   .;;      :;               .;: :;;;;:                   ",
    "            ;;:   :;       ;:              ;; :;;;;;;;:                 ",
    "             :;:  :;     ;:               :;.;;;;;;;;                   ",
    "              :;;: ::                    ;;;;:..:;;:                    ",
    "                ;;;;;;.               :;;;;:...:;;:                     ",
    "                   :;;;;;;.        :;;;;:.....;;;.                      ",
    "                        .;;;;  .;;;;:..  ..:;;;:                        ",
    "                     :;;;;;;;;;;;;;;;;;;;;;;;.                          ",
    "                    ....:::.:;;;;;.                                     ",
    "                            ;;:.                                        "
    }; 
    

    root_scene.static_texts = swsh_rose;
    root_scene.static_texts.push_back("                ----Welcome to SweetShell!----");
    root_scene.static_texts.push_back("v0.3");
    root_scene.static_texts.push_back("");
    root_scene.static_texts.push_back("");
    root_scene.menuOptions.push_back(std::make_unique<ProjsButton>("Projects"));
    root_scene.menuOptions.push_back(std::make_unique<BackButton>("Create New Project", 1003));
    root_scene.menuOptions.push_back(std::make_unique<QuitButton>("Quit"));

    swsh_database sd;

    while(true){
        // Quit
        if(signal.code == 1000) break;
        // Root
        if(signal.code == 0){
            signal = root_scene.run();
        }
        // Projects
        if(signal.code == 1001){
            // Init
            swsh_ui projects_scene;
            projects_scene.static_texts = {"----Your Projects List----"};

            std::vector<Project_data> projects = sd.iterate_projects();
            if(projects.empty()){
                projects_scene.static_texts.push_back("There are no project yet.");

            } else{
                for(Project_data proj: projects){
                    projects_scene.menuOptions.push_back(std::make_unique<ProjsInfoButton>(
                        proj.name, 
                        proj
                    ));
                }
            }

            projects_scene.menuOptions.push_back(std::make_unique<BackButton>(
                "<< Back",
                0
            ));
            signal = projects_scene.run();


            // Projects-Information
            if(signal.code == 1002){
                // Init
                swsh_ui project_info_scene;

                std::optional<Preset> preset = sd.loadPresetById(signal.project_data.id);
                if(preset.has_value()){
                    project_info_scene.static_texts.push_back("----Detail----");
                    project_info_scene.static_texts.push_back("");
                    project_info_scene.static_texts.push_back("Project: " + preset.value().project_name);
                    project_info_scene.static_texts.push_back("---");
                    
                    for(Command cmd: preset.value().commands){
                        project_info_scene.static_texts.push_back("Name: " + cmd.name);
                        project_info_scene.static_texts.push_back("Command: " + cmd.command);
                        if(!cmd.description.empty()){
                            project_info_scene.static_texts.push_back("Description: " + cmd.description);
                        }
                        project_info_scene.static_texts.push_back("");
                    }
                    
                    project_info_scene.menuOptions.push_back(std::make_unique<RunPresetButton>(
                        "Run This Project",
                        &preset.value()
                    ));

                    project_info_scene.menuOptions.push_back(std::make_unique<DeletePresetButton>(
                        "Delete This Project",
                        signal.project_data.id
                    ));

                    project_info_scene.menuOptions.push_back(std::make_unique<ExitInfoButton>(
                        "<< Back"
                    ));

                    while(signal.code != 10020 && signal.code != 1001){
                        signal = project_info_scene.run();
                    }

                    signal.code = 1001;
                }
            }
                

        }
        // Create New Project
        if(signal.code == 1003){

            std::cout << termcolor::bold
            << "----Create New Project----"
            << termcolor::reset
            << std::endl
            << std::endl;

            Preset new_preset;
            std::string proj_name;
            
            std::cout << termcolor::bold << termcolor::color<170>
            << "What's the name of your project?"
            << std::endl
            << "> "
            << termcolor::reset;
            
            std::getline(std::cin, proj_name);
            new_preset.project_name = proj_name;

            int count = 1;

            while(true){

                std::cout << termcolor::bold << termcolor::color<170>
                << "Enter your " << count << " command."
                << " (Enter q to quit)"
                << std::endl
                << "> "
                << termcolor::reset;

                std::string cmd;
                std::getline(std::cin, cmd);

                if(cmd == "q" || cmd == "Q"){
                    if(count > 1){
                        sd.save_preset(new_preset);
                    }
                    signal.code = 0;
                    break;

                }

                count++;

                std::cout << termcolor::bold << termcolor::color<170>
                << "Enter its name."
                << std::endl
                << "> "
                << termcolor::reset;

                std::string cmd_name;
                std::getline(std::cin, cmd_name);

                std::cout << termcolor::bold << termcolor::color<170>
                << "Enter its description."
                << " (Optional)"
                << std::endl
                << "> "
                << termcolor::reset;

                std::string cmd_description;
                std::getline(std::cin, cmd_description);

                Command new_cmd(cmd, cmd_name);
                cmd_description.empty() ? 
                new_cmd.description = "" : new_cmd.description = cmd_description;

                new_preset.add(new_cmd);

            }
        }
    }
    system(CLEAR_SCREEN);

    return 0;


}