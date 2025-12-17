#pragma once
#include <random>

namespace pez
{

class NumberGenerator
{
protected:
    std::mt19937 gen;

    NumberGenerator()
        : gen(0)
    {}

public:
    void setSeed(uint32_t const seed)
    {
        gen = std::mt19937{seed};
    }
};


template<typename T>
class RealNumberGenerator : public NumberGenerator
{
private:
    std::uniform_real_distribution<T> m_dis;

public:
    RealNumberGenerator()
        : m_dis(0.0f, 1.0f)
    {}

    // random_device is not copyable
    RealNumberGenerator(const RealNumberGenerator& right)
        : m_dis(right.m_dis)
    {}

    float get()
    {
        return m_dis(gen);
    }

    float getUnder(T max)
    {
        return get() * max;
    }

    float getRange(T min, T max)
    {
        return min + get() * (max - min);
    }

    float getRange(T width)
    {
        return getRange(-width, width) * 0.5f;
    }
};


template<typename T>
class RNG
{
private:
    static RealNumberGenerator<T> gen;

public:
    static T get()
    {
        return gen.get();
    }

    static float getUnder(T max)
    {
        return gen.getUnder(max);
    }

    static uint64_t getUintUnder(uint64_t const max)
    {
        return static_cast<uint64_t>(gen.getUnder(static_cast<float>(max) + 1.0f));
    }

    static float getRange(T min, T max)
    {
        return gen.getRange(min, max);
    }

    static float getRange(T width)
    {
        return gen.getRange(width);
    }

    static float getFullRange(T width)
    {
        return gen.getRange(static_cast<T>(2.0f) * width);
    }

    static bool proba(float threshold)
    {
        return get() < threshold;
    }

    static void setSeed(uint32_t seed)
    {
        gen.setSeed(seed);
    }

    template<typename TObject>
    static size_t pickIdx(std::vector<TObject>& container)
    {
        return static_cast<size_t>(getUnder(static_cast<T>(container.size())));
    }

    template<typename TObject>
    static TObject& pick(std::vector<TObject>& container)
    {
        return container[pickIdx(container)];
    }
};

using RNGf = RNG<float>;

template<typename T>
RealNumberGenerator<T> RNG<T>::gen = RealNumberGenerator<T>();


template<typename T>
class IntegerNumberGenerator : public NumberGenerator
{
public:
    IntegerNumberGenerator()
            : NumberGenerator()
    {}

    // random_device is not copyable
    IntegerNumberGenerator(const IntegerNumberGenerator<T>& right)
            : NumberGenerator()
    {}

    T getUnder(T max)
    {
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, max);
        return dist(gen);
    }

    T getRange(T min, T max)
    {
        std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
        return dist(gen);
    }
};


template<typename T>
class RNGi
{
private:
    static IntegerNumberGenerator<T> gen;

public:
    static T getUnder(T max)
    {
        return gen.getUnder(max);
    }

    static T getRange(T min, T max)
    {
        return gen.getRange(min, max);
    }
};

template<typename T>
IntegerNumberGenerator<T> RNGi<T>::gen;

using RNGi32 = RNGi<int32_t>;
using RNGi64 = RNGi<int64_t>;
using RNGu32 = RNGi<uint32_t>;
using RNGu64 = RNGi<uint64_t>;

}
