#ifndef CCA_HPP
#define CCA_HPP

#include <SFML/Graphics.hpp>
#include <atomic>
#include <thread>
#include <vector>
#include <memory> 

#include "CLHandler.h"
#include "ocl_utils.h"
#include "filter.h"

struct MatrixComponent;
struct InputComponent;
struct ErrorBoxComponent;
struct ImFont; 

using CellType = float;

class CCA
{
public:
    CCA();
    ~CCA();
    void run();

private:
    void processEvents();
    void update(const sf::Time& dt);
    void render();

    void renderImgui();
    void handleGuiActions(); 
    bool canProcessInput();

    void setupSimulation();
    void runSimulationStep();
    void resetSimulationState();

    void startKernelCompilation();
    void checkKernelCompilationStatus();
    void applyNewKernel(ocl::CompileResult& result);
    void compileKernelInThread(std::string activationFunction);


    sf::RenderWindow m_window;
    sf::View m_view;

    CLHandler m_clHandler;
    cl::Program m_program;
    cl::Kernel m_kernel;
    cl::Buffer m_devicePixels, m_deviceFilter, m_deviceCells, m_deviceTempCells, m_deviceColor;

    sf::Texture m_texture;
    sf::Sprite m_sprite;
    std::vector<CellType> m_hostCells;
    std::vector<CellType> m_tempHostCells;
    std::vector<sf::Uint8> m_pixels;
    Filter m_filter;
    std::vector<float> m_color;
    std::string m_kernelSourceCode;

    ImFont* m_font;
    std::unique_ptr<MatrixComponent> m_inputMat;
    std::unique_ptr<InputComponent> m_inputText;
    std::unique_ptr<ErrorBoxComponent> m_error;
    bool m_fs_checkbox = false, m_hs_checkbox = false, m_vs_checkbox = false;

    std::thread m_compilationThread;
    std::atomic<bool> m_isCompiling; 
    std::atomic<bool> m_compilationFinished;
    std::optional<ocl::CompileResult> m_compilationResult;
};

#endif // CCA_HPP