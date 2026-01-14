// main.cpp
//Código principal
#include "Scene.hpp"
#include <Window.hpp>
#include <SDL3/SDL_main.h>

using udit::Scene;
using udit::Window;

int main(int, char* [])
{
    constexpr unsigned w = 1024;
    constexpr unsigned h = 576;
    Window window("Proyecto Desierto Modular", w, h, { 3, 3 });
    Scene  scene(w, h);
    bool exit = false;

    do {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) exit = true;
            else if (event.type == SDL_EVENT_MOUSE_MOTION && (event.motion.state & SDL_BUTTON_LMASK)) {
                scene.on_drag(event.motion.xrel, event.motion.yrel);
            }
        }
        const bool* keys = SDL_GetKeyboardState(nullptr);
        scene.handle_input(keys);

        scene.update();
        scene.render();
        window.swap_buffers();
    } while (!exit);

    SDL_Quit();
    return 0;
}