project(PiClaw_Client)
add_subdirectory(include/Fusion)

set(CLIENT_SOURCES
    src/mpu.cpp
	include/LinuxI2CDev/I2Cdev/I2Cdev.cpp
	include/LinuxI2CDev/MPU6050/MPU6050.cpp
    include/LinuxI2CDev/QMC5883P/QMC5883P.cpp
    include/LinuxI2CDev/ADS1115/ADS1115.cpp
)

if(IS_PI)
    # --- BUILD FOR RASPBERRY PI ---
    set(TARGET_NAME PiClaw_Client)
    add_executable(${TARGET_NAME} ${CLIENT_SOURCES})
    
    find_library(WIRINGPI_LIB wiringPi REQUIRED)
    target_link_libraries(${TARGET_NAME} PRIVATE ${WIRINGPI_LIB} Fusion) 

else()
    # --- BUILD FOR PC (CHECK ONLY) ---
    set(TARGET_NAME PiClaw_Client_Check)
    add_library(${TARGET_NAME} OBJECT ${CLIENT_SOURCES})
    
    target_link_libraries(${TARGET_NAME} PRIVATE Fusion)
    
    # PC-specific mock headers
    target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/PiHeaders)
endif()

# --- COMMON INCLUDES ---
target_include_directories(${TARGET_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev/I2Cdev
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev/MPU6050
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev/QMC5883P
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev/ADS1115
    ${CMAKE_SOURCE_DIR}/common
)