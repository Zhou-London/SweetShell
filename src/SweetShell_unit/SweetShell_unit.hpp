#pragma once

#include <vector>
#include "TerminalInstance/TerminalInstance.hpp"
#include "Preset/Preset.hpp"

class swsh_unit {
private:
    std::vector<TerminalInstance> terminals;
    const Preset* preset;

    int window_counter = 0;

    std::string generate_window_id();
    pid_t process_script(std::string command);
    void end_terminal(pid_t pid);

public:
    swsh_unit() {};
    swsh_unit(const Preset* p): preset(p) {};
    ~swsh_unit();

    pid_t execute(Command command);
    void set_preset(const Preset* preset);
    std::vector<pid_t> execute_preset();
    void close_all();

    bool has_running_terminals() const;
};