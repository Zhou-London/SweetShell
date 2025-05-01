#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <Preset/Preset.hpp>
#include <string>
#include <vector>

#include "Project_data/Project_data.hpp"

class swsh_database{
private:
    void init_helper();

public:
    swsh_database();
    SQLite::Database db;

    void init();
    void check();

    bool save_preset(const Preset& preset);
    std::vector<Project_data> iterate_projects();
    std::optional<Preset> loadPresetById(long long project_id);
    std::optional<Preset> loadPreset(const Project_data& project);
    void deletePresetById(long long project_id);

    
};