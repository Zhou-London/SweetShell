#include <iostream>
#include <unordered_map>

#include "Util/safe_access.hpp"
#include "TerminalInstance/TerminalInstance.hpp"
#include "SweetShell_unit/SweetShell_unit.hpp"


using namespace std;

int main(void){
    SweetShell_unit swsh;
    swsh.execute("whoami");
    while(true){
        char input;
        cin >> input;
        if(input == 'e') break;
    }
    return 0;
}