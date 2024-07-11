#ifndef  CCA_HPP
#define CCA_HPP
#include <iostream>
#include <fstream>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <thread>
#include "MatrixComponent.hpp"
#include "InputComponent.hpp"
#include "ErrorBoxComponent.hpp"
#include "cl_handler.hpp"
#include "cfg.hpp"
#include "filter.hpp"
using CellType = float;

class CCA
{
public:
    CCA() : m_window(sf::VideoMode(constants::width, constants::height), "CCA"),
        m_view(sf::FloatRect(0, 0, constants::width, constants::height)),
        m_hostCells(constants::size), m_tempHostCells(constants::size), m_pixels(constants::size * 4), m_inputMat(3, 3)
    {
        m_sourceCode = std::move(ocl::loadKernelFromFile(constants::pathToKernel));
        m_window.setFramerateLimit(60);
        ImGui::SFML::Init(m_window);
        utils::randomize(m_hostCells, 0.0f, 1.0f);
        utils::copy(m_tempHostCells, m_hostCells);
        m_texture.create(constants::width, constants::height);
        m_sprite.setTexture(m_texture);
        m_filter = Filter{ {0, 1, 0,
                           1, 1, 1,
                           0, 1, 0},  3, 3 };
        m_deviceCells     = m_clHandler.requestMemory(m_hostCells, CL_MEM_READ_ONLY);
        m_deviceTempCells = m_clHandler.requestMemory(m_tempHostCells, CL_MEM_READ_WRITE);
        m_devicePixels    = m_clHandler.requestMemory(m_pixels, CL_MEM_READ_WRITE);
        m_deviceFilter    = m_clHandler.requestMemory(m_filter.mat, CL_MEM_READ_ONLY);
        m_deviceColor     = m_clHandler.requestMemory(m_color, CL_MEM_READ_ONLY);
        ocl::CompileResult result = std::move(compile(constants::defaultFunction));
        handleCompilation(result);
        auto io = ImGui::GetIO();
        io.Fonts->Clear();
        m_font  = io.Fonts->AddFontFromFileTTF(constants::pathToFont.c_str(), constants::fontSize);
        m_inputMat.from1D(m_filter.mat);
        ImGui::SFML::UpdateFontTexture();
        m_inputText.setText(constants::defaultFunction);
    }
    void run()
    {
        sf::Clock clock;
        while (m_window.isOpen())
        {
            float dt = clock.restart().asSeconds();
            events();
            update(dt);
            render();
        }
    }
private:
    void update(float dt)
    {
        ImGui::SFML::Update(m_window, sf::seconds(dt));
        if (canProcessEvent())
        {
            moveView(dt);
        }
        m_clHandler.requestWrite(m_deviceCells, m_hostCells);
        m_clHandler.requestWrite(m_deviceColor, m_color);
        m_clHandler.executeKernel(m_kernel, cl::NDRange(constants::height, constants::width));
        m_clHandler.requestRead(m_deviceTempCells, m_tempHostCells);
        m_clHandler.requestRead(m_devicePixels, m_pixels);
        utils::copy(m_hostCells, m_tempHostCells);
        m_texture.update(m_pixels.data());
        if (m_finishCompilation)
        {
            handleCompilation(m_result);
            m_finishCompilation    = false;
            m_hasClickedCompileBtn = false;
        }
    }
    void moveView(float dt)
    {
        float vel = 200;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            m_view.move(0, -dt * vel);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            m_view.move(0, dt * vel);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            m_view.move(-dt * vel, 0);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            m_view.move(dt * vel, 0);
        }
    }
    void events()
    {
        sf::Event ev;
        while (m_window.pollEvent(ev))
        {
            ImGui::SFML::ProcessEvent(m_window, ev);
            if (ev.type == sf::Event::Closed)
            {
                m_window.close();
                break;
            }
            if (canProcessEvent())
            {
                if (ev.type == sf::Event::MouseWheelMoved)
                {
                    m_view.zoom(1 - ev.mouseWheel.delta * 0.1);
                    m_window.setView(m_view);
                }
            }
        }
    }
    bool canProcessEvent()
    {
        return !ImGui::GetIO().WantCaptureKeyboard && !ImGui::GetIO().WantCaptureMouse;
    }
    void render()
    {
        renderImgui();
        m_window.clear(sf::Color::Black);
        m_window.setView(m_view);
        m_window.draw(m_sprite);
        m_window.setView(m_window.getDefaultView());
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
    void renderImgui()
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
                m_inputMat.from1D(m_filter.mat);
                m_clHandler.requestWrite(m_deviceFilter, m_filter.mat);
            }
            hs = vs = fs;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("HSymmetry", &hs))
        {
            if (hs)
            {
                m_filter.makeHSymmetric();
                m_inputMat.from1D(m_filter.mat);
                m_clHandler.requestWrite(m_deviceFilter, m_filter.mat);
            }
            fs = fs && !hs ? false : fs;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("VSymmetry", &vs) && vs)
        {
            if (vs)
            {
                m_filter.makeVSymmetric();
                m_inputMat.from1D(m_filter.mat);
                m_clHandler.requestWrite(m_deviceFilter, m_filter.mat);
            }
            fs = fs && !vs ? false : fs;
        }
        m_inputMat.render();
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
            m_inputMat.from1D(m_filter.mat);
            resetState();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        { 
            resetState();
        }
        ImGui::Text("Activation function");
        m_inputText.render();
        if (ImGui::Button("Compile") && !m_hasClickedCompileBtn)
        {
            m_hasClickedCompileBtn = true;
            std::string activationFunction(m_inputText.getRawText());
            m_finishCompilation = false;
            m_thread = std::thread(&CCA::compíleInThread, this, activationFunction);
            m_thread.detach();
        }
        m_error.render();
        ImGui::PopFont();
        ImGui::End();
    }
    void checkboxes()
    {

    }
    void resetState()
    {
        m_inputMat.convertTo1D(m_filter.mat);
        utils::randomize(m_hostCells, 0.0f, 1.0f);
        utils::copy(m_tempHostCells, m_hostCells);
        m_clHandler.requestWrite(m_deviceCells, m_hostCells);
        m_clHandler.requestWrite(m_deviceTempCells, m_tempHostCells);
        m_clHandler.requestWrite(m_deviceFilter, m_filter.mat);
    }
    void handleCompilation(ocl::CompileResult &result)
    {
        if (result.program == std::nullopt)
        {
            m_error.setActive(true);
            m_error.setText(result.error.c_str());
            return;
        }
        m_program = result.program.value();
        m_kernel = cl::Kernel(m_program, constants::kernelName.c_str());
        setKernelArgs();
        m_error.setActive(false);
    }
    void compíleInThread(const std::string& activationFunction)
    {
        m_result = compile(activationFunction);
        m_finishCompilation = true;
    }
    ocl::CompileResult compile(const std::string &activationFunction)
    {
        std::string fullCode = activationFunction + m_sourceCode;
        return ocl::buildProgram(m_clHandler.context, m_clHandler.device, fullCode);
    }
    void setKernelArgs()
    {
        m_kernel.setArg(0, m_deviceCells);
        m_kernel.setArg(1, m_deviceTempCells);
        m_kernel.setArg(2, m_devicePixels);
        m_kernel.setArg(3, m_deviceFilter);
        m_kernel.setArg(4, m_deviceColor);
        m_kernel.setArg(5, constants::height);
        m_kernel.setArg(6, constants::width);
    }
    sf::RenderWindow       m_window;
    sf::View               m_view;
    CLHandler              m_clHandler;

    cl::Buffer             m_devicePixels;
    cl::Buffer             m_deviceFilter;
    cl::Buffer             m_deviceCells;
    cl::Buffer             m_deviceTempCells;
    cl::Buffer	           m_deviceColor;
    cl::Program            m_program;
    cl::Kernel             m_kernel;

    sf::Texture            m_texture;
    sf::Sprite             m_sprite;
    std::vector<CellType>  m_hostCells;
    std::vector<CellType>  m_tempHostCells;
    std::vector<sf::Uint8> m_pixels;
    std::string            m_sourceCode;
    ocl::CompileResult     m_result;
    bool                   m_hasClickedCompileBtn = false;
    std::atomic<bool>      m_finishCompilation    = false;
    std::thread            m_thread;
    Filter                 m_filter;
    ImFont                *m_font;
    std::vector<float>     m_color = {1, 0, 0};
    MatrixComponent        m_inputMat;
    InputComponent         m_inputText;
    ErrorBoxComponent      m_error;
};
#endif // ! CCA_HPP
