#ifndef  MATRIX_COMPONENT_HPP
#define  MATRIX_COMPONENT_HPP
#include <imgui.h>
#include <imgui-SFML.h>
#include <vector>
struct MatrixComponent
{
public:
	MatrixComponent(size_t rows, size_t cols) : m_mat(rows), m_rows(rows), m_cols(cols)
	{
		for (size_t i = 0; i < rows; i++)
		{
			for (size_t j = 0; j < cols; j++)
			{
				m_mat[i].push_back(0);
			}
		}
	}
	void render()
	{
		for (size_t i = 0; i < m_rows; i++)
		{
			ImGui::PushID(i);
			ImGui::InputScalarN("", m_type, m_mat[i].data(), m_cols);
			ImGui::PopID();
		}
	}
	void convertTo1D(std::vector<float>& mat)
	{
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
	void from1D(std::vector<float>& mat)
	{
		for (size_t i = 0; i < m_rows; i++)
		{
			for (size_t j = 0; j < m_cols; j++)
			{
				m_mat[i][j] = mat[j + i * m_cols];
			}
		}
	}
private:
	ImGuiDataType m_type = ImGuiDataType_Float;
	std::vector<std::vector<float>> m_mat;
	size_t m_rows;
	size_t m_cols;
};
#endif // ! MATRIX_COMPONENT_HPP
