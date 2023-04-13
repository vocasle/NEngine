#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include <iostream>

#include "vulkan_application.h"

SDL_Surface *win_surface = nullptr;
SDL_Window *window = nullptr;
nengine::vulkan_application *app = nullptr;
bool is_window_visible = true;
bool running = true;

void
poll_events()
{
    SDL_Event e = {};
    while (SDL_PollEvent(&e) != 0) {
        // TODO Do not process mouse and keyboard input if ImGui already
        // processed it
        ImGui_ImplSDL2_ProcessEvent(&e);

        if (e.type == SDL_WINDOWEVENT) {
            switch (e.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    app->on_window_resized();
                    break;
                case SDL_WINDOWEVENT_SHOWN:
                case SDL_WINDOWEVENT_RESTORED:
                    is_window_visible = true;
                    break;
                case SDL_WINDOWEVENT_FOCUS_LOST:
                case SDL_WINDOWEVENT_HIDDEN:
                case SDL_WINDOWEVENT_MINIMIZED:
                    is_window_visible = false;
                    break;
                default:
                    break;
            }
        }
        else if (e.type == SDL_QUIT) {
            running = false;
        }
        else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }
    }
}

bool
init_sdl_context()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Failed to init SDL" << std::endl;
        return false;
    }

    window = SDL_CreateWindow("NEngine",
                              100,
                              100,
                              1280,
                              720,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
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

    while (running) {
        const uint64_t start = SDL_GetPerformanceCounter();

        poll_events();

        if (is_window_visible) {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            ImGui::ShowDemoWindow();

            app->draw_frame();
        }

        const uint64_t end = SDL_GetPerformanceCounter();
        const float elapsed =
            (end - start) / (float)SDL_GetPerformanceFrequency();
        SDL_SetWindowTitle(
            window, std::format("NEngine - {} FPS", 1.0f / elapsed).c_str());
    }

    destroy_sdl2_context();
}