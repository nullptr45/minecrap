// OpenGL
#include <glad/glad.h>

// Other includes
#include <SDL.h>
#include <chrono>
#include <iostream>

// imgui
#include "gfx/shader.h"
#include "gfx/texture.h"
#include "imgui.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

// constants
constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;
constexpr const char *TITLE = "Minecrap";

static float vertices[] = {
	// first triangle
	 0.5f,  0.5f, 0.0f,    1.0f, 0.0f,// top right
	 0.5f, -0.5f, 0.0f,    1.0f, 1.0f,// bottom right
	-0.5f,  0.5f, 0.0f,    0.0f, 0.0f,// top left 
	// second triangle 
	 0.5f, -0.5f, 0.0f,    1.0f, 1.0f,// bottom right
	-0.5f, -0.5f, 0.0f,    0.0f, 1.0f,// bottom left
	-0.5f,  0.5f, 0.0f,    0.0f, 0.0f // top left
};

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to init SDL" << std::endl;
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

    SDL_Window *window =
        SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

    if (window == nullptr) {
        std::cerr << "Failed to open window" << std::endl;
        return -1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::cerr << "Failed to create OpenGL context" << std::endl;
        return -1;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to load OpenGL extensions" << std::endl;
        return -1;
    }

    SDL_GL_MakeCurrent(window, context);

    glViewport(0.0f, 0.0f, WIDTH, HEIGHT);
    glClearColor(0.0f, 0.65f, 1.0f, 1.0f);

    // Initialize ImGui
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;           // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport /

    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        // style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 0.f;
        style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
    }
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 330");

    Shader shader("resources/shaders/basic.vert.glsl",
                  "resources/shaders/basic.frag.glsl");

    Texture texture("resources/textures/grass_block_side.png");
    texture.bind();

    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    uint32_t vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    shader.set_int("tex", texture.id);
    shader.bind();

    bool wireframe = false;

    auto last_time = std::chrono::high_resolution_clock::now();
    bool is_running = true;
    while (is_running) {
        // Delta time
        auto current_time = std::chrono::high_resolution_clock::now();
        float delta = (std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_time)).count() / 1e6f;
        last_time = std::chrono::high_resolution_clock::now();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
                case SDL_QUIT:
                    is_running = false;
                    break;
                default:
                    break;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Begin("Stats");

        ImGui::Checkbox("Wireframe", &wireframe);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        shader.bind();
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_Quit();

    return 0;
}
