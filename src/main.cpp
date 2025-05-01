#include <iostream>
#include <unordered_map>
#include <termcolor.hpp>

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
    Signal signal;


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
    root_scene.menuOptions.push_back(std::make_unique<TestButton>("Create New Project", 123));
    root_scene.menuOptions.push_back(std::make_unique<QuitButton>("Quit"));

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


            swsh_database sd;
            std::vector<Project_data> projects = sd.iterate_projects();
            for(Project_data proj: projects){
                projects_scene.menuOptions.push_back(std::make_unique<ProjsInfoButton>(
                    proj.name, 
                    proj
                ));
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

                    project_info_scene.menuOptions.push_back(std::make_unique<ExitInfoButton>(
                        "<< Back"
                    ));

                    while(signal.code != 10020){
                        signal = project_info_scene.run();
                    }

                    signal.code = 1001;
                }
                

            }
        }
    }

    return 0;


}