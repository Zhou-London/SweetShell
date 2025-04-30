#pragma once

#include <string>
#include <vector>
#include <memory>

class Preset;

class Command{
public:
    std::string command;
    std::string name;
    std::string description;
    Command(const std::string& command, const std::string& name): 
        command(command),
        name(name),
        description("")
    {}

    Command(const Command&) = default;
    Command& operator=(const Command&) = default;
    Command(Command&&) = default;
    Command& operator=(Command&&) = default;
    ~Command() = default;
};

class Preset{
public:
    std::vector<Command> commands;
    std::string project_name;
    Preset(const std::string& name): project_name(name){};
    ~Preset() = default;

    Preset(const Preset&) = default;
    Preset& operator=(const Preset&) = default;
    Preset(Preset&&) = default;
    Preset& operator=(Preset&&) = default;

    void add(const Command& c);

    Command remove(int index);
};
