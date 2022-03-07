
#include "menu_helpers.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"
#include "droid.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"

#include "Menu.hpp"
#include "ConfigSystem.h"
#include "helpers/math.hpp"

static ConVar* cl_mouseenable = nullptr;

constexpr static float get_sidebar_item_width()
{
    return 150.0f;
}
constexpr static float get_sidebar_item_height()
{
    return  50.0f;
}

namespace ImGuiEx
{
    inline bool ColorEdit4(const char* label, Color* v, ImGuiColorEditFlags flags = 0)
    {
        auto clr = ImVec4
        {
            v->r() / 255.0f,
            v->g() / 255.0f,
            v->b() / 255.0f,
            v->a() / 255.0f
        };

        if (ImGui::ColorEdit4(label, &clr.x, flags))
        {
            v->SetColor(clr.x, clr.y, clr.z, clr.w);
            return true;
        }
        return false;
    }
    inline bool ColorEdit3(const char* label, Color* v)
    {
        return ColorEdit4(label, v, false);
    }
}

int MenuHelper::get_fps()
{
    using namespace std::chrono;
    static int count = 0;
    static auto last = high_resolution_clock::now();
    auto now = high_resolution_clock::now();
    static int fps = 0;

    count++;

    if(duration_cast<milliseconds>(now - last).count() > 1000)
    {
        fps = count;
        count = 0;
        last = now;
    }

    return fps;
}

void MenuHelper::Initialize()
{
    MenuHelper::Get()._visible = true;

    cl_mouseenable = g_CVar->FindVar("cl_mouseenable");

    CreateStyle();
    Menu::Get().Initialize();
}

void MenuHelper::Shutdown()
{
    ImGui_ImplDX9_Shutdown();
    cl_mouseenable->SetValue(true);
}

void MenuHelper::OnDeviceLost()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void MenuHelper::OnDeviceReset()
{
    ImGui_ImplDX9_CreateDeviceObjects();
}

void MenuHelper::Render()
{
    Menu::Get().DrawRadar();

    ImGui::GetIO().MouseDrawCursor = MenuHelper::Get()._visible;
    if (!MenuHelper::Get()._visible)
    {
        return;
    }

    Menu::Get().Render();
    
    //ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiSetCond_Once);
    //ImGui::SetNextWindowSize(ImVec2{ 1000, 0 }, ImGuiSetCond_Once);
}

void MenuHelper::Show()
{
    MenuHelper::Get()._visible = true;
    cl_mouseenable->SetValue(false);
}

void MenuHelper::Hide()
{
    MenuHelper::Get()._visible = false;
    cl_mouseenable->SetValue(true);
}

void MenuHelper::Toggle()
{
    cl_mouseenable->SetValue(MenuHelper::Get()._visible);
    MenuHelper::Get()._visible = !MenuHelper::Get()._visible;
}

void MenuHelper::CreateStyle()
{
    ImGui::StyleColorsDark();
    ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);
    _style.FrameRounding = 0.f;
    _style.WindowRounding = 0.f;
    _style.ChildRounding = 0.f;
    _style.Colors[ImGuiCol_Button] = ImVec4(0.260f, 0.590f, 0.980f, 0.670f);
    _style.Colors[ImGuiCol_Header] = ImVec4(0.260f, 0.590f, 0.980f, 0.670f);
    _style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.260f, 0.590f, 0.980f, 1.000f);
    //_style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.000f, 0.545f, 1.000f, 1.000f);
    //_style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.060f, 0.416f, 0.980f, 1.000f);
    _style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.30f, 1.0f);
    _style.Colors[ImGuiCol_WindowBg] = ImVec4(0.000f, 0.009f, 0.120f, 0.940f);
    _style.Colors[ImGuiCol_PopupBg] = ImVec4(0.076f, 0.143f, 0.209f, 1.000f);
    ImGui::GetStyle() = _style;
}

bool MenuHelper::components::StartWindow(std::string title, ImVec2 size, ImGuiWindowFlags flags)
{
    CurrentItem = 0;
    FrameStarted = true;
    style = &ImGui::GetStyle();
    x = size.x;
    y = size.y;
    ImGui::SetNextWindowSize(size, ImGuiSetCond_Once);
    InColumns = false;
    return ImGui::Begin(title.data(), &MenuHelper::Get()._visible, flags);
}

void MenuHelper::components::EndWindow()
{
    FrameStarted = false;
    ImGui::End();
}

