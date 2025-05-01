#pragma once

#include <string>
#include "Preset/Preset.hpp"
#include "Project_data/Project_data.hpp"


class Signal{
public:
    int code;
    std::string message;
    Preset preset;
    Project_data project_data;

    Signal(){}
};