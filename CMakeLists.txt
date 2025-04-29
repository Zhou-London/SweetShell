cmake_minimum_required(VERSION 3.10)

project(SweetShell VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(swsh 
    src/main.cpp
    src/Util/safe_access.hpp
    src/TerminalInstance/TerminalInstance.hpp
    src/SweetShell_unit/SweetShell_unit.cpp
    src/SweetShell_unit/SweetShell_unit.hpp
)

target_include_directories(swsh PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)