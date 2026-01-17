#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "chip8.h"

constexpr int VIDEO_WIDTH  = 64;
constexpr int VIDEO_HEIGHT = 32;
constexpr int SCALE        = 10;

int main(int argc, char* argv[])
{
    // ---------- SDL INIT ----------
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        VIDEO_WIDTH * SCALE,
        VIDEO_HEIGHT * SCALE,
        SDL_WINDOW_SHOWN
    );

    if (!window) return 1;

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED
    );

    if (!renderer) return 1;

    // ---------- CHIP-8 INIT ----------
    Chip8 chip8;
    static int counter = 0;
if (++counter % 500 == 0)
    SDL_SetWindowTitle(window, "RUNNING");


    if (!chip8.load("roms/SPACEINVADERS.ch8")) {
    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_ERROR,
        "ROM LOAD FAILED",
        "chip8.load() returned false",
        window
    );
    SDL_Delay(3000);
    return 1;
}

    bool running = true;
    SDL_Event event;

    // ---------- MAIN LOOP ----------
    while (running) {

        // ---- EVENTS ----
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        // ---- CPU ----
        // Run multiple cycles per frame
        for (int i = 0; i < 10; ++i) {
            chip8.cycle();
        }

        // ---- RENDER ----
        static Uint32 lastTimerTick = SDL_GetTicks();

        if (SDL_GetTicks() - lastTimerTick >= 16) { // ~60 Hz
        chip8.updateTimers();
        lastTimerTick = SDL_GetTicks();
}
        
        if (chip8.drawFlag) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            for (int y = 0; y < VIDEO_HEIGHT; ++y) {
                for (int x = 0; x < VIDEO_WIDTH; ++x) {
                    if (chip8.gfx[y * VIDEO_WIDTH + x]) {
                        SDL_Rect pixel{
                            x * SCALE,
                            y * SCALE,
                            SCALE,
                            SCALE
                        };
                        SDL_RenderFillRect(renderer, &pixel);
                    }
                }
            }

            SDL_RenderPresent(renderer);
            chip8.drawFlag = false;
        }

        SDL_Delay(1); // prevent CPU hogging
    }

    // ---------- CLEANUP ----------
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
