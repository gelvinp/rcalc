#include "imgui_standard_backend.h"

#include "core/logger.h"

#include "app/application.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "assets/app_icon.png.gen.h"
#include "stb_image.h"

#include "app/renderers/imgui/imgui_renderer.h"

namespace RCalc {


ImGuiBackend& ImGuiBackend::get_platform_backend() {
    static ImGuiStandardBackend backend;
    return reinterpret_cast<ImGuiBackend&>(backend);
}


static void glfw_error_callback(int error, const char* description) {
    Logger::log_err("[GLFW] Error %d: %s\n", error, description);
}

Result<> ImGuiStandardBackend::init(Renderer::SubmitTextCallback cb_submit_text) {
    UNUSED(cb_submit_text);

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        Logger::log_err("Unable to initialize GLFW!");
        return Err(ERR_INIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    p_window = glfwCreateWindow(640, 480, "RCalc", nullptr, nullptr);
    if (p_window == nullptr) {
        Logger::log_err("Unable to create window!");
        return Err(ERR_INIT_FAILURE);
    }

    // Set app icon
    int x, y, n;
    unsigned char* icon = stbi_load_from_memory(Assets::app_icon_png.data(), (int)Assets::app_icon_png.size(), &x, &y, &n, 4);
    GLFWimage glfw_icon { 256, 256, icon };
    glfwSetWindowIcon(p_window, 1, &glfw_icon);
    stbi_image_free(icon);

    glfwMakeContextCurrent(p_window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    ImGui_ImplGlfw_InitForOpenGL(p_window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    return Ok();
}


void ImGuiStandardBackend::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(p_window);
    glfwTerminate();
}


void ImGuiStandardBackend::start_frame() {
    close_requested = glfwWindowShouldClose(p_window);

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiStandardBackend::render_frame() {
    ImGui::Render();

    int window_w, window_h;
    glfwGetFramebufferSize(p_window, &window_w, &window_h);

    glViewport(0, 0, window_w, window_h);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(p_window);
}

void ImGuiStandardBackend::recreate_font_atlas() {
    ImGui_ImplOpenGL3_CreateFontsTexture();
}


void ImGuiStandardBackend::copy_to_clipboard(const std::string_view& string) {
    glfwSetClipboardString(p_window, string.data());
}

}