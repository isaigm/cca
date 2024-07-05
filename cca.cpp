#include <stdio.h>
#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <chrono>

#define COLS 1920
#define ROWS 1080
using CellType = float;
std::string loadKernelFromFile(std::string path)
{
    std::fstream file(path);
    if (file.is_open())
    {
        std::string content(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        return content;
    }
    throw std::exception("error at opening file");
}
cl::Program buildProgram(cl::Context &context, cl::Device &device, std::string path)
{
    cl::Program::Sources sources;
    std::string sourceCode = loadKernelFromFile(path);
    sources.push_back({ sourceCode.c_str(), sourceCode.length() });
    cl::Program program(context, sources);
    if (program.build({ device }) != CL_SUCCESS)
    {
        std::cout << "error:" << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
        throw std::exception("Compiling error");
    }
    return program;
}
float random(float min, float max)
{
    float r = float(rand()) / float(RAND_MAX);
    return r * (max - min) + min;
}
void randomize(CellType* cells)
{
    for (int i = 0; i < ROWS * COLS; i++)
    {
        cells[i] = random(0.0f, 1.0f);
    }
}
int main()
{
    srand(time(nullptr));
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.size() == 0)
    {
        std::cout << "No OpenCL platform found\n";
        return 1;
    }

    cl::Platform default_platform = platforms[0];
    std::cout << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

    std::vector<cl::Device> devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    if (devices.size() == 0)
    {
        std::cout << "No devices found\n";
        return 1;
    }
    float kernel[9] = {0,1, 0,
                      1, 1, 1,
                      0, 1, 0 };
    cl::Device device = devices[0];

    std::cout << device.getInfo<CL_DEVICE_NAME>() << "\n";
    cl::Context context({ device });
    cl::CommandQueue queue(context, device);


    auto psProgram = buildProgram(context, device,
        "C:\\Users\\isaig\\examples\\opencl\\perform_step.cl");

    unsigned int rows = ROWS;
    unsigned int cols = COLS;
    size_t sizeInBytes = ROWS * COLS * sizeof(CellType);
    CellType* hostCells;
    CellType* tempHostCells;
    hostCells     = new CellType[rows * cols];
    tempHostCells = new CellType[rows * cols];
    randomize(hostCells);
    std::memcpy(tempHostCells, hostCells, sizeInBytes);
    cl::Buffer deviceCells(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeInBytes, hostCells);
    cl::Buffer deviceTempCells(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeInBytes, hostCells);
    cl::Kernel psKernel(psProgram, "perform_step");
    sf::Texture texture;
    sf::Sprite sprite;
    texture.create(COLS, ROWS);
    sprite.setTexture(texture);
    sf::Uint8* pixels = new sf::Uint8[ROWS * COLS * 4];
    texture.update(pixels);

    cl::Buffer devicePixels(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 4 * sizeof(sf::Uint8) * rows * cols, pixels);
    cl::Buffer deviceKernel(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(kernel), kernel);
    psKernel.setArg(1, deviceTempCells);    
    psKernel.setArg(2, devicePixels);
    psKernel.setArg(3, deviceKernel);
    psKernel.setArg(4, rows);
    psKernel.setArg(5, cols);
    sf::RenderWindow window(sf::VideoMode(COLS, ROWS), "Continous CA");
    window.setFramerateLimit(60);
    sf::View view(sf::FloatRect(0, 0, COLS, ROWS));
    sf::Clock clock;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
                window.close();
            if (ev.type == sf::Event::MouseWheelMoved)
            {
                view.zoom(1 - ev.mouseWheel.delta * 0.1);
                window.setView(view);
            }
            if (ev.type == sf::Event::KeyPressed)
            {
                if (ev.key.code == sf::Keyboard::R)
                {
                    float r1 = random(-1.0f, 1.0f);
                    float r2 = random(-1.0f, 1.0f);
                    float r3 = random(-1.0f, 1.0f);
                    kernel[0] = r1;
                    kernel[2] = r1;
                    kernel[6] = r1;
                    kernel[8] = r1;
                    
                    kernel[4] = r2;
                    
                    kernel[1] = r3;
                    kernel[5] = r3;
                    kernel[7] = r3;
                    kernel[3] = r3;
                    randomize(hostCells);
                    std::memcpy(tempHostCells, hostCells, sizeInBytes);
                    
                    queue.enqueueWriteBuffer(deviceCells, CL_TRUE, 0, sizeInBytes, hostCells);
                    queue.enqueueWriteBuffer(deviceTempCells, CL_TRUE, 0, sizeInBytes, tempHostCells);
                    queue.enqueueWriteBuffer(deviceKernel, CL_TRUE, 0, sizeof(kernel), kernel);

                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            view.move(0, -dt * 200);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            view.move(0, dt * 200);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            view.move(-dt * 200, 0);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            view.move(dt * 200, 0);
        }

        queue.enqueueWriteBuffer(deviceCells, CL_TRUE, 0, sizeInBytes, hostCells);
        psKernel.setArg(0, deviceCells);
        queue.enqueueNDRangeKernel(psKernel, cl::NullRange, cl::NDRange(rows, cols));
        queue.finish();
        queue.enqueueReadBuffer(deviceTempCells, CL_TRUE, 0, sizeInBytes, tempHostCells);
        queue.enqueueReadBuffer(devicePixels, CL_TRUE, 0, sizeof(sf::Uint8) * rows * cols * 4, pixels);
        std::memcpy(hostCells, tempHostCells, sizeInBytes);

        texture.update(pixels);
        window.clear(sf::Color::Black);
        window.setView(view);
        window.draw(sprite);
        window.setView(window.getDefaultView());
        window.display();
    }
    delete[] hostCells;
    delete[] tempHostCells;
    delete[] pixels;
    return 0;
}
