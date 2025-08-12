#include "MatrixComponent.h"
#include <imgui.h>
#include <stdexcept> 

MatrixComponent::MatrixComponent(size_t rows, size_t cols)
    : m_type(ImGuiDataType_Float),
    m_mat(rows, std::vector<float>(cols, 0.0f)), 
    m_rows(rows),
    m_cols(cols)
{
    if (rows == 0 || cols == 0) {
        // Opcional: manejar el caso de una matriz vacía si es necesario
    }
}

void MatrixComponent::render()
{
    for (size_t i = 0; i < m_rows; i++)
    {
        ImGui::PushID(static_cast<int>(i));
        ImGui::InputScalarN("##row", m_type, m_mat[i].data(), static_cast<int>(m_cols));
        ImGui::PopID();
    }
}

void MatrixComponent::convertTo1D(std::vector<float>& mat) const
{
    if (mat.size() != m_rows * m_cols) {
        mat.resize(m_rows * m_cols);
    }

    size_t k = 0;
    for (size_t i = 0; i < m_rows; i++)
    {
        for (size_t j = 0; j < m_cols; j++)
        {
            mat[k] = m_mat[i][j];
            k++;
        }
    }
}

void MatrixComponent::from1D(const std::vector<float>& mat)
{
    if (mat.size() != m_rows * m_cols) {
        return;
    }

    for (size_t i = 0; i < m_rows; i++)
    {
        for (size_t j = 0; j < m_cols; j++)
        {
            m_mat[i][j] = mat[j + i * m_cols];
        }
    }
}