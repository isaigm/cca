#ifndef CFG_HPP
#define CFG_HPP
#include <string>

namespace constants
{
    constexpr int width = 1500;
    constexpr int height = 720;
    constexpr size_t size = width * height;
    constexpr int fontSize = 18;
    extern std::string kernelName;
    extern std::string defaultFunction;
    extern std::string pathToFont;
    extern std::string pathToKernel;
    void initializePaths();
}
#endif // CFG_HPP