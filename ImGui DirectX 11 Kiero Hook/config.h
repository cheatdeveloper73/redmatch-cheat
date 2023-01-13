#pragma once

#include "imgui/imgui.h"
#include "menu/keybind.h"

struct Color
{
    float r, g, b, a;
    Color()
    {
        r = 1.f, g = 1.f, b = 1.f, a = 1.f;
    }
    Color(float _r, float _g, float _b, float _a)
    {
        this->r = _r;
        this->g = _g;
        this->b = _b;
        this->a = _a;
    }
    ImVec4 x()
    {
        return ImVec4(r, g, b, a);
    }
    ImU32 c()
    {
        return IM_COL32(r * 255.f, g * 255.f, b * 255.f, a * 255.f);
    }
    ImU32 c(int m)
    {
        return IM_COL32((r * 255.f) - m, (g * 255.f) - m, (b * 255.f) - m, (a * 255.f) - m);
    }
};

namespace config
{

    inline bool enable_aimbot = false;
    inline int aimbot_type = 0;
    inline float aimbot_fov = 100.f;
    inline float aimbot_smoothness = 3.f;
    inline bool enable_smoothing = true;
    inline bool show_fov = true;
    inline Color fov_color{1.f, 1.f, 1.f, 1.f};
    
    inline bool enable_box_esp = true;
    inline Color box_esp_visible{ 1.f, 1.f, 1.f, 1.f };
    inline Color box_esp_invisible{ 0.5f, 0.5f, 0.5f, 0.5f };

    inline bool enable_healthbar = true;

    inline bool enable_name_esp = true;
    inline Color name_esp_visible{ 1.f, 1.f, 1.f, 1.f };
    inline Color name_esp_invisible{ 0.5f, 0.5f, 0.5f, 0.5f };

    inline bool weapon_name_esp = true;
    inline Color weapon_esp_visible{ 1.f, 1.f, 1.f, 1.f };
    inline Color weapon_esp_invisible{ 0.5f, 0.5f, 0.5f, 0.5f };

    inline bool enable_chams = true;
    inline Color chams_visible{ 1.f, 1.f, 1.f, 1.f };
    inline Color chams_invisible{ 0.5f, 0.5f, 0.5f, 0.5f };

    inline bool enable_no_spread = false;
    inline bool enable_infinite_ammo = false;
    inline bool enable_rapid_fire = false;

    inline CKeybind flyhack_keybind{ "flyhack" };
    inline CKeybind aimbot_keybind{ "aimbot" };

}