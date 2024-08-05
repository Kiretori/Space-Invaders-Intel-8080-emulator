#include <stdio.h>
#include <SDL2/SDL.h>
#include "../src/emu8080.h"
#include "hardware.h"

// Map IO ports to CPU IO memory
void port_init(State8080 *cpu) {
    cpu->input[INP1] = read_inp1;
    cpu->input[INP2] = read_inp2;

    cpu->input[SHFT_IN] = read_shift;
    cpu->output[SHFT_DATA] = write_shift;
    cpu->output[SHFTAMNT] = write_shift_amnt;

    cpu->output[SOUND1] = write_snd1;
    cpu->output[SOUND2] = write_snd2;

    cpu->output[WATCHDOG] = write_watchdog;
}

// Initializes SDL
bool init_SDL() {
    return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) >= 0;
}

// Create the SDL window
SDL_Window *create_window() {
    int window_width = DISP_WIDTH * DISP_SCALE;
    int window_height = DISP_HEIGHT * DISP_SCALE;

    SDL_Window *new_window = SDL_CreateWindow("Space Invaders",
                                              SDL_WINDOWPOS_CENTERED,
                                              SDL_WINDOWPOS_CENTERED,
                                              window_width,
                                              window_height,
                                              SDL_WINDOW_SHOWN);

    return new_window;
}

int main(void) {
    State8080 state;
    Reset8080(&state);
    if    (!LoadRomIntoMemory(&state, "roms/invaders.h", 0x0000)
        || !LoadRomIntoMemory(&state, "roms/invaders.g", 0x0800)
        || !LoadRomIntoMemory(&state, "roms/invaders.f", 0x1000)
        || !LoadRomIntoMemory(&state, "roms/invaders.e", 0x1800)) {
        fprintf(stderr, "Could not open Space Invaders ROM files.\n");
        return 1;
    }
    port_init(&state);

    if (!init_SDL()) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = create_window();
    if (!window) {
        fprintf(stderr, "Could not create SDL window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (!surface) {
        fprintf(stderr, "Could not create SDL surface: %s\n", SDL_GetError());
        return 1;
    }

    if (!audio_init()) {
        fprintf(stderr, "Could not initialize SDL mixer.\n");
    }

    while(!state.exit) {
        // Draw the screen and handle input
        display_draw(window, surface, &state);
        state.exit = !handle_input();

        uint32_t ticks = SDL_GetTicks();

        // Execute all cycles before a half-screen refresh
        while (state.total_cycles < (VBLANK_RATE / 2)) {
            Emulate8080Op(&state);
        }
        cpu_req_interrupt(&state, 0xcf);      // 0xcf = RST 1

        // Execute all cycles before a full-screen refresh
        while (state.total_cycles < VBLANK_RATE) {
            Emulate8080Op(&state);
        }
        cpu_req_interrupt(&state, 0xd7);      // 0xd7 = RST 2

        // Reset cycle counter and sleep until end of refresh period
        state.total_cycles = 0;
        SDL_Delay((1000 / REFRESH_RATE) - (SDL_GetTicks() - ticks));
    }

    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    audio_quit();
    SDL_Quit();
}