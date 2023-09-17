#pragma once

#include <string>
#include <vector>
#include <ranges>
// ranges::copy
#include <algorithm>

namespace SharedUtils
{
    using namespace std;

    int endsWith(const char *s, const char *part);

    std::string readShaderFile(const char *fileName);

    std::vector<unsigned int> readFile(const std::string &filename);

    void printShaderSource(const char *text);

    // required by boost ext di
    struct ConfigString : std::string
    {
        using std::string::string;
    };

    // versioning of function
    inline namespace
    {
        namespace rv = std::ranges::views;
        namespace rg = std::ranges;

        void convert(std::vector<ConfigString> const &in, std::vector<const char *> &out)
        {
            out.resize(in.size());
            auto v = in | rv::transform([](const ConfigString &s)
                                        { return s.c_str(); });
            rg::copy(v, out.begin());
        }
    }

    namespace version_2
    {
        void convert(std::vector<string> const &in, std::vector<const char *> &out);
    }
}