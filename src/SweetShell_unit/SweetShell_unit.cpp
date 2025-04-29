#include "SweetShell_unit.hpp"

#include <vector>
#include <ctime> // For std::time
#include <sstream> // For std::stringstream
#include <unistd.h> // For getpid()
#include <sys/wait.h> // For system
#include <iostream>

#include "TerminalInstance/TerminalInstance.hpp"

std::string SweetShell_unit::generate_window_id(){
    window_counter++;
    std::time_t now = std::time(nullptr);
    pid_t pid = getpid();

    //
    // A window_id is composed of
    // 1. Current count
    // 2. Current Timestamp
    // 3. PID (Identity of a process in the computer system)
    //
    // E.g "SweetShell-1-1698765432-12345"
    //
    std::stringstream ss;
    ss << "SweetShell-" << window_counter << "-" << now << "-" << pid;
    return ss.str();
};

pid_t SweetShell_unit::process_script(std::string command){

    // "command" is to be acted in a new terminal page
    // "window_id" is to be used to track this new terminal page
    //
    std::string window_id = generate_window_id();
    std::stringstream script;
    script << "osascript -e 'tell application \"Terminal\"' "
               << "-e 'set newWindow to do script \"" << command << "\"' "
               << "-e 'set custom title of newWindow to \"" << window_id << "\"' "
               << "-e 'activate' "
               << "-e 'end tell'";

    //
    // * Core Logic
    // 1. Use fork() to create a new process.
    // 2. The following code is both running on the new process
    //    and the old process.
    // 3. Let's call them "Child" and "Parent" process.
    // 4. In "Child", pid = 0 by definition, run the script
    // 5. In "Parent", pid is "Child"'s pid, track it using a vector.
    // Note that the code is running on two process, which means, twice
    // But with different pid.
    //
    pid_t pid = fork();
    if (pid == 0){
        int result = system(script.str().c_str());
        if (result != 0) {
            std::cerr << "Failed to process the command" << std::endl;
            exit(1);
        }
        exit(0);
    } else if (pid > 0){
        terminals.emplace_back(pid, window_id);
        return pid;
    } else {
        std::cerr << "Failed to fork. Check your system health." << std::endl;
    }
    return -1;
}

void SweetShell_unit::end_terminal(pid_t pid){
    if(pid <= 0) return;

    auto it = std::find_if(terminals.begin(), terminals.end(), 
    [pid](const TerminalInstance& t){return t.pid == pid;});

    if(it == terminals.end()) return;

    int status;
    waitpid(pid, &status, 0);

    std::stringstream close_script;
    close_script << "osascript -e 'tell application \"Terminal\"' "
                    << "-e 'close (every window whose custom title is \"" << it->window_id << "\")' "
                    << "-e 'if (count of windows) = 0 then quit' "
                    << "-e 'end tell'";
    int result = system(close_script.str().c_str());
    if (result != 0) {
        std::cerr << "Warning: Failed to close Terminal window for PID " << pid << std::endl;
    }
    terminals.erase(it);
}

pid_t SweetShell_unit::execute(std::string command) { return process_script(command); }

void SweetShell_unit::close(pid_t pid){ end_terminal(pid); }

void SweetShell_unit::close_all() {
    while (!terminals.empty()) { end_terminal(terminals.back().pid); }
}

std::vector<pid_t> SweetShell_unit::get_pids() const {
    std::vector<pid_t> pids;
    for (const auto& t : terminals) { pids.push_back(t.pid); }
    return pids;
}

bool SweetShell_unit::has_running_terminals() const {
    return !terminals.empty();
}

SweetShell_unit::~SweetShell_unit(){ close_all(); }