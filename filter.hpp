#ifndef FILTER_HPP
#define FILTER_HPP
#include "utils.hpp"
struct Filter
{
    Filter()
    {

    }
    Filter(std::initializer_list<float> kernel)
    {
        for (auto x : kernel)
        {
            mat.push_back(x);
        }
    }
    void randomize()
    {
        float r1 = utils::random(-1.0f, 1.0f);
        float r2 = utils::random(-1.0f, 1.0f);
        float r3 = utils::random(-1.0f, 1.0f);
        mat[0] = r1;
        mat[2] = r1;
        mat[6] = r1;
        mat[8] = r1;

        mat[4] = r2;

        mat[1] = r3;
        mat[5] = r3;
        mat[7] = r3;
        mat[3] = r3;
    }
    std::vector<float> mat;
};
#endif // !FILTER_HPP
