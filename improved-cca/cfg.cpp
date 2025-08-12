#include "cfg.h"
#include <filesystem>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace constants
{
    std::string kernelName = "perform_step";

    std::string defaultFunction = "float gaussian(float x, float b) {\n"
        "return 1.0f / pow(2.0f, (pow(x - b, 2.0f)));\n"
        "}\n"

        "float activation(float x) {\n"
        "return gaussian(x, 3.5f);\n"
        "}\n";
    std::string pathToFont;
    std::string pathToKernel; 

    std::string getExecutablePath() {
#ifdef _WIN32
        WCHAR path[MAX_PATH] = { 0 };
        GetModuleFileNameW(NULL, path, MAX_PATH);
        return std::filesystem::path(path).string();
#else
        return "";
#endif
    }

    void initializePaths() {
        std::filesystem::path exePath(getExecutablePath());
        std::filesystem::path basePath = exePath.parent_path();

        pathToFont = (basePath / "assets" / "ProggyClean.ttf").string();

        pathToKernel = (basePath / "assets" / "perform_step.cl").string();
    }
}