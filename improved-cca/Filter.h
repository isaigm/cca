#ifndef FILTER_HPP
#define FILTER_HPP

#include <vector>
#include <initializer_list>

struct Filter
{
    Filter(); 
    Filter(std::initializer_list<float> kernel, size_t rows, size_t cols);

    void randomize();
    void makeFullSymmetric();
    void makeHSymmetric();
    void makeVSymmetric();

    std::vector<float> mat;

private:
    size_t m_rows;
    size_t m_cols;
};

#endif // FILTER_HPP