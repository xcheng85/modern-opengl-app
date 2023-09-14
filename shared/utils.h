#pragma once

#include <string>
#include <vector>

namespace SharedUtils
{
    int endsWith(const char *s, const char *part);

    std::string readShaderFile(const char *fileName);

    std::vector<unsigned int>  readFile(const std::string &filename);

    void printShaderSource(const char *text);

    
}