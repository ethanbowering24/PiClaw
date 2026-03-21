cmake_minimum_required(VERSION 3.23)

project(PiClaw)

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    set(IS_PI TRUE)
    message(STATUS "Target Hardware: Raspberry Pi")
else()
    set(IS_PI FALSE)
    message(STATUS "Target Hardware: PC (check for compile errors only)")
endif()

option(BUILD_CLIENT "Build client" ON)  # ON is the default
option(BUILD_SERVER "Build server" ON)  # ON is the default

if(BUILD_CLIENT)
    add_subdirectory(PiClaw_Client)
endif()

if(BUILD_SERVER)
    add_subdirectory(PiClaw_Server)
endif()

