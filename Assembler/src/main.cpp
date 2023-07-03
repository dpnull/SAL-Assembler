#include <iostream>
#include <filesystem>

#include "Assembler.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "No arguments passed. Please, specify a path to a .sal file";
        return -1;
    }

    Assembler assembler;

    for (int i = 1; i < 2; i++)
    {
        std::cout << argv[i] << std::endl;
        if (std::filesystem::path(argv[i]).extension() != ".sal" && std::filesystem::path(argv[i]).extension() != ".SAL")
        {
            std::cerr << "File provided is not a .sal file";
            return -1;
        }
        assembler.Assemble(argv[i]);
    }
    return 0;
}