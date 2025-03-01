# Set minimum Cmake version
cmake_minimum_required(VERSION 3.11)

# Start project and set its name
project(unsafeTicks LANGUAGES CXX)

#set default build type to Debug for development and debugging purposes
set(default_build_type "Debug")

set(CMAKE_BUILD_DIR "build")

# The following is required for language servers (like clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)

# Set the Qt version and required components
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Add executables
add_executable(unsafeTicks 
    src/main.cpp 
    src/server-connection.cpp
    src/ticket.cpp
    src/exceptions-base.cpp
    src/QRCodeWindow.cpp
)

#add all external libraries
add_subdirectory(submodules/json)
add_subdirectory(submodules/cpp-httplib)
set(HMAC_WRAPPER "openssl" CACHE STRING "tell libcotp to use openssl")
add_subdirectory(submodules/libcotp)
set(WITHOUT_PNG "ON" CACHE STRING "tell qrencode to disable png support")
add_subdirectory(submodules/qrencode)

#link libraries
target_link_libraries(unsafeTicks
    nlohmann_json::nlohmann_json
    httplib
    cotp
    qrencode
    Qt5::Widgets
)
target_include_directories(unsafeTicks PUBLIC submodules/qrencode) #for some reason needed for qrencode.h file
