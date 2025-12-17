#pragma once
#include <numeric>
#include <vector>


template<typename TValue>
struct IRunningAccumulator
{
    virtual ~IRunningAccumulator() = default;

    virtual bool add(TValue val) = 0;
    virtual void clear() = 0;
    virtual void setCapacity(size_t capacity) = 0;
    virtual TValue get() const = 0;
    virtual size_t getCount() const = 0;
    virtual bool isOverflowing() const = 0;

    operator TValue() const
    {
        return get();
    }
};


template<typename T>
struct RAccBase : IRunningAccumulator<T>
{
    size_t         max_values_count;
    std::vector<T> values;
    size_t         current_index;
    T              pop_value;

    explicit
    RAccBase(uint32_t max_size=8)
        : max_values_count(max_size)
        , values(max_size)
        , current_index(0)
        , pop_value{}
    {}

    ~RAccBase() override = default;

    bool add(T const val) override
    {
        const bool   pop = current_index >= max_values_count;
        const size_t i   = current_index % max_values_count;
        pop_value = values[i];
        values[i] = val;
        ++current_index;
        return pop;
    }

    void clear() override
    {
        current_index = 0;
    }

    [[nodiscard]]
    size_t getCount() const override
    {
        return std::min(current_index, max_values_count);
    }

    [[nodiscard]]
    bool isOverflowing() const override
    {
        return current_index >= max_values_count;
    }

    T get() const override
    {
        return values[getIndex(-1)];
    }

    [[nodiscard]]
    size_t getIndex(int32_t const offset = 0) const
    {
        size_t const count = getCount();
        if (count) {
            return (current_index + offset) % count;
        }
        return 0;
    }

    [[nodiscard]]
    T getValue(int32_t const offset) const
    {
        return values[getIndex(offset)];
    }

    template<typename TCallback>
    void foreach(TCallback&& callback)
    {
        uint32_t const count = getCount();
        for (uint32_t i{0}; i < count; ++i) {
            callback(i, values[getIndex(i)]);
        }
    }

    void setCapacity(size_t const capacity) override
    {
        max_values_count = capacity;
        values.resize(capacity);
    }
};

template<typename T>
struct RSum : public RAccBase<T>
{
    float sum{0.0f};

    explicit
    RSum(size_t const max_size = 8)
        : RAccBase<T>(max_size)
    {}

    bool add(T const v) override
    {
        sum += v;
        bool const pop = RAccBase<T>::add(v);
        if (pop) {
            sum -= RAccBase<T>::pop_value;
        }
        return pop;
    }

    T get() const override
    {
        return sum;
    }
};

template<typename T>
struct RMean final : public RSum<T>
{
    explicit
    RMean(size_t const capacity = 8)
        : RSum<T>{capacity}
    {
    }

    [[nodiscard]]
    T get() const override
    {
        auto const count = static_cast<float>(RSum<T>::getCount());
        if (count == 0.0f) {
            return 0.0f;
        }
        float const sum = std::reduce(RSum<T>::values.begin(), RSum<T>::values.end());
        return sum / count;
    }
};


template<typename T>
struct RDiff final : public RAccBase<T>
{
    explicit
    RDiff(uint32_t max_size = 8)
        : RAccBase<T>(max_size)
    {
    }

    void addValue(T v)
    {
        RAccBase<T>::addValueBase(v);
    }

    T get() const override
    {
        return RAccBase<T>::values[RAccBase<T>::getIndex(-1)] - RAccBase<T>::values[RAccBase<T>::getIndex()];
    }
};
