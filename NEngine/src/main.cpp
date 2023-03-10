#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <iostream>

#include "vulkan_application.h"

SDL_Surface *win_surface = nullptr;
SDL_Window *window = nullptr;
nengine::vulkan_application *app = nullptr;

bool
loop()
{
    SDL_Event e = {};
    while (SDL_PollEvent(&e) != 0) {
        switch (e.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    return false;
                }
                break;
            default:
                break;
        }
    }
    return true;
}

bool
init_sdl_context()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Failed to init SDL" << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        "Vulkan triangle", 100, 100, 1280, 720, SDL_WINDOW_VULKAN);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        return false;
    }

    win_surface = SDL_GetWindowSurface(window);

    if (!win_surface) {
        std::cerr << "Failed to get surface" << std::endl;
        return false;
    }
    return true;
}

void
destroy_sdl2_context()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
    delete app;
}

int
main(int argc, char **argv)
{
    if (!init_sdl_context()) {
        return 1;
    }

    app = new nengine::vulkan_application(window);

    while (loop()) {
        SDL_Delay(16);
    }

    destroy_sdl2_context();
}