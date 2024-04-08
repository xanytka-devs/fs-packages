#include "firesteel/rendering/renderer.hpp"
#ifdef VULKAN_RENDERING

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "firesteel/app.hpp"
#include "firesteel/log.hpp"
#include "firesteel/rendering/window.hpp"
#include "firesteel/input/keyboard.hpp"
#include "firesteel/input/mouse.hpp"
//#include <backends/imgui_impl_glfw.h>
//#include <backends/imgui_impl_vulkan.h>

namespace firesteel {

    unsigned int Window::width = 800;
    unsigned int Window::height = 600;
    bool ui_initialized = false;

    Window::Window() : m_title("Hello firesteel!"), m_window(nullptr) {}
    Window::Window(unsigned int t_width, unsigned int t_height, const char* t_title)
        : m_title(t_title), m_window(nullptr) {
        width = t_width; height = t_height;
    }
    Window::Window(unsigned int t_width, unsigned int t_height, std::string t_title)
        : m_title(t_title), m_window(nullptr) {
        width = t_width; height = t_height;
    }

    void Window::framebuffer_callback(GLFWwindow* t_window, int t_width, int t_height) {
        //Window processes.
    }

    void Window::size_callback(GLFWwindow* t_window, int t_width, int t_height) {
        //Update window params.
        width = t_width;
        height = t_height;
    }

    bool Window::initialize() {
        //Create window.
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(width, height, m_title.c_str(), NULL, NULL);
        if(!m_window) return false;
        //Set context.
        glfwMakeContextCurrent(m_window);
        if(m_vsync) glfwSwapInterval(1);
        else glfwSwapInterval(0);
        glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE);
        glfwInitHint(GLFW_RESIZABLE, GLFW_TRUE);
        LOG_INFO("Window '" + m_title + "' initialized.");
        return true;
    }

    void Window::update() {
        //Window processes.
        glfwSwapBuffers(m_window);
        glfwPollEvents();
        //Set clear color.
        glm::vec4 color = Renderer::get_clear_color();
    }

    void Window::gui_initialize() {
        //Initialize ImGui.
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        //Setup flags.
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_ViewportsEnable;
        //Create context for ImGui.
        //ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        //ImGui_ImplVulkan_Init();
        //ui_initialized = true;
        //LOG_INFO("ImGui initialized.");
    }

    void Window::gui_update() {
        if (!ui_initialized) return;
        //Update ImGui.
        //ImGui_ImplVulkan_NewFrame();
        //ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Window::gui_draw() {
        if (!ui_initialized) return;
        //Draw ImGui.
        ImGui::Render();
        //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void Window::set_init_params() {
        //Set viewport.
        glfwSetFramebufferSizeCallback(m_window, framebuffer_callback);
        glfwSetWindowSizeCallback(m_window, size_callback);
        //Callbacks for input.
        glfwSetKeyCallback(m_window, Keyboard::key_callback);
        glfwSetCursorPosCallback(m_window, Mouse::cursor_callback);
        glfwSetMouseButtonCallback(m_window, Mouse::button_callback);
        glfwSetScrollCallback(m_window, Mouse::scroll_callback);
        //Enable depth test.
    }

    int Window::get_param_i(WindowParam t_param) const {
        switch (t_param) {
        case firesteel::W_CURSOR:
            return m_cur_state;
            break;
        case firesteel::W_VSYNC:
            return m_vsync;
            break;
        default:
            return 0;
            break;
        }
    }

    bool Window::get_param_b(WindowParam t_param) const {
        return get_param_i(t_param) == 1;
    }

    void Window::set_param(WindowParam t_param, bool t_val) {
        set_param(t_param, (t_val ? 1 : 0));
    }

    void Window::set_param(WindowParam t_param, int t_val) {
        switch (t_param) {
        case W_VSYNC:
            m_vsync = t_val;
            glfwMakeContextCurrent(m_window);
            if(m_vsync) glfwSwapInterval(1);
            else glfwSwapInterval(0);
            break;
        case W_CURSOR:
            m_cur_state = CursorState(t_val);
            switch ((int)t_val) {
            case C_NONE:
                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
            case C_LOCKED:
                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
                break;
            case C_DISABLED:
                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;
            case C_HIDDEN:
                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                break;
            }
            break;
        default:
            break;
        }
    }

    void Window::set_param(WindowParam t_param, glm::vec2 t_vec) {
        switch (t_param) {
        case W_OPACITY:
            glfwSetWindowOpacity(m_window, t_vec.x);
            break;
        case W_POS:
            glfwSetWindowPos(m_window, (int)t_vec.x, (int)t_vec.y);
            break;
        case W_SIZE:
            glfwSetWindowSize(m_window, (int)t_vec.x, (int)t_vec.y);
            break;
        default:
            break;
        }
    }

    void Window::set_param(WindowParam t_param, const char* t_val) {
        switch (t_param) {
        case W_TITLE:
            m_title = t_val;
            glfwSetWindowTitle(m_window, t_val);
            break;
        default:
            break;
        }
    }

    void Window::set_param(WindowParam t_param, std::string t_val) {
        set_param(t_param, t_val.c_str());
    }

    void Window::close() {
        //Close window.
        glfwSetWindowShouldClose(m_window, true);
    }

    void Window::gui_shutdown() {
        if (!ui_initialized) return;
        //Shutdown ImGui.
        //ImGui_ImplVulkan_Shutdown();
        //ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    bool Window::closing() {
        return glfwWindowShouldClose(m_window) == 1;
    }

}

#endif // VULKAN_RENDERING