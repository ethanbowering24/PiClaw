project(PiClaw_Server)

if(IS_PI)
    # --- BUILD FOR RASPBERRY PI ---
    add_executable(PiClaw_Server src/main.cpp)
    
    find_library(WIRINGPI_LIB wiringPi REQUIRED)
    # target_link_libraries(remote_client ${WIRINGPI_LIB} pthread)
    target_link_libraries(PiClaw_Server ${WIRINGPI_LIB})
    target_include_directories(PiClaw_Server PRIVATE ${CMAKE_SOURCE_DIR}/common)
else()
    # --- BUILD FOR PC (CHECK ONLY) ---
    add_library(PiClaw_Server_Check OBJECT src/main.cpp)

    target_include_directories(PiClaw_Server_Check PRIVATE ${CMAKE_SOURCE_DIR}/common)
    target_include_directories(PiClaw_Server_Check PRIVATE ${CMAKE_SOURCE_DIR}/common/LinuxI2CDev)
    target_include_directories(PiClaw_Server_Check PRIVATE ${CMAKE_SOURCE_DIR}/common/LinuxI2CDev/I2Cdev)
    target_include_directories(PiClaw_Server_Check PRIVATE ${CMAKE_SOURCE_DIR}/common/LinuxI2CDev/MPU6050)

    target_include_directories(PiClaw_Server_Check SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/wiringPiHeader)
endif()
