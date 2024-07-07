#ifndef  CCA_HPP
#define CCA_HPP
#include <iostream>
#include <fstream>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include "cl_handler.hpp"
#include "cfg.hpp"
#include "filter.hpp"
using CellType = float;

class CCA
{
public:
	CCA() : m_window(sf::VideoMode(constants::WIDTH, constants::HEIGHT), ""),
		m_program(ocl::buildProgram(m_clHandler.context, m_clHandler.device,
			constants::pathToKernel)), m_kernel(m_program, constants::kernelName.c_str()),
		m_hostCells(constants::SIZE), m_tempHostCells(constants::SIZE), m_pixels(constants::SIZE * 4)
	{
		srand(time(nullptr));
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

		m_kernel.setArg(1, m_deviceTempCells);
		m_kernel.setArg(2, m_devicePixels);
		m_kernel.setArg(3, m_deviceFilter);
		m_kernel.setArg(4, constants::HEIGHT);
		m_kernel.setArg(5, constants::WIDTH);

		auto io = ImGui::GetIO();
		io.Fonts->Clear();
		m_font  = io.Fonts->AddFontFromFileTTF(constants::pathToFont.c_str(), 24);

		ImGui::SFML::UpdateFontTexture();

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
		m_kernel.setArg(0, m_deviceCells);
		m_clHandler.executeKernel(m_kernel, cl::NDRange(constants::HEIGHT, constants::WIDTH));
		m_clHandler.finishQueue();
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
		float firstRow[3] = { 0 };
		float secondRow[3] = { 0 };
		float thirdRow[3] = { 0 };
		char buff[1024] = { 0 };
		ImGui::Begin("Configuration");

		ImGui::PushFont(m_font);
		ImGui::Text("Kernel");
		ImGui::InputFloat3("", firstRow);
		ImGui::InputFloat3("", secondRow);
		ImGui::InputFloat3("", thirdRow);
		ImGui::Button("Randomize");
		ImGui::SameLine();
		ImGui::Button("Reset");
		ImGui::Text("Activation function");
		ImGui::InputTextMultiline("", buff, sizeof(buff), { 600, 256 }, ImGuiInputTextFlags_AllowTabInput);
		ImGui::PopFont();

		ImGui::End();
	}
	sf::RenderWindow m_window;
	CLHandler        m_clHandler;

	cl::Buffer  m_devicePixels;
	cl::Buffer  m_deviceFilter;
	cl::Buffer  m_deviceCells;
	cl::Buffer  m_deviceTempCells;
	cl::Program m_program;
	cl::Kernel  m_kernel;

	sf::Texture m_texture;
	sf::Sprite  m_sprite;

	std::vector<CellType>  m_hostCells;
	std::vector<CellType>  m_tempHostCells;
	std::vector<sf::Uint8> m_pixels;

	Filter m_filter;
	ImFont *m_font;
};
#endif // ! CCA_HPP
