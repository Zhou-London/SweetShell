#pragma once
/* Utilty Function used to access a Map safely. */

#include <string>

template<typename Map>
auto& safe_access(Map& map, const std::string& key){
    auto it = map.find(key);
    if(it == map.end()){
        throw std::out_of_range("Failed to access the map "+ key + " not found.");
    }
    return it->second;
}