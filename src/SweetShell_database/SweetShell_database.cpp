#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <sstream>
#include <termcolor.hpp>
#include <vector>

#include "SweetShell_database.hpp"
#include "Preset/Preset.hpp"

swsh_database::swsh_database() : db("swsh_database.db", 
                                SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
{
    //std::cout<<"Database connected."<<std::endl;
}

void swsh_database::init_helper(){

    SQLite::Transaction transaction(db);

    db.exec(R"(
        CREATE TABLE IF NOT EXISTS projects (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL
        );
    )");

    db.exec(R"(
        CREATE TABLE IF NOT EXISTS commands (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            command TEXT NOT NULL,
            name TEXT NOT NULL,
            description TEXT,
            project_id INTEGER,
            FOREIGN KEY (project_id) REFERENCES projects(id) ON DELETE SET NULL ON UPDATE CASCADE
        );
    )");

    db.exec("CREATE INDEX IF NOT EXISTS idx_command_project ON commands (project_id);");
    db.exec("CREATE INDEX IF NOT EXISTS idx_command_name ON commands (name);");

    transaction.commit();
}

void swsh_database::init(){
    try{
        init_helper();
        std::cout << "Database initlized." <<std::endl;
    }catch(const std::exception& e){
        std::cerr << "Error with Database Initlization: " 
        << e.what() << std::endl;
        exit(1);
    }
}

void swsh_database::check(){
    std::stringstream ss;
    ss << 
    "Commands Table: " << db.tableExists("commands") << std::endl << 
    "Projects Table: " << db.tableExists("projects") << std::endl <<
    "Check your SQLite3 and SQLiteCpp status if there is any problem."
    << std::endl;
    std::cout << termcolor::bold << ss.str();
}

bool swsh_database::save_preset(const Preset& preset){
    long long project_id = -1;
    SQLite::Transaction transaction(this->db);
    try {
        // * Insert to projects
        SQLite::Statement query_project(this->db, "SELECT id FROM projects WHERE name = ?");
        query_project.bind(1, preset.project_name);

        if (query_project.executeStep()) {
            // Already exist
            project_id = query_project.getColumn(0).getInt64();
            std::cout << "Project '" << preset.project_name << "' exists, ID: " << project_id << std::endl;

            // Delete the old data
            SQLite::Statement delete_commands(this->db, "DELETE FROM commands WHERE project_id = ?");
            delete_commands.bind(1, project_id);
            int deleted_rows = delete_commands.exec();
            std::cout << "Deleted " << deleted_rows << " old data." << std::endl;

        } else {
            // Doesn't exist
            SQLite::Statement insert_project(this->db, "INSERT INTO projects (name) VALUES (?)");
            insert_project.bind(1, preset.project_name);
            int rows_affected = insert_project.exec();
            if (rows_affected > 0) {
                // Get its ID
                project_id = this->db.getLastInsertRowid();
                std::cout << "Project '" << preset.project_name << "' created, ID: " << project_id << std::endl;
            } else {
                // Failed insert
                std::cerr << "Error: Failed to insert '" << preset.project_name << std::endl;
                return false;
            }
        }

        // Check project_id valid
        if (project_id <= 0) {
             std::cerr << "Error: project_id invalid" << std::endl;
             return false;
        }


        // * Insert to commands

        SQLite::Statement insert_command(this->db,
            "INSERT INTO commands (command, name, description, project_id) VALUES (?, ?, ?, ?)");

        int inserted_cmd_count = 0;

        // Iterate preset.commands
        for (const Command& cmd : preset.commands) {
            insert_command.bind(1, cmd.command);
            insert_command.bind(2, cmd.name);
            // Optional Description
            if (cmd.description.empty()) {
                insert_command.bind(3, SQLite::Null);
            } else {
                insert_command.bind(3, cmd.description);
            }
            insert_command.bind(4, project_id);


            int cmd_rows_affected = insert_command.exec();
            if (cmd_rows_affected <= 0) {
                std::cerr << "Failed to insert "<< cmd.name << std::endl;
            } else {
                inserted_cmd_count++;
            }
            // Reset the statement
            insert_command.reset();
        }

        // * Submit transaction
        transaction.commit();
        std::cout << termcolor::color<170> <<
        "Successfully inserted " << preset.project_name << std::endl;
        return true;

    } catch (const SQLite::Exception& e) {
        std::cerr << "Database Error: " << e.what() << std::endl;
        return false; 
    } catch (const std::exception& e) {
        std::cerr << "System Error: " << e.what() << std::endl;
        return false; 
    }

}

std::vector<Project_data> swsh_database::iterate_projects(){
    std::vector<Project_data> projects_list;
    try {
        SQLite::Statement query(this->db, "SELECT id, name FROM projects ORDER BY name ASC");
        // Iterate every row
        while (query.executeStep()) {
            long long id = query.getColumn("id").getInt64();
            std::string name = query.getColumn("name").getText();

            projects_list.push_back(Project_data(id, name));
        }
    } catch (const SQLite::Exception& e) {
        std::cerr << "Failed to search data: " << e.what() << std::endl;
        projects_list.clear();
    } catch (const std::exception& e) {
        std::cerr << "Unknown system error during searching: " << e.what() << std::endl;
        projects_list.clear();
    }

    return projects_list;

}

std::optional<Preset> swsh_database::loadPresetById(long long project_id) {
    try {
        SQLite::Statement query_project(this->db, "SELECT name FROM projects WHERE id = ?");
        query_project.bind(1, project_id);

        std::string project_name;
        if (query_project.executeStep()) {
            project_name = query_project.getColumn(0).getText();
        } else {
            // Doesn't exist
            std::cerr << "Error: Failed to find Project ID: " << project_id << std::endl;
            return std::nullopt;
        }

        Preset loadedPreset(project_name);

        // Search for its commands
        SQLite::Statement query_commands(this->db,
            "SELECT command, name, description FROM commands WHERE project_id = ? ORDER BY name ASC");
        query_commands.bind(1, project_id);

        // Add commands to the preset
        while (query_commands.executeStep()) {
            std::string cmd_str = query_commands.getColumn(0).getText();
            std::string name_str = query_commands.getColumn(1).getText();
            std::string desc_str = query_commands.getColumn(2).getText();

            loadedPreset.commands.emplace_back(cmd_str, name_str);
            loadedPreset.commands.back().description = desc_str;
        }

        return loadedPreset;

    } catch (const SQLite::Exception& e) {
        std::cerr << "Failed to load Project: " << project_id << e.what() << std::endl;
        return std::nullopt;
    } catch (const std::exception& e) {
        std::cerr << "System Error Failed to load: " << project_id << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<Preset> swsh_database::loadPreset(const Project_data& project){
    try{
        SQLite::Statement query_project_id(this->db, "SELECT id FROM projects WHERE name = ?");
        query_project_id.bind(1, project.name);

        if(query_project_id.executeStep()){
            long long id = query_project_id.getColumn(0).getInt64();
            return loadPresetById(id);
        } else {
            std::cerr << "Failed to find project: "<< project.name << std::endl;
            return std::nullopt;
        }
    } catch (SQLite::Exception& e){
        std::cerr << "Database error during searching: " << project.name << e.what() << std::endl;
        return std::nullopt;
    } catch (std::exception& e){
        std::cerr << "System error during searching: " << project.name << e.what() << std::endl;
        return std::nullopt;
    }
} 
