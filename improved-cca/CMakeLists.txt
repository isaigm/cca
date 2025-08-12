cmake_minimum_required(VERSION 3.18)
project(ContinuousCellularAutomata LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/isaig/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

find_package(SFML 2.5.1 REQUIRED COMPONENTS system window graphics)
find_package(ImGui REQUIRED)
find_package(imgui-sfml REQUIRED)
find_package(OpenCL REQUIRED)
find_package(Threads REQUIRED)

include_directories(include)
add_executable(CCA_Executable
    main.cpp
    cca.cpp 
    cfg.cpp 
    Filter.cpp 
    CLHandler.cpp 
    ocl_utils.cpp
    ErrorBoxComponent.cpp 
    InputComponent.cpp 
    MatrixComponent.cpp 
    
)

set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(ASSETS_DIR "${PROJECT_ROOT_DIR}/assets")

function(path_to_string_literal input_path output_variable)
    string(REPLACE "\\" "\\\\" escaped_path "${input_path}")
    set(final_string "\"${escaped_path}\"")
    set(${output_variable} "${final_string}" PARENT_SCOPE)
endfunction()

path_to_string_literal("${ASSETS_DIR}" ASSETS_PATH_STR)

target_compile_definitions(CCA_Executable PRIVATE
    "ASSETS_PATH=${ASSETS_PATH_STR}"
)

target_link_libraries(CCA_Executable PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    imgui::imgui
    ImGui-SFML::ImGui-SFML
    OpenCL::OpenCL
    Threads::Threads
)

add_custom_command(TARGET CCA_Executable POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ASSETS_DIR}"
        $<TARGET_FILE_DIR:CCA_Executable>/assets
    COMMENT "Copying assets to build directory"
)