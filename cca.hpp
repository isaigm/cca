#ifndef  CCA_HPP
#define CCA_HPP
#include <iostream>
#include <fstream>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <regex>
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
	CCA() : m_window(sf::VideoMode(constants::WIDTH, constants::HEIGHT), ""),
		m_hostCells(constants::SIZE), m_tempHostCells(constants::SIZE), m_pixels(constants::SIZE * 4), m_inputMat(3, 3)
	{
		srand(time(nullptr));
		m_sourceCode = std::move(ocl::loadKernelFromFile(constants::pathToKernel));
		compile(constants::defaultFunction);

		m_window.setFramerateLimit(60);
		ImGui::SFML::Init(m_window);
		
		utils::randomize(m_hostCells, 0.0f, 1.0f);
		utils::copy(m_tempHostCells, m_hostCells);
		m_texture.create(constants::WIDTH, constants::HEIGHT);
		m_sprite.setTexture(m_texture);
		m_filter = Filter{ 0, 1, 0,
						   1, 1, 1,
						   0, 1, 0};
		m_deviceCells     = m_clHandler.requestMemory(m_hostCells, CL_MEM_READ_ONLY);
		m_deviceTempCells = m_clHandler.requestMemory(m_tempHostCells, CL_MEM_READ_WRITE);
		m_devicePixels    = m_clHandler.requestMemory(m_pixels, CL_MEM_READ_WRITE);
		m_deviceFilter    = m_clHandler.requestMemory(m_filter.mat, CL_MEM_READ_ONLY);
		m_deviceColor     = m_clHandler.requestMemory(m_color, CL_MEM_READ_ONLY);

		setKernelArgs();

		auto io = ImGui::GetIO();
		io.Fonts->Clear();
		m_font  = io.Fonts->AddFontFromFileTTF(constants::pathToFont.c_str(), 22);

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
		m_clHandler.requestWrite(m_deviceCells, m_hostCells);
		m_clHandler.requestWrite(m_deviceColor, m_color);

		m_clHandler.executeKernel(m_kernel, cl::NDRange(constants::HEIGHT, constants::WIDTH));
		m_clHandler.requestRead(m_deviceTempCells, m_tempHostCells);
		m_clHandler.requestRead(m_devicePixels, m_pixels);
		utils::copy(m_hostCells, m_tempHostCells);
		m_texture.update(m_pixels.data());
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
		}
	}
	void render()
	{
		renderImgui();
		m_window.clear(sf::Color::Black);
		m_window.draw(m_sprite);
		ImGui::SFML::Render(m_window);
		m_window.display();
	}
	void renderImgui()
	{

		ImGui::Begin("Configuration");
		ImGui::PushFont(m_font);
		ImGui::ColorPicker3("", m_color.data());
		ImGui::Text("Kernel");
		m_inputMat.render();
		if (ImGui::Button("Randomize"))
		{
			m_filter.randomize();
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
		if (ImGui::Button("Compile"))
		{
			try
			{
				std::string activationFunction(m_inputText.getRawText());
				compile(activationFunction);
				setKernelArgs();
				m_error.setActive(false);
			}
			catch (std::exception e)
			{
				m_error.setActive(true);
				m_error.setText(e.what());
			}
		}
		m_error.render();
		ImGui::PopFont();

		ImGui::End();
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
	void compile(const std::string &activationFunction)
	{
		std::string fullCode = std::move(std::regex_replace(m_sourceCode, std::regex(constants::templateStr), activationFunction));
		m_program = ocl::buildProgram(m_clHandler.context, m_clHandler.device, fullCode);
		m_kernel = cl::Kernel(m_program, constants::kernelName.c_str());
	}
	void setKernelArgs()
	{
		m_kernel.setArg(0, m_deviceCells);
		m_kernel.setArg(1, m_deviceTempCells);
		m_kernel.setArg(2, m_devicePixels);
		m_kernel.setArg(3, m_deviceFilter);
		m_kernel.setArg(4, m_deviceColor);
		m_kernel.setArg(5, constants::HEIGHT);
		m_kernel.setArg(6, constants::WIDTH);
	}
	sf::RenderWindow m_window;
	CLHandler        m_clHandler;

	cl::Buffer  m_devicePixels;
	cl::Buffer  m_deviceFilter;
	cl::Buffer  m_deviceCells;
	cl::Buffer  m_deviceTempCells;
	cl::Buffer	m_deviceColor;
	cl::Program m_program;
	cl::Kernel  m_kernel;

	sf::Texture m_texture;
	sf::Sprite  m_sprite;

	std::vector<CellType>  m_hostCells;
	std::vector<CellType>  m_tempHostCells;
	std::vector<sf::Uint8> m_pixels;
	std::string m_sourceCode;
	Filter m_filter;
	ImFont *m_font;
	std::vector<float> m_color = {1, 0, 0};
	MatrixComponent m_inputMat;
	InputComponent m_inputText;
	ErrorBoxComponent m_error;
};
#endif // ! CCA_HPP