void MenuHelper::components::NavbarIcons(char* items[], char* hovered_items[], int size, int& selected, ImFont* font)
{
    ImGuiIO& io = ImGui::GetIO();
    if (!FrameStarted)
    {
        return;
    }
    //float SingleWight = (x / size) - (16.f / size);
    float SingleWight = 0.f;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    //ImGui::PushFont(font);
    if (!InColumns)
    {
        SingleWight = (x / size) - (16.f / size);
    }
    else
    {
        SingleWight = ((x / ColumnSize) / size) - (16.f / size);
    }
    for (int i = 0; i < size; i++)
    {
        char* text = "";
        if (NavbarHovered[CurrentItem][i])
        {
            ImGui::PushFont(io.FontDefault);
            text = hovered_items[i];
        }
        else
        {
            text = items[i];
            ImGui::PushFont(font);
        }
        ImVec4 OrgTextClr = style->Colors[ImGuiCol_Text];
        ImVec4 Clr = i == selected ? ImVec4{ 1.f, 0.f, 0.f, 1.f } :
                     OrgTextClr;
        style->Colors[ImGuiCol_Text] = Clr;
        if (ImGui::Button(text, ImVec2(SingleWight, 25.f)))
        {
            selected = i;
        }
        NavbarHovered[CurrentItem][i] = ImGui::IsItemHovered();
        style->Colors[ImGuiCol_Text] = OrgTextClr;
        if (i != size)
        {
            ImGui::SameLine();
        }
        ImGui::PopFont();
    }
    ImGui::PopStyleVar(4);
    ImGui::Spacing();
    CurrentItem++;
}

void MenuHelper::components::Navbar(char* items[], int size, int& selected)
{
    ImGuiIO& io = ImGui::GetIO();
    if (!FrameStarted)
    {
        return;
    }
    float SingleWight = 0.f;
    if(!InColumns)
    {
        SingleWight = (x / size) - (16.f / size);
    }
    else
    {
        SingleWight = ((x / ColumnSize) / size) - (16.f / size);
    }
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    //ImGui::PushFont(font);
    for (int i = 0; i < size; i++)
    {
        ImVec4 OrgTextClr = style->Colors[ImGuiCol_Text];
        ImVec4 Clr = i == selected ? ImVec4{ 1.f, 0.f, 0.f, 1.f } :
                     OrgTextClr;
        style->Colors[ImGuiCol_Text] = Clr;
        if (ImGui::Button(items[i], ImVec2(SingleWight, 25.f)))
        {
            selected = i;
        }
        NavbarHovered[CurrentItem][i] = ImGui::IsItemHovered();
        style->Colors[ImGuiCol_Text] = OrgTextClr;
        if (i != size)
        {
            ImGui::SameLine();
        }
    }
    ImGui::PopStyleVar(4);
    ImGui::Spacing();
    CurrentItem++;
}

