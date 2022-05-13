cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3")

project(fbtt)

include(GNUInstallDirs)

add_library(fbtt INTERFACE)

set_target_properties(${PROJECT_NAME} PROPERTIES
   PUBLIC_HEADER include/fbtt.hpp
)

configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)

target_include_directories(${PROJECT_NAME} INTERFACE include)

# provides support for make install
install(
    FILES include/assert.hpp
        include/assertionFailure.hpp
        include/errorConcepts.hpp
        include/fbtt.hpp
        include/functionConcepts.hpp
        include/multiTest.hpp
        include/terminalColor.hpp
        include/test.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)