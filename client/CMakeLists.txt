# Set minimum Cmake version
cmake_minimum_required(VERSION 3.11)

# Start project and set its name
project(ticketmaster-but-worse LANGUAGES CXX)

# The following is required for language servers (like clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)

# Set the Qt version and required components
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Find and link qrencode library
find_package(PkgConfig REQUIRED)
pkg_check_modules(QREncode REQUIRED libqrencode)

# Add executable
add_executable(ticketmaster-but-worse src/main.cpp src/QRCodeWindow.cpp)


# Link Qt libraries to the ticketmaster-but-worse app
target_link_libraries(ticketmaster-but-worse PRIVATE Qt5::Widgets)


#add json library
add_subdirectory(submodules/json)
target_link_libraries(ticketmaster-but-worse PRIVATE nlohmann_json::nlohmann_json)

#add httplib library
add_subdirectory(submodules/cpp-httplib)
target_link_libraries(ticketmaster-but-worse PRIVATE httplib)  


# Link qrencode to the executable
add_subdirectory(submodules/qrencode)
target_link_libraries(ticketmaster-but-worse PRIVATE qrencode)