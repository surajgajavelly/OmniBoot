cmake_minimum_required(VERSION 3.20)

# Project Config
project(OmniBoot VERSION 0.1.0 LANGUAGES C ASM)

# --- 1. Toolchain Setup ---
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm-none-eabi.cmake")

# --- 2. CPU Flags ---
set(CPU_FLAGS 
    -mcpu=cortex-m4 
    -mthumb 
    -mfpu=fpv4-sp-d16 
    -mfloat-abi=hard
)

# Compile Options
add_compile_options(${CPU_FLAGS} -Wall -Wextra -Og -g3)

# Link Options (FIXED: Removed -specs=nano.specs to prevent "rename spec" error)
add_link_options(${CPU_FLAGS} -Wl,--gc-sections)

# --- 3. Define Target Board ---
set(TARGET_BOARD "stm32f446re")

# --- 4. Include Paths ---
include_directories(
    core/inc
    target/${TARGET_BOARD}/bsp/inc
    target/common
)

# --- 5. Source Files ---
file(GLOB_RECURSE CORE_SOURCES "core/src/*.c")
file(GLOB_RECURSE BSP_SOURCES "target/${TARGET_BOARD}/bsp/src/*.c")
set(STARTUP_CODE "target/${TARGET_BOARD}/bsp/src/startup_stm32.c")

# --- 6. The Executable ---
add_executable(${PROJECT_NAME}.elf
    ${CORE_SOURCES}
    ${BSP_SOURCES}
    ${STARTUP_CODE}
)

# --- 7. Linker Script ---
set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/target/${TARGET_BOARD}/linker/bootloader.ld")

target_link_options(${PROJECT_NAME}.elf PRIVATE 
    -T${LINKER_SCRIPT}
    -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map
)

# --- 8. Post-Build: Generate Binary ---
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND arm-none-eabi-size ${PROJECT_NAME}.elf
    COMMENT "Generating ${PROJECT_NAME}.bin"
)