#pragma once

#include <SDL_events.h>
#include <SDL_scancode.h>
#include <glm/glm.hpp>

namespace input {

static constexpr float MOUSE_SENS = 0.1f;

void init();

void update();
void event(const SDL_Event& event);

bool key_down(SDL_Scancode key);
bool key_pressed(SDL_Scancode key);
bool key_released(SDL_Scancode key);

bool mouse_down(uint8_t button);
bool mouse_pressed(uint8_t button);
bool mouse_released(uint8_t button);

const glm::ivec2& mouse_pos();
const glm::ivec2& mouse_delta();
int mouse_wheel();

glm::vec2 move_vec();
glm::vec2 look_vec();

} // namespace input
