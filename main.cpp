#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "chip8.h"

constexpr int VIDEO_WIDTH  = 64;
constexpr int VIDEO_HEIGHT = 32;
constexpr int SCALE        = 10;

int main(int argc, char* argv[])
{
    //  SDL INIT
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

    //  CHIP-8 INIT 
    Chip8 chip8;
    static int counter = 0;
if (++counter % 500 == 0)
    SDL_SetWindowTitle(window, "RUNNING");


if (argc < 2) {
    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_ERROR,
        "ARGC ERROR",
        "No ROM argument received",
        nullptr
    );
    SDL_Delay(3000);
    return 1;
}

// SDL_ShowSimpleMessageBox(
//     SDL_MESSAGEBOX_INFORMATION,
//     "ARG OK",
//     argv[1],
//     nullptr
// );


if (!chip8.load(argv[1])) {
    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_ERROR,
        "ROM LOAD FAILED",
        argv[1],
        window
    );
    return 1;
}

    bool running = true;
    SDL_Event event;

    //  MAIN LOOP 
    while (running) {

        //  EVENTS 
        
while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
        running = false;
    }

    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        bool pressed = (event.type == SDL_KEYDOWN);

        uint8_t chip8Key = 0;
        bool mapped = true;

        switch (event.key.keysym.sym) {
            case SDLK_1: chip8Key = 0x1; break;
            case SDLK_2: chip8Key = 0x2; break;
            case SDLK_3: chip8Key = 0x3; break;
            case SDLK_4: chip8Key = 0xC; break;

            case SDLK_q: chip8Key = 0x4; break;
            case SDLK_w: chip8Key = 0x5; break;
            case SDLK_e: chip8Key = 0x6; break;
            case SDLK_r: chip8Key = 0xD; break;

            case SDLK_a: chip8Key = 0x7; break;
            case SDLK_s: chip8Key = 0x8; break;
            case SDLK_d: chip8Key = 0x9; break;
            case SDLK_f: chip8Key = 0xE; break;

            case SDLK_z: chip8Key = 0xA; break;
            case SDLK_x: chip8Key = 0x0; break;
            case SDLK_c: chip8Key = 0xB; break;
            case SDLK_v: chip8Key = 0xF; break;

            default:
                mapped = false;
                break;
        }

        if (mapped) {
            chip8.key[chip8Key] = pressed;

            if (pressed) {
                chip8.onKeyPress(chip8Key);
            }
        }
    }
}



        // CPU;

        // Run multiple cycles per frame
        for (int i = 0; i < 10; ++i) {
    chip8.cycle();
        }


        // RENDER
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

    //  CLEANUP 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
