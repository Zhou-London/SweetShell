cmake_minimum_required(VERSION 3.10)

project(SweetShell VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/external/SQLiteCpp)

add_executable(swsh 
    src/main.cpp
    src/Util/safe_access.hpp
    src/TerminalInstance/TerminalInstance.hpp
    src/SweetShell_unit/SweetShell_unit.cpp
    src/SweetShell_unit/SweetShell_unit.hpp
    src/SweetShell_database/SweetShell_database.cpp
    src/SweetShell_database/SweetShell_database.hpp
    src/Preset/Preset.cpp
    src/Preset/Preset.hpp
    src/Project_data/Project_data.hpp
    src/SweetShell_UI/swsh_ui.cpp
    src/SweetShell_UI/swsh_ui.hpp
    src/SweetShell_UI/Button.cpp
    src/SweetShell_UI/Button.hpp
    src/Signal/Signal.hpp
)

target_include_directories(swsh PRIVATE 
                        ${CMAKE_CURRENT_SOURCE_DIR}/src
                        ${CMAKE_CURRENT_SOURCE_DIR}/external/termcolor/include)

target_link_libraries(swsh PRIVATE
                    SQLiteCpp
                    sqlite3
                    pthread
                    dl
                    )