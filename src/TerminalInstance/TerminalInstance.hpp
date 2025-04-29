#pragma once

#include <string>

class TerminalInstance {
public:
    pid_t pid;
    std::string window_id;
    TerminalInstance(pid_t p, const std::string& id) : pid(p),window_id(id){}
};