#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>

struct instruction
{
    std::string operation;
    std::string operand1;
    std::string operand2;
};

class Assembler
{
    public:
        Assembler();
        ~Assembler();

        void Assemble(std::string path);

    private:
        void ParseFile(std::string path);
        void ParseCodeStringVector(std::vector<std::string> vector);
        void ParseDataStringVector(std::vector<std::string> vector);
        void Compile();
        void WriteFile();
        void Reset();

        std::string DecimalToBinary(int num);

    private:
        std::string path;
        std::vector<std::string> dataStrings;
        std::vector<std::string> codeStrings;
        std::vector<std::string> data;
        std::vector<instruction> code;
        std::vector<std::string> compiledOutput;
        std::unordered_map<std::string, std::string> operationsMap = {
            {"ADD","0000"},
            {"SUB","0001"},
            {"AND","0010"},
            {"OR","0011"},
            {"JMP","0100"},
            {"JGT","0101"},
            {"JLT","0110"},
            {"JEQ","0111"},
            {"INC","1001"},
            {"DEC","1010"},
            {"NOT","1011"},
            {"LOAD","1100"},
            {"STORE","1101"}
        };
        std::unordered_map<std::string, std::string> registersMap = {
            {"r0","000"},
            {"r1","001"},
            {"r2","010"},
            {"r3","011"},
            {"r4","100"},
            {"r5","101"},
            {"r6","110"},
            {"r7","111"}
        };
        std::unordered_map<std::string, int> labelsMap = {};
};