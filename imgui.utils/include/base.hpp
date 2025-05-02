#ifndef FS_IMGUI_BASE
#define FS_IMGUI_BASE

#include <imgui.h>
#include <../external/imgui/misc/cpp/imgui_stdlib.h>
#include <../external/imgui/backends/imgui_impl_glfw.h>
#include <../external/imgui/backends/imgui_impl_opengl3.h>
#include <../external/imgui/imgui_internal.h>
#include <string>
#include "../engine/include/utils/utils.hpp"

namespace FSImGui {

    static ImGuiDockNodeFlags defaultDockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoWindowMenuButton;
    static ImGuiWindowFlags defaultDockspaceWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    static ImFont* LoadFont(const std::string& tFontPath, const float tFontSize = 12.0f, const bool tClear = false) {
        ImGuiIO& io = ImGui::GetIO();
        if(tClear) io.Fonts->Clear();
        // Base font
        return io.Fonts->AddFontFromFileTTF(tFontPath.c_str(), tFontSize, NULL, io.Fonts->GetGlyphRangesCyrillic());
    }

    float vec4[4] = { 0,0,0,0 };
    float vec3[3] = { 0,0,0 };
    float vec2[2] = { 0,0 };
    static bool ColorEdit3(const char* tName, glm::vec3* tColor) {
        vec3[0] = tColor->r;
        vec3[1] = tColor->g;
        vec3[2] = tColor->b;
        bool b = ImGui::ColorEdit3(tName, vec3);
        if (b) {
            tColor->x = vec3[0];
            tColor->y = vec3[1];
            tColor->z = vec3[2];
        }
        return b;
    }
    static bool DragFloat4(const char* tName, glm::vec4* tFloats, const float& tSpeed = 0.1f) {
        vec4[0] = tFloats->x;
        vec4[1] = tFloats->y;
        vec4[2] = tFloats->z;
        vec4[3] = tFloats->w;
        bool b = ImGui::DragFloat4(tName, vec4, tSpeed);
        if (b) {
            tFloats->x = vec4[0];
            tFloats->y = vec4[1];
            tFloats->z = vec4[2];
            tFloats->w = vec4[3];
        }
        return b;
    }
    static bool DragLFloat4(const char* tName, float& tF1, float& tF2, float& tF3, float& tF4, const float& tSpeed = 0.1f) {
        vec4[0] = tF1;
        vec4[1] = tF2;
        vec4[2] = tF3;
        vec4[3] = tF4;
        bool b = ImGui::DragFloat4(tName, vec4, tSpeed);
        if (b) {
            tF1 = vec4[0];
            tF2 = vec4[1];
            tF3 = vec4[2];
            tF4 = vec4[3];
        }
        return b;
    }
    static bool DragFloat3(const char* tName, glm::vec3* tFloats, const float& tSpeed = 0.1f) {
        vec3[0] = tFloats->x;
        vec3[1] = tFloats->y;
        vec3[2] = tFloats->z;
        bool b = ImGui::DragFloat3(tName, vec3, tSpeed);
        if(b) {
            tFloats->x = vec3[0];
            tFloats->y = vec3[1];
            tFloats->z = vec3[2];
        }
        return b;
    }
    static bool DragLFloat3(const char* tName, float& tF1, float& tF2, float& tF3, const float& tSpeed = 0.1f) {
        vec3[0] = tF1;
        vec3[1] = tF2;
        vec3[2] = tF3;
        bool b = ImGui::DragFloat3(tName, vec3, tSpeed);
        if (b) {
            tF1 = vec3[0];
            tF2 = vec3[1];
            tF3 = vec3[2];
        }
        return b;
    }
    static bool DragFloat2(const char* tName, glm::vec2* tFloats, const float& tSpeed = 0.1f) {
        vec2[0] = tFloats->x;
        vec2[1] = tFloats->y;
        bool b = ImGui::DragFloat2(tName, vec2, tSpeed);
        if (b) {
            tFloats->x = vec2[0];
            tFloats->y = vec2[1];
        }
        return b;
    }
    static bool DragLFloat2(const char* tName, float& tF1, float& tF2, const float& tSpeed = 0.1f) {
        vec2[0] = tF1;
        vec2[1] = tF2;
        bool b = ImGui::DragFloat2(tName, vec2, tSpeed);
        if (b) {
            tF1 = vec2[0];
            tF2 = vec2[1];
        }
        return b;
    }

}

#endif // !FS_IMGUI_BASE