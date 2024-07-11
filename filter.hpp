#ifndef FILTER_HPP
#define FILTER_HPP
#include "utils.hpp"
struct Filter
{
    Filter()
    {

    }
    Filter(std::initializer_list<float> kernel, size_t rows, size_t cols) : m_rows(rows), m_cols(cols)
    {
        assert(kernel.size() == rows * cols);
        for (auto x : kernel)
        {
            mat.push_back(x);
        }
    }
    void randomize()
    {
        for (size_t i = 0; i < mat.size(); i++)
        {
            mat[i] = utils::random(-1.0f, 1.0f);
        }
    }
    void makeFullSymmetric()
    {
        makeHSymmetric();
        makeVSymmetric();
    }
    void makeHSymmetric()
    {
        auto makeCol = [&](size_t col)
        { 
            size_t left  = 0;
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
    void makeVSymmetric()
    {
        auto makeRow = [&](size_t row)
        {
            size_t left  = 0;
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

    std::vector<float> mat;
private:

    size_t  m_rows{};
    size_t  m_cols{};
};
#endif // !FILTER_HPP
