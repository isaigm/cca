#ifndef ERROR_BOX_COMPONENT_HPP 
#define ERROR_BOX_COMPONENT_HPP
#include <string> 

struct ErrorBoxComponent
{
public:
    ErrorBoxComponent();
    void render();
    void setActive(bool active);
    void setText(const std::string& text);

private:
    bool m_active;
    static const size_t maxSize = 512;
    char m_buffer[maxSize];
};

#endif // ERROR_BOX_COMPONENT_HPP