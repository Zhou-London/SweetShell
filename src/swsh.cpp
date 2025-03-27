#include <iostream>
#include <string>
#include <sstream>
#include <ctime>      // For timestamp
#include <unistd.h>   // For getpid() and fork()
#include <sys/wait.h> // For waitpid()
#include <vector>
#include <unordered_map>

template<typename Map>
auto& safe_access(Map& map, const std::string& key){
    auto it = map.find(key);
    if (it == map.end()){
        throw std::out_of_range("Failed to access the Map: " + key + " not found.");
    }
    return it->second;
}

class TerminalInstance {
public:
    pid_t pid;
    std::string window_id;  // Unique window identifier
    TerminalInstance(pid_t p, const std::string& id) : pid(p), window_id(id) {}
};

class SweetShell_unit {
private:
    std::vector<TerminalInstance> terminals;
    int window_counter = 0;  // Incremental counter for uniqueness within process

    // Generate a unique window identifier
    std::string generate_window_id() {
        window_counter++; // Increment the counter
        std::time_t now = std::time(nullptr); // Current timestamp
        pid_t pid = getpid(); // Process ID
        std::stringstream ss;
        ss << "SweetShell-" << window_counter << "-" << now << "-" << pid;
        return ss.str(); // e.g., "SweetShell-1-1698765432-12345"
    }

    pid_t process_script(std::string command) {
        std::string window_id = generate_window_id();
        std::stringstream script;
        script << "osascript -e 'tell application \"Terminal\"' "
               << "-e 'set newWindow to do script \"" << command << "\"' "
               << "-e 'set custom title of newWindow to \"" << window_id << "\"' "
               << "-e 'activate' "
               << "-e 'end tell'";

        pid_t pid = fork();
        if (pid == 0) {  // Child process
            int result = system(script.str().c_str());
            if (result != 0) {
                std::cerr << "Failed to process the command" << std::endl;
                exit(1);
            }
            exit(0);
        } else if (pid > 0) {  // Parent process
            terminals.emplace_back(pid, window_id);
            return pid;
        } else {
            throw std::runtime_error("Failed to fork. What's wrong with your system?");
        }
        return -1;
    }

    void end_terminal(pid_t pid) {
        if (pid <= 0) return;

        // Find the terminal instance
        auto it = std::find_if(terminals.begin(), terminals.end(),
                              [pid](const TerminalInstance& t) { return t.pid == pid; });
        if (it == terminals.end()) return;  // Not found

        // Wait for the process to complete
        int status;
        waitpid(pid, &status, 0);

        // Close the specific window using its custom title
        std::stringstream close_script;
        close_script << "osascript -e 'tell application \"Terminal\"' "
                     << "-e 'close (every window whose custom title is \"" << it->window_id << "\")' "
                     << "-e 'if (count of windows) = 0 then quit' "  // Quit Terminal if no windows remain
                     << "-e 'end tell'";
        
        int result = system(close_script.str().c_str());
        if (result != 0) {
            std::cerr << "Warning: Failed to close Terminal window for PID " << pid << std::endl;
        }

        // Remove from vector
        terminals.erase(it);
    }

public:
    SweetShell_unit() {}

    ~SweetShell_unit() {
        close_all();
    }

    pid_t execute(std::string command) {
        try {
            return process_script(command);
        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return -1;
        }
    }

    void close(pid_t pid) {
        end_terminal(pid);
    }

    void close_all() {
        while (!terminals.empty()) {
            end_terminal(terminals.back().pid);
        }
    }

    std::vector<pid_t> get_pids() const {
        std::vector<pid_t> pids;
        for (const auto& t : terminals) {
            pids.push_back(t.pid);
        }
        return pids;
    }

    bool has_running_terminals() const {
        return !terminals.empty();
    }
};

class SweetShell {
private:

    // Collection -> Commands
    std::unordered_map<std::string, std::vector<std::string>> command_table;
    // Collection -> SweetShell_unit
    std::unordered_map<std::string, SweetShell_unit> unit_table;

    void run_unit(std::string collection){

        //Get the Data
        try{
            const auto& commands = safe_access(command_table, collection);
            SweetShell_unit& shell_unit = safe_access(unit_table, collection);

            std::vector<pid_t> pids;
            for(std::string command:commands){
                pids.push_back(shell_unit.execute(command));
            }
            std::cout<<"Available PID: "<<std::endl;
            for(pid_t pid:pids){
                std::cout<<pid<<std::endl;
            }
        } catch (std::exception& e){
            std::cout << e.what() << std::endl;
        }
    }

    void close_unit(std::string collection){
        try{
            SweetShell_unit& shell_unit = safe_access(unit_table, collection);
            shell_unit.close_all();
        } catch (std::exception& e){
            std::cout << e.what() << std::endl;
        }
    }

public:
    SweetShell(): 
        command_table(),
        unit_table(){
            command_table["test"] = {"ls", "whoami"};
            unit_table["test"] = SweetShell_unit();
        }
    
    void test(){
        run_unit("tes");
        std::cin >> std::noskipws;
        std::cin.ignore();
        close_unit("tes");
    }
};


int main() {

    SweetShell swsh = SweetShell();
    swsh.test();
    
    return 0;
}