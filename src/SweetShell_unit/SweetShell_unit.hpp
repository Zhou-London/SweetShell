#pragma once

#include <vector>
#include "TerminalInstance/TerminalInstance.hpp"

class swsh_unit {
private:
    std::vector<TerminalInstance> terminals;
    int window_counter = 0;

    std::string generate_window_id();
    pid_t process_script(std::string command);
    void end_terminal(pid_t pid);

public:
    swsh_unit() {};
    ~swsh_unit();

    pid_t execute(std::string command);

    void close(pid_t pid);

    void close_all();

    std::vector<pid_t> get_pids() const;

    bool has_running_terminals() const;
};