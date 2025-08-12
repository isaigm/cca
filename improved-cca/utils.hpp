#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>

namespace utils
{
    template<typename T>
    T random(T min, T max);

    template <typename T>
    void randomize(std::vector<T>& vec, T min, T max);

    template <typename T>
    void copy(std::vector<T>& dst, const std::vector<T>& src);
}



#include <random>
#include <algorithm>

namespace utils
{
    inline std::mt19937& get_random_engine()
    {
        static std::mt19937 gen{ std::random_device{}() };
        return gen;
    }

    template<typename T>
    T random(T min, T max)
    {
        if constexpr (std::is_floating_point_v<T>) {
            std::uniform_real_distribution<T> distribution(min, max);
            return distribution(get_random_engine());
        }
        else if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> distribution(min, max);
            return distribution(get_random_engine());
        }
    }

    template <typename T>
    void randomize(std::vector<T>& vec, T min, T max)
    {
        for (size_t i = 0; i < vec.size(); i++)
        {
            vec[i] = random(min, max);
        }
    }

    template <typename T>
    void copy(std::vector<T>& dst, const std::vector<T>& src)
    {
        if (dst.size() != src.size()) {
            dst.resize(src.size());
        }
        std::copy(src.begin(), src.end(), dst.begin());
    }
}

#endif // UTILS_HPP