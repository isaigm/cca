#include "ErrorBoxComponent.h" 
#include <imgui.h> 
#include <cstdio> 

ErrorBoxComponent::ErrorBoxComponent()
    : m_active(false)
{
    setText("An compilation error has occurred");
}

void ErrorBoxComponent::render()
{
    if (m_active)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", m_buffer);
    }
}
void ErrorBoxComponent::setActive(bool active)
{
    m_active = active;
}
void ErrorBoxComponent::setText(const std::string& text)
{
    std::snprintf(m_buffer, sizeof(m_buffer), "%s", text.c_str());
}