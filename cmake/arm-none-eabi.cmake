set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# --- FIX 1: Skip linking during compiler checks ---
# This tells CMake: "Don't try to build a working .exe just to test the compiler.
# Just check if you can compile a .c file."
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Specify the cross compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# Hardware Specific Flags
set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# --- FIX 2: Add nosys.specs ---
# This provides dummy implementations for _exit, _sbrk, etc.
set(CMAKE_C_FLAGS_INIT "${MCU_FLAGS} --specs=nano.specs --specs=nosys.specs")
set(CMAKE_CXX_FLAGS_INIT "${MCU_FLAGS} --specs=nano.specs --specs=nosys.specs")
set(CMAKE_ASM_FLAGS_INIT "${MCU_FLAGS}")

# Search mode settings
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)