#pragma once

#include <string>

class Project_data{
public:
    long long id;
    std::string name;

    Project_data(){};
    Project_data(long long i, std::string name): id(i), name(name) {};

};