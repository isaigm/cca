#include "Filter.h"
#include "utils.hpp" 
#include <cassert>

Filter::Filter() : m_rows(0), m_cols(0)
{
}

Filter::Filter(std::initializer_list<float> kernel, size_t rows, size_t cols)
    : mat(kernel), m_rows(rows), m_cols(cols)
{
    assert(kernel.size() == rows * cols && "Kernel size must match rows * cols");
}

void Filter::randomize()
{
    for (size_t i = 0; i < mat.size(); i++)
    {
        mat[i] = utils::random(-1.0f, 1.0f);
    }
}

void Filter::makeFullSymmetric()
{
    makeHSymmetric();
    makeVSymmetric();
}

void Filter::makeHSymmetric()
{
    auto makeCol = [&](size_t col)
        {
            size_t left = 0;
            size_t right = m_rows - 1;
            while (left < right)
            {
                mat[col + right * m_cols] = mat[col + left * m_cols];
                left++;
                right--;
            }
        };
    for (size_t i = 0; i < m_cols; i++)
    {
        makeCol(i);
    }
}

void Filter::makeVSymmetric()
{
    auto makeRow = [&](size_t row)
        {
            size_t left = 0;
            size_t right = m_cols - 1;
            while (left < right)
            {
                mat[right + row * m_cols] = mat[left + row * m_cols];
                left++;
                right--;
            }
        };
    for (size_t i = 0; i < m_rows; i++)
    {
        makeRow(i);
    }
}