void MenuHelper::components::HelpMarker(const char* text)
{
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void MenuHelper::components::Checkbox(std::string text, std::string settings_name)
{
    bool b = g_Config.GetBool(settings_name);
    bool org = b;
    ImGui::Checkbox(text.c_str(), &b);
    if (b != org)
    {
        g_Config.Set(settings_name, b);
    }
    CurrentItem++;
}

void MenuHelper::components::ColorBox(std::string text, std::string settings_name_color)
{
    Color c = g_Config.GetColor(settings_name_color);
    Color orgc = c;
    std::string color_name = " ";
    for (int i = 0; i < CurrentItem; i++)
    {
        color_name += " ";
    }
    ImGuiEx::ColorEdit4(color_name.c_str(), &c, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    if (c != orgc)
    {
        g_Config.Set(settings_name_color, c);
    }
    ImGui::SameLine();
    ImGui::Text(text.c_str());
    CurrentItem++;
}

void MenuHelper::components::ColorBox2(std::string text, std::string settings_name_color, std::string settings_name_color2)
{
    Color c = g_Config.GetColor(settings_name_color);
    Color c2 = g_Config.GetColor(settings_name_color2);
    Color orgc = c;
    Color orgc2 = c2;
    std::string color_name = " ";
    for (int i = 0; i < CurrentItem; i++)
    {
        color_name += " ";
    }
    std::string color_name2 = color_name + " ";
    ImGuiEx::ColorEdit4(color_name.c_str(), &c, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    if (c != orgc)
    {
        g_Config.Set(settings_name_color, c);
    }
    ImGui::SameLine();
    ImGuiEx::ColorEdit4(color_name2.c_str(), &c2, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    if (c2 != orgc2)
    {
        g_Config.Set(settings_name_color2, c2);
    }
    ImGui::SameLine();
    ImGui::Text(text.c_str());
    CurrentItem++;
}

void MenuHelper::components::ColorCheckbox(std::string text, std::string settings_name_bool, std::string settings_name_color)
{
    Color c = g_Config.GetColor(settings_name_color);
    Color orgc = c;
    std::string color_name = " ";
    for (int i = 0; i < CurrentItem; i++)
    {
        color_name += " ";
    }
    ImGuiEx::ColorEdit4(color_name.c_str(), &c, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    if (c != orgc)
    {
        g_Config.Set(settings_name_color, c);
    }
    ImGui::SameLine();
    bool b = g_Config.GetBool(settings_name_bool);
    bool org = b;
    ImGui::Checkbox(text.c_str(), &b);
    if (b != org)
    {
        g_Config.Set(settings_name_bool, b);
    }
    CurrentItem++;
}

void MenuHelper::components::ColorCheckbox2(std::string text, std::string settings_name, std::string settings_name_color, std::string settings_name_color2)
{
    Color c = g_Config.GetColor(settings_name_color);
    Color c2 = g_Config.GetColor(settings_name_color2);
    Color orgc = c;
    Color orgc2 = c2;
    std::string color_name = " ";
    for (int i = 0; i < CurrentItem; i++)
    {
        color_name += " ";
    }
    std::string color_name2 = color_name + " ";
    ImGuiEx::ColorEdit4(color_name.c_str(), &c, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    if (c != orgc)
    {
        g_Config.Set(settings_name_color, c);
    }
    ImGui::SameLine();
    ImGuiEx::ColorEdit4(color_name2.c_str(), &c2, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    if (c2 != orgc2)
    {
        g_Config.Set(settings_name_color2, c2);
    }
    ImGui::SameLine();
    bool b = g_Config.GetBool(settings_name);
    bool org = b;
    ImGui::Checkbox(text.c_str(), &b);
    if (b != org)
    {
        g_Config.Set(settings_name, b);
    }
    CurrentItem++;
}

void MenuHelper::components::Label(std::string text)
{
    ImGui::Text(text.c_str());
    CurrentItem++;
}

void MenuHelper::components::ComboBox(std::string text, const char* items[], int size, std::string settings_name)
{
    int selected = g_Config.GetInt(settings_name);
    int orgs = selected;
    const char* itemSelected = items[selected];
    if (ImGui::BeginCombo(text.c_str(), itemSelected))
    {
        for (int n = 0; n < size; n++)
        {
            bool is_selected = (itemSelected == items[n]);
            if (ImGui::Selectable(items[n], is_selected))
            {
                itemSelected = items[n];
                selected = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    if (selected != orgs)
    {
        g_Config.Set(settings_name, selected);
    }
    CurrentItem++;
}

void MenuHelper::components::ComboCheckBox(std::string text, std::string items[], std::string setting_names[], int size)
{
    std::string t = " ";
    for (int i = 0; i < CurrentItem; i++)
    {
        t += " ";
    }
    if (ImGui::BeginCombo(t.c_str(), text.c_str()))
    {
        for (int i = 0; i < size; i++)
        {
            bool b = g_Config.GetBool(setting_names[i]);
            bool orgb = b;
            ImGui::Selectable(items[i].data(), &b);
            if (b != orgb)
            {
                g_Config.Set(setting_names[i], b);
            }
        }
        ImGui::EndCombo();
    }
    CurrentItem++;
}

void MenuHelper::components::BeginChild(std::string id, ImVec2 size, bool border, ImGuiWindowFlags flags)
{
    ImGui::BeginChild(id.c_str(), size, border, flags);
}

void MenuHelper::components::EndChild()
{
    ImGui::EndChild();
}

void MenuHelper::components::Columns(int count, bool border, char* id)
{
    InColumns = true;
    ColumnSize = count;
    ColumnCurrent = count;
    ImGui::Columns(count, id, border);
}

void MenuHelper::components::NextColumn()
{
    ColumnCurrent--;
    if (ColumnCurrent <= 0)
    {
        InColumns = false;
        ColumnSize = 0;
        ColumnCurrent = 0;
    }
    ImGui::NextColumn();
}

void MenuHelper::components::SameLine()
{
    ImGui::SameLine();
}

void MenuHelper::components::SliderInt(std::string text, std::string settings_name, int min, int max)
{
    int v = g_Config.GetInt(settings_name);
    int org_v = v;
    if (v > max)
    {
        v = max;
    }
    if (v < min)
    {
        v = min;
    }
    ImGui::SliderInt(text.c_str(), &v, min, max);
    if (v != org_v)
    {
        g_Config.Set(settings_name, v);
    }
}

void MenuHelper::components::SliderFloat(std::string text, std::string settings_name, float min, float max)
{
    float v = g_Config.GetFloat(settings_name);
    float org_v = v;
    if (v > max)
    {
        v = max;
    }
    if (v < min)
    {
        v = min;
    }
    ImGui::SliderFloat(text.c_str(), &v, min, max);
    if (v != org_v)
    {
        g_Config.Set(settings_name, v);
    }
    //ImGui::Hotkey()
}

void MenuHelper::components::Hotkey(std::string text, std::string settings_name)
{
    int key = g_Config.GetInt(settings_name);
    int orgkey = key;
    ImGui::Hotkey(text.data(), &key);
    if (key != orgkey)
    {
        g_Config.Set(settings_name, key);
    }
}

bool MenuHelper::components::Button(std::string text)
{
    return ImGui::Button(text.data());
}

void MenuHelper::components::Spacing()
{
    ImGui::Spacing();
}