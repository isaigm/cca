#ifndef INPUT_COMPONENT_hPP
#define INPUT_COMPONENT
#include <imgui.h>
#include <imgui-SFML.h>
struct InputComponent
{
public:
    void render()
    {
        ImGui::InputTextMultiline("", m_buffer, sizeof(m_buffer), {width, height}, ImGuiInputTextFlags_AllowTabInput);
    }
    void setText(const std::string& text)
    {
        std::snprintf(m_buffer, sizeof(m_buffer), "%s", text.c_str());
    }
    void setText(const char* text)
    {
        std::snprintf(m_buffer, sizeof(m_buffer), "%s", text);
    }
    const char* getRawText()
    {
        return m_buffer;
    }
private:
    const static size_t maxSize = 512;
    const static int width      = 512;
    const static int height     = 128;
    char m_buffer[maxSize];
};

#endif // !INPUT_COMPONENT_hPP
