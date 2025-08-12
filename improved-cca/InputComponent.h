#ifndef INPUT_COMPONENT_HPP
#define INPUT_COMPONENT_HPP

#include <string>

struct InputComponent
{
public:
    void render();
    void setText(const std::string& text);
    void setText(const char* text);
    const char* getRawText();

private:
    static const size_t maxSize = 512;
    static const int width = 512;
    static const int height = 128;
    char m_buffer[maxSize] = { 0 }; 
};

#endif // INPUT_COMPONENT_HPP