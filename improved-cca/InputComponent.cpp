#include "InputComponent.h"
#include <imgui.h>
#include <cstdio>

void InputComponent::render()
{
    ImGui::InputTextMultiline("##Input", m_buffer, sizeof(m_buffer), ImVec2(width, height), ImGuiInputTextFlags_AllowTabInput);
}

void InputComponent::setText(const std::string& text)
{
    std::snprintf(m_buffer, sizeof(m_buffer), "%s", text.c_str());
}

void InputComponent::setText(const char* text)
{
    std::snprintf(m_buffer, sizeof(m_buffer), "%s", text);
}

const char* InputComponent::getRawText()
{
    return m_buffer;
}