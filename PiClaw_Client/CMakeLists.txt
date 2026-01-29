project(PiClaw_Client)

if(IS_PI)
    # --- BUILD FOR RASPBERRY PI ---
    add_executable(PiClaw_Client src/mpu.cpp)
    
    find_library(WIRINGPI_LIB wiringPi REQUIRED)
    # target_link_libraries(remote_client ${WIRINGPI_LIB} pthread)
    target_link_libraries(PiClaw_Client ${WIRINGPI_LIB})    

    target_include_directories(PiClaw_Client PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev
        ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev/I2Cdev
        ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev/MPU6050
        ${CMAKE_SOURCE_DIR}/common
    )    
else()
    # --- BUILD FOR PC (CHECK ONLY) ---
    add_library(PiClaw_Client_Check OBJECT 
        src/main.cpp
        src/glove.cpp
        src/mpu.cpp
    )

    target_include_directories(PiClaw_Client_Check PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev
        ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev/I2Cdev
        ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev/MPU6050
        ${CMAKE_SOURCE_DIR}/common
    )
        
    target_include_directories(PiClaw_Client_Check SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/PiHeaders)
endif()
