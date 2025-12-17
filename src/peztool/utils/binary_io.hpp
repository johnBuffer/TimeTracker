#pragma once

#include <fstream>

namespace pez
{
struct BinaryWriter
{
    std::ofstream outfile;

    explicit
    BinaryWriter(const std::filesystem::path& filepath)
        : outfile{filepath, std::istream::out | std::ios::binary}
    {}

    ~BinaryWriter()
    {
        if (outfile)
        {
            outfile.close();
        }
    }

    /** Dumps the provided value as a binary blob
     *
     * @tparam TValue The value's type
     * @param value The value to dump
     */
    template<typename TValue>
    void write(const TValue& value)
    {
        outfile.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
};

struct BinaryReader
{
    std::ifstream infile;

    explicit
    BinaryReader(const std::filesystem::path& filepath)
            : infile{filepath, std::ios_base::binary}
    {}

    ~BinaryReader()
    {
        if (infile)
        {
            infile.close();
        }
    }

    [[nodiscard]]
    bool isValid() const
    {
        return infile.operator bool();
    }

    template<typename TValue>
    TValue read()
    {
        TValue result = {};
        infile.read(reinterpret_cast<char*>(&result), sizeof(TValue));
        return result;
    }

    template<typename TValue>
    void readInto(TValue& value)
    {
        infile.read(reinterpret_cast<char*>(&value), sizeof(TValue));
    }
};
}