#include "Preset.hpp"
#include <memory>

void Preset::add(const Command& c){
    commands.push_back(c);
}

Command Preset::remove(int index){
    if (index < 0 || static_cast<size_t>(index) >= commands.size()) {
        throw std::out_of_range("Index out of range for Preset::remove");
    }
    auto it = commands.begin() + index;
    Command removed = *it;
    commands.erase(it);
    return removed;
}