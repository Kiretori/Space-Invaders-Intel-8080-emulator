cmake_minimum_required(VERSION 3.1)

project(Space-Invaders-Emulator
    VERSION 1.0
    DESCRIPTION "Space Invaders and Intel 8080 Emulator"
    LANGUAGES C)

add_executable("invaders"
    game/invaders.c
    game/hardware.c
    src/opcodes.c
    src/emu8080.c)

target_include_directories("invaders" PUBLIC src game)
target_compile_options("invaders" PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-function -Wno-unused-result -Wno-unused-parameter)
target_link_libraries("invaders" -lSDL2 SDL2_mixer)