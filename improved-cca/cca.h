#ifndef CCA_HPP
#define CCA_HPP

#include <SFML/Graphics.hpp>
#include <atomic>
#include <thread>
#include <vector>
#include <memory> // Para std::unique_ptr

#include "CLHandler.h"
#include "ocl_utils.h"
#include "filter.h"

// Declaraciones adelantadas para nuestros componentes de UI
struct MatrixComponent;
struct InputComponent;
struct ErrorBoxComponent;
struct ImFont; // ImGui usa punteros a structs opacos

using CellType = float;

class CCA
{
public:
    CCA();
    ~CCA();
    void run();

private:
    // --- Métodos del Bucle Principal ---
    void processEvents();
    void update(const sf::Time& dt);
    void render();

    // --- Métodos de la Interfaz Gráfica (GUI) ---
    void renderImgui();
    void handleGuiActions(); // Lógica de los botones y checkboxes
    bool canProcessInput();

    // --- Métodos de Simulación y OpenCL ---
    void setupSimulation();
    void runSimulationStep();
    void resetSimulationState();

    // --- Métodos de Compilación del Kernel (Thread-Safe) ---
    void startKernelCompilation();
    void checkKernelCompilationStatus();
    void applyNewKernel(ocl::CompileResult& result);
    void compileKernelInThread(std::string activationFunction);

    // --- Miembros de Datos ---

    // Ventana y Vista
    sf::RenderWindow m_window;
    sf::View m_view;

    // Motor de OpenCL y sus objetos
    CLHandler m_clHandler;
    cl::Program m_program;
    cl::Kernel m_kernel;
    cl::Buffer m_devicePixels, m_deviceFilter, m_deviceCells, m_deviceTempCells, m_deviceColor;

    // Estado de la Simulación en el Host (CPU)
    sf::Texture m_texture;
    sf::Sprite m_sprite;
    std::vector<CellType> m_hostCells;
    std::vector<CellType> m_tempHostCells;
    std::vector<sf::Uint8> m_pixels;
    Filter m_filter;
    std::vector<float> m_color;
    std::string m_kernelSourceCode;

    // Estado y Componentes de la Interfaz Gráfica
    ImFont* m_font;
    std::unique_ptr<MatrixComponent> m_inputMat;
    std::unique_ptr<InputComponent> m_inputText;
    std::unique_ptr<ErrorBoxComponent> m_error;
    // Variables para controlar el estado de la UI
    bool m_fs_checkbox = false, m_hs_checkbox = false, m_vs_checkbox = false;

    // --- Gestión de Hilos (Thread-Safe) ---
    std::thread m_compilationThread;
    std::atomic<bool> m_isCompiling; // Para saber si un hilo está activo
    // Usaremos un par para pasar el resultado del hilo al hilo principal de forma segura
    std::atomic<bool> m_compilationFinished;
    std::optional<ocl::CompileResult> m_compilationResult;
};

#endif // CCA_HPP