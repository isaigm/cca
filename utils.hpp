#ifndef UTILS_HPP
#define UTILS_HPP
#include <vector>
#include <random>
namespace utils
{
    static std::default_random_engine gen;

    template<class T>
    T random(T min, T max)
    {
        std::uniform_real_distribution<T> distribution(min, max);
        return distribution(gen);
    }
    template <class T>
    void randomize(std::vector<T>& vec, T min, T max)
    {
        for (size_t i = 0; i < vec.size(); i++)
        {
            vec[i] = random(min, max);
        }
    }
    template <class T>
    void copy(std::vector<T>& dst, std::vector<T>& src)
    {
        std::copy(src.begin(), src.end(), dst.begin());
    }
};
#endif // !UTILS_HPP
