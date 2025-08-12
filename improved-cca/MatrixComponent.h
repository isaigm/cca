#ifndef MATRIX_COMPONENT_HPP
#define MATRIX_COMPONENT_HPP

#include <vector>
#include <imgui.h>

struct MatrixComponent
{
public:
    MatrixComponent(size_t rows, size_t cols);

    void render();
    void convertTo1D(std::vector<float>& mat) const; 
    void from1D(const std::vector<float>& mat); 

private:
    ImGuiDataType m_type;
    std::vector<std::vector<float>> m_mat;
    size_t m_rows;
    size_t m_cols;
};

#endif // MATRIX_COMPONENT_HPP