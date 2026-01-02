#include "cca.h"
#include "cfg.h"
#include "utils.hpp"
#include "MatrixComponent.h"
#include "InputComponent.h"
#include "ErrorBoxComponent.h"
#include <imgui.h>
#include <imgui-SFML.h>


CCA::CCA()
    : m_window(sf::VideoMode(constants::width, constants::height), "Continuous Cellular Automata"),
    m_view(sf::FloatRect(0, 0, constants::width, constants::height)),
    m_hostCells(constants::size),
    m_tempHostCells(constants::size),
    m_pixels(constants::size * 4),
    m_color({ 1.0f, 0.0f, 0.0f }),
    m_fs_checkbox(false), m_hs_checkbox(false), m_vs_checkbox(false),
    m_isCompiling(false),
    m_compilationFinished(false)
{
    constants::initializePaths();
    m_window.setFramerateLimit(60);
    ImGui::SFML::Init(m_window);

    m_inputMat = std::make_unique<MatrixComponent>(3, 3);
    m_inputText = std::make_unique<InputComponent>();
    m_error = std::make_unique<ErrorBoxComponent>();

    setupSimulation();

    auto& io = ImGui::GetIO();
    io.Fonts->Clear();
    m_font = io.Fonts->AddFontFromFileTTF(constants::pathToFont.c_str(), constants::fontSize);
    ImGui::SFML::UpdateFontTexture();
}

CCA::~CCA()
{
    if (m_compilationThread.joinable()) {
        m_compilationThread.join();
    }
    ImGui::SFML::Shutdown();
}


void CCA::run()
{
    sf::Clock deltaClock;
    while (m_window.isOpen())
    {
        processEvents();
        update(deltaClock.restart());
        render();
    }
}

void CCA::processEvents()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(m_window, event);
        if (event.type == sf::Event::Closed)
        {
            m_window.close();
        }
        if (canProcessInput() && event.type == sf::Event::MouseWheelMoved)
        {
            m_view.zoom(1.0f - event.mouseWheel.delta * 0.1f);
        }
    }
}

void CCA::update(const sf::Time& dt)
{
    ImGui::SFML::Update(m_window, dt);

    renderImgui();

    if (canProcessInput()) {
        sf::Vector2f moveDir;
        float vel = 200.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveDir.y -= 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveDir.y += 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveDir.x -= 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveDir.x += 1;
        m_view.move(moveDir * vel * dt.asSeconds());
    }

    runSimulationStep();
    checkKernelCompilationStatus();
}

void CCA::render()
{
    m_window.clear(sf::Color::Black);

    m_window.setView(m_view);
    m_window.draw(m_sprite);

    m_window.setView(m_window.getDefaultView());
    ImGui::SFML::Render(m_window);

    m_window.display();
}


void CCA::renderImgui()
{
    ImGui::Begin("Configuration");
    ImGui::PushFont(m_font);
    ImGui::ColorPicker3("", m_color.data());
    ImGui::Text("Kernel");
    static bool fs = false;
    static bool hs = false;
    static bool vs = false;
    
    if (ImGui::Checkbox("FullSymmetry", &fs))
    {
        if (fs)
        {
            m_filter.makeFullSymmetric();
            m_inputMat->from1D(m_filter.mat);
            m_clHandler.writeBuffer(m_deviceFilter, m_filter.mat);
        }
        hs = vs = fs;
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("HSymmetry", &hs))
    {
        if (hs)
        {
            m_filter.makeHSymmetric();
            m_inputMat->from1D(m_filter.mat);
            m_clHandler.writeBuffer(m_deviceFilter, m_filter.mat);
        }
        fs = fs && !hs ? false : fs;
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("VSymmetry", &vs) && vs)
    {
        if (vs)
        {
            m_filter.makeVSymmetric();
            m_inputMat->from1D(m_filter.mat);
            m_clHandler.writeBuffer(m_deviceFilter, m_filter.mat);
        }
        fs = fs && !vs ? false : fs;
    }
    m_inputMat->render();
    if (ImGui::Button("Randomize"))
    {
        m_filter.randomize();
        if (fs || (hs && vs))
        {
            m_filter.makeFullSymmetric();
        }
        else if (hs)
        {
            m_filter.makeHSymmetric();
        }
        else if (vs)
        {
            m_filter.makeVSymmetric();
        }
        m_inputMat->from1D(m_filter.mat);
        resetSimulationState();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        resetSimulationState();
    }

    ImGui::Text("Activation Function (OpenCL C)");
    m_inputText->render();

    if (m_isCompiling) {
        ImGui::Text("Compiling...");
    }
    else {
        if (ImGui::Button("Compile Kernel")) {
            startKernelCompilation();
        }
    }

    m_error->render();

    ImGui::PopFont();
    ImGui::End();
}


