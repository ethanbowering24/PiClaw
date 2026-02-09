project(PiClaw_Client)

add_subdirectory(include/Fusion)

set(MPU_SOURCES
	include/LinuxI2CDev/I2Cdev/I2Cdev.cpp
	include/LinuxI2CDev/MPU6050/MPU6050.cpp
)

add_executable(PiClaw_Client 
    src/mpu.cpp
    ${MPU_SOURCES}
)

target_include_directories(PiClaw_Client PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev/I2Cdev
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LinuxI2CDev/MPU6050
    ${CMAKE_SOURCE_DIR}/common
)    

target_link_libraries(PiClaw_Client PRIVATE Fusion)