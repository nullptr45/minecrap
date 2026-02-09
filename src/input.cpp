#include "input.h"
#include <SDL_scancode.h>

namespace {

uint8_t keys_curr[SDL_NUM_SCANCODES];
uint8_t keys_prev[SDL_NUM_SCANCODES];

uint32_t mouse_curr = 0;
uint32_t mouse_prev = 0;

glm::ivec2 mouse_pos = {};
glm::ivec2 mouse_delta = {};
int        mouse_wheel = 0;

} // namespace

namespace input {

void init()
{
    SDL_memset(keys_curr, 0, sizeof(keys_curr));
    SDL_memset(keys_prev, 0, sizeof(keys_prev));
}

void update()
{
    memcpy(keys_prev, keys_curr, sizeof(keys_curr));
    mouse_prev = mouse_curr;

    ::mouse_delta = {};
    ::mouse_wheel = 0;

    SDL_PumpEvents();
    memcpy(keys_curr, SDL_GetKeyboardState(nullptr), sizeof(keys_curr));

    int x, y;
    mouse_curr = SDL_GetMouseState(&x, &y);
    ::mouse_pos = {x, y};
}

void event(const SDL_Event& event)
{
    switch (event.type) {
    case SDL_MOUSEMOTION:
        ::mouse_delta.x += event.motion.xrel;
        ::mouse_delta.y += event.motion.yrel;
        break;
    case SDL_MOUSEWHEEL:
        ::mouse_wheel += event.wheel.y;
        break;
    }
}

bool key_down(SDL_Scancode key)
{
    return keys_curr[key];
}

bool key_pressed(SDL_Scancode key)
{
    return keys_curr[key] && !keys_prev[key];
}

bool key_released(SDL_Scancode key)
{
    return !keys_curr[key] && keys_prev[key];
}

bool mouse_down(uint8_t button)
{
    return mouse_curr & SDL_BUTTON(button);
}

bool mouse_pressed(uint8_t button)
{
    return (mouse_curr & SDL_BUTTON(button)) && !(mouse_prev & SDL_BUTTON(button));
}

bool mouse_released(uint8_t button)
{
    return !(mouse_curr & SDL_BUTTON(button)) && (mouse_prev & SDL_BUTTON(button));
}

const glm::ivec2& mouse_pos()
{
    return ::mouse_pos;
}

const glm::ivec2& mouse_delta()
{
    return ::mouse_delta;
}

int mouse_wheel()
{
    return ::mouse_wheel;
}

} // namespace input
