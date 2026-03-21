project(PiClaw_Server)

set(SERVER_SOURCES
    src/main.cpp
    src/servo.cpp
)

if(IS_PI)
    # --- BUILD FOR RASPBERRY PI ---
    set(TARGET_NAME PiClaw_Server)
    add_executable(${TARGET_NAME} ${SERVER_SOURCES})
    
    find_library(WIRINGPI_LIB wiringPi REQUIRED)
    find_library(PIGPIO_LIB pigpio REQUIRED)
    target_link_libraries(${TARGET_NAME} PRIVATE ${WIRINGPI_LIB} ${PIPGPIO_LIB}) 

else()
    # --- BUILD FOR PC (CHECK ONLY) ---
    set(TARGET_NAME PiClaw_Server_Check)
    add_library(${TARGET_NAME} OBJECT ${SERVER_SOURCES})
        
    # PC-specific mock headers
    target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/PiHeaders)
endif()

# --- COMMON INCLUDES ---
target_include_directories(${TARGET_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/common
)