void CCA::setupSimulation()
{
    m_kernelSourceCode = ocl::loadKernelFromFile(constants::pathToKernel);
    m_texture.create(constants::width, constants::height);
    m_sprite.setTexture(m_texture);

    m_filter = Filter({ 0, 1, 0, 1, 1, 1, 0, 1, 0 }, 3, 3);
    m_inputMat->from1D(m_filter.mat);
    m_inputText->setText(constants::defaultFunction.c_str());

    resetSimulationState();

    std::string fullInitialCode = constants::defaultFunction + m_kernelSourceCode;
    auto result = ocl::buildProgram(m_clHandler.context, m_clHandler.device, fullInitialCode);
    applyNewKernel(result);
}

void CCA::runSimulationStep()
{
    m_clHandler.writeBuffer(m_deviceCells, m_hostCells);
    m_clHandler.writeBuffer(m_deviceColor, m_color);
    m_clHandler.executeKernel(m_kernel, cl::NDRange(constants::height, constants::width));
    m_clHandler.readBuffer(m_deviceTempCells, m_tempHostCells);
    m_clHandler.readBuffer(m_devicePixels, m_pixels);
    m_clHandler.finishQueue();
    utils::copy(m_hostCells, m_tempHostCells);
    m_texture.update(m_pixels.data());
}

void CCA::resetSimulationState()
{
    utils::randomize(m_hostCells, 0.0f, 1.0f);
    utils::copy(m_tempHostCells, m_hostCells);
    m_inputMat->convertTo1D(m_filter.mat);

    m_deviceCells = m_clHandler.createBuffer(m_hostCells, CL_MEM_READ_WRITE);
    m_deviceTempCells = m_clHandler.createBuffer(m_tempHostCells, CL_MEM_READ_WRITE);
    m_devicePixels = m_clHandler.createBuffer<sf::Uint8>(constants::size * 4, CL_MEM_WRITE_ONLY);
    m_deviceFilter = m_clHandler.createBuffer(m_filter.mat, CL_MEM_READ_ONLY);
    m_deviceColor = m_clHandler.createBuffer(m_color, CL_MEM_READ_ONLY);

    if (m_kernel()) {
        m_kernel.setArg(0, m_deviceCells);
        m_kernel.setArg(1, m_deviceTempCells);
        m_kernel.setArg(2, m_devicePixels);
        m_kernel.setArg(3, m_deviceFilter);
        m_kernel.setArg(4, m_deviceColor);
        m_kernel.setArg(5, constants::height);
        m_kernel.setArg(6, constants::width);
    }
}

void CCA::startKernelCompilation()
{
    if (m_compilationThread.joinable()) {
        m_compilationThread.join();
    }
    m_isCompiling = true;
    m_compilationFinished = false;
    m_error->setActive(false);

    std::string activationFunction = m_inputText->getRawText();

    m_compilationThread = std::thread(&CCA::compileKernelInThread, this, activationFunction);
}

void CCA::checkKernelCompilationStatus()
{
    if (m_compilationFinished)
    {
        if (m_compilationThread.joinable()) {
            m_compilationThread.join();
        }

        if (m_compilationResult.has_value()) {
            applyNewKernel(m_compilationResult.value());
        }

        m_isCompiling = false;
        m_compilationFinished = false;
        m_compilationResult.reset();
    }
}

void CCA::compileKernelInThread(std::string activationFunction)
{
    std::string fullCode = activationFunction + m_kernelSourceCode;
    m_compilationResult = ocl::buildProgram(m_clHandler.context, m_clHandler.device, fullCode);
    m_compilationFinished = true;
}

void CCA::applyNewKernel(ocl::CompileResult& result)
{
    if (!result.program.has_value())
    {
        m_error->setActive(true);
        m_error->setText(result.error);
        return;
    }

    m_program = result.program.value();
    m_kernel = cl::Kernel(m_program, constants::kernelName.c_str());

    m_kernel.setArg(0, m_deviceCells);
    m_kernel.setArg(1, m_deviceTempCells);
    m_kernel.setArg(2, m_devicePixels);
    m_kernel.setArg(3, m_deviceFilter);
    m_kernel.setArg(4, m_deviceColor);
    m_kernel.setArg(5, constants::height);
    m_kernel.setArg(6, constants::width);

    m_error->setActive(false);
}

bool CCA::canProcessInput() {
    auto& io = ImGui::GetIO();
    return !io.WantCaptureKeyboard && !io.WantCaptureMouse;
}
