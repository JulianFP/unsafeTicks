# Set minimum Cmake version
cmake_minimum_required(VERSION 3.11)

# Start project and set its name
project(ticketmaster-but-worse LANGUAGES CXX)

# The following is required for language servers (like clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add executable
add_executable(ticketmaster-but-worse src/main.cpp)

#add json library
add_subdirectory(submodules/json)
target_link_libraries(ticketmaster-but-worse PRIVATE nlohmann_json::nlohmann_json)

#add httplib library
add_subdirectory(submodules/cpp-httplib)
target_link_libraries(ticketmaster-but-worse PRIVATE httplib)
