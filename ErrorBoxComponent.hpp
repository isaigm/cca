#ifndef ERROR_BOX_COMPONENT
#include <imgui.h>
#include <imgui-SFML.h>
struct ErrorBoxComponent
{
public:
    ErrorBoxComponent()
    {
        setText("An compilation error has occurred");
    }
    void render()
    {
        if (m_active)
        {
            ImGui::TextColored(ImColor(255, 0, 0), m_buffer);
        }
    }
    void setActive(bool active)
    {
        m_active = active;
    }
    void setText(const std::string& text)
    {
        std::snprintf(m_buffer, sizeof(m_buffer), "%s", text.c_str());
    }
private:
    bool m_active = false;
    static const size_t maxSize = 512;
    char m_buffer[maxSize];
};
#endif // !ERROR_BOX_COMPONENT