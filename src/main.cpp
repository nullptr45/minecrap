#include "camera.h"
#include "input.h"
#include "shader.h"
#include "texture.h"

#include <SDL.h>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <cstdint>
#include <iostream>

// imgui
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <imgui.h>

static constexpr int         WINDOW_WIDTH = 1280;
static constexpr int         WINDOW_HEIGHT = 720;
static constexpr const char* WINDOW_TITLE = "Minecrap";

static constexpr float MOVE_SPEED = 20.f;

// clang-format off
static constexpr float vertices[] = {
    // Front
     0.5f,  0.5f,  0.5f, 1.f, 1.f,
    -0.5f,  0.5f,  0.5f, 0.f, 1.f,
    -0.5f, -0.5f,  0.5f, 0.f, 0.f,
     0.5f, -0.5f,  0.5f, 1.f, 0.f,

    // Back
    -0.5f,  0.5f, -0.5f, 1.f, 1.f,
     0.5f,  0.5f, -0.5f, 0.f, 1.f,
     0.5f, -0.5f, -0.5f, 0.f, 0.f,
    -0.5f, -0.5f, -0.5f, 1.f, 0.f,

    // Left
    -0.5f,  0.5f,  0.5f, 1.f, 1.f,
    -0.5f,  0.5f, -0.5f, 0.f, 1.f,
    -0.5f, -0.5f, -0.5f, 0.f, 0.f,
    -0.5f, -0.5f,  0.5f, 1.f, 0.f,

    // Right
     0.5f,  0.5f, -0.5f, 1.f, 1.f,
     0.5f,  0.5f,  0.5f, 0.f, 1.f,
     0.5f, -0.5f,  0.5f, 0.f, 0.f,
     0.5f, -0.5f, -0.5f, 1.f, 0.f,

    // Top
     0.5f,  0.5f, -0.5f, 1.f, 1.f,
    -0.5f,  0.5f, -0.5f, 0.f, 1.f,
    -0.5f,  0.5f,  0.5f, 0.f, 0.f,
     0.5f,  0.5f,  0.5f, 1.f, 0.f,

    // Bottom
     0.5f, -0.5f,  0.5f, 1.f, 1.f,
    -0.5f, -0.5f,  0.5f, 0.f, 1.f,
    -0.5f, -0.5f, -0.5f, 0.f, 0.f,
     0.5f, -0.5f, -0.5f, 1.f, 0.f,
};

static constexpr uint32_t indices[] = {
    0,  1,  2,  0,  2,  3,  // Front
    4,  5,  6,  4,  6,  7,  // Back
    8,  9,  10, 8,  10, 11, // Left
    12, 13, 14, 12, 14, 15, // Right
    16, 17, 18, 16, 18, 19, // Top
    20, 21, 22, 20, 22, 23  // Bottom
};
// clang-format on

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to init SDL2: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    if (!window) {
        std::cerr << "Failed to open window" << std::endl;
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    SDL_GLContext gl_ctx = SDL_GL_CreateContext(window);
    if (!gl_ctx) {
        std::cerr << "Failed to create OpenGL context" << std::endl;
        return -1;
    }
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to load OpenGL extensions" << std::endl;
        return -1;
    }

    SDL_GL_MakeCurrent(window, gl_ctx);
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.33, 0.81, 0.92, 1.0);

    Shader shader("assets/shaders/world.vert.glsl", "assets/shaders/world.frag.glsl");
    shader.bind();
    Texture texture("assets/textures/grass.jpg");

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo, ibo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5,
                          reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    Camera    camera{};
    glm::vec3 player_pos{0.f};

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_ctx);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    input::init();
    bool     is_running = true;
    uint64_t curr_time = SDL_GetPerformanceCounter();
    while (is_running) {
        // Delta time
        uint64_t prev_time = curr_time;
        curr_time = SDL_GetPerformanceCounter();
        float dt = (float) ((curr_time - prev_time) / (float) SDL_GetPerformanceFrequency());

        // Event polling
        input::update();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            input::event(event);
            if (event.type == SDL_QUIT) {
                is_running = false;
            } else if (event.type == SDL_WINDOWEVENT &&
                       event.window.event == SDL_WINDOWEVENT_RESIZED) {
                glViewport(0, 0, event.window.data1, event.window.data2);
                camera.set_aspect((float) event.window.data1 / event.window.data2);
            }
        }

        if (input::key_pressed(SDL_SCANCODE_ESCAPE)) {
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }

        glm::vec2 look = input::look_vec();
        camera.add_yaw(look.x);
        camera.add_pitch(look.y);

        glm::vec2 move = input::move_vec();
        glm::vec3 offset = camera.right() * move.x + camera.forward() * move.y;
        player_pos += offset * MOVE_SPEED * dt;
        camera.set_position(player_pos);

        // Begin rendering
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui::Begin("Stats");
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::End();

        glm::mat4 model = glm::mat4(1.0f);
        shader.set_uniform("model", model);

        shader.set_uniform("view", camera.view());
        shader.set_uniform("proj", camera.proj());

        shader.bind();
        texture.bind();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
