#include "Assembler.hpp"
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

Assembler::Assembler()
{
    
}

Assembler::~Assembler()
{

}

std::string Assembler::DecimalToBinary(int num) // blatantly stolen off the internet and slightly tailored to our needs, same as above.
                                     // third example block at https://www.geeksforgeeks.org/program-decimal-binary-conversion/
{
    std::string str;
    while(num){
        if(num & 1) // 1
            str+='1';
        else // 0
            str+='0';
        num>>=1; // Right Shift by 1 
    }
    if (str.size() > 22)
        std::cerr << "Operand size exceeds 22 bits" << std::endl;
    return str;
}

void Assembler::ParseFile(std::string path)
{
    std::ifstream file(path);
    if (file.is_open())
    {
        std::string line;
        bool isInData = false;
        while (std::getline(file, line, '\x0A'))
        {
            std::string::iterator it;

            for (it = line.begin(); it != line.end(); it++) {
                if (!isspace(*it)) {
                    break;
                }
            }

            line.erase(line.begin(), it);
            if (line.find(".data") != std::string::npos)
                isInData = true;
            if (line.find(".code") != std::string::npos)
                isInData = false;

            for (it = line.begin(); it != line.end(); it++) {
                if (*it == '/') {
                    line.erase(it, line.end());
                    break;
                }
            }
            if (line.empty())
            {
                continue;
            }
            if (!isInData)
                codeStrings.push_back(line);
            else
                dataStrings.push_back(line);

        }
    }
}

void Assembler::ParseCodeStringVector(std::vector<std::string> vector)
{
    int extendedCount = 0;
    for (int i = 0; i < vector.size(); i++)
    {
        /**for (int n = 0; n < vector[i].size(); n++) {
                if (isspace(vector[i][n]) && vector[i][n] != '\0') {
                    vector[i].erase(n, 1);
                }
        }**/
        if (vector[i].find(".code") != std::string::npos)
            continue;
        if (vector[i].find(':') != std::string::npos)
        {
            std::string::iterator it;
            for (it = vector[i].begin(); it <= vector[i].end(); it++)
            {
                if (*it == ' ')
                {
                    vector[i].erase(it);
                    it--;
                }
            }
            labelsMap.insert({vector[i].erase(vector[i].size() - 1, 1), code.size() + extendedCount});
            continue;
        }
        std::string unprocessedString = vector[i];
        std::string operation;
        std::string operand1;
        std::string operand2;
        std::string::iterator it;

            for (it = vector[i].begin(); it != vector[i].end(); it++) {
                if (!isspace(*it) && *it != '\0') {
                    unprocessedString.erase(0, 1);
                    operation += *it;
                }
                else if(!unprocessedString.empty()){
                    unprocessedString.erase(0, 1);
                    break;
                }
                else {
                break;
                }
            }
            for (it = unprocessedString.begin(); it <= unprocessedString.end(); it++)
            {
                if (*it == ' ')
                {
                    unprocessedString.erase(it);
                    it--;
                }
            }
            for (int i = 0; i < unprocessedString.size(); i++) {
                if (isspace(unprocessedString[i]) && unprocessedString[i] != '\0') {
                    unprocessedString.erase(i, 1);
                }
            }
            for (it = unprocessedString.begin(); it != unprocessedString.end(); unprocessedString.erase(0, 1)) {
                if (*it != ',' && *it != '\0'){
                    operand1 += *it;
                }
                else if (*it == ','){
                    unprocessedString.erase(0, 1);
                    break;
                }
                else {
                    break;
                }
            }
            operand2 = unprocessedString;
            if (operand2.find('#') != std::string::npos)
                {
                    std::string tmp = operand2;
                    tmp.erase(0, 1);
                    if (DecimalToBinary(stoi(tmp)).size() > 6)
                        extendedCount++;
                }
            code.push_back(instruction{ operation, operand1, operand2 });
    }
}

void Assembler::ParseDataStringVector(std::vector<std::string> vector)
{
    for (int i = 0; i < vector.size(); i++)
    {
        std::string::iterator it;
        for (it = vector[i].begin(); it <= vector[i].end(); it++)
            {
                if (*it == ' ')
                {
                    vector[i].erase(it);
                    it--;
                }
            }
        if (vector[i] == ".data")
            continue;
        data.push_back(vector[i]);
        labelsMap.insert({vector[i], labelsMap.size()});
    }
}

bool isNumber(const std::string& s) // to be removed after labels are handled correctly.
{
    for (char const &ch : s) {
        if (std::isdigit(ch) == 0)
            return false;
    }
    return true;
}

std::string reverse(std::string str)
{
    std::string result;
    for(int i=str.size()-1 ; i>=0 ; i--)
        result += str[i];
    return result;
}

void Assembler::Compile()
{
    if (!labelsMap.empty())
    {
        for (auto it = labelsMap.cbegin(); it != labelsMap.cend(); ++it)
            std::cout << (*it).first << " : " << (*it).second << std::endl;
    }
    if (!code.empty())
    {
        for (int i = 0; i < code.size(); i++)
        {
            std::string opcode = operationsMap[code[i].operation]; //DONE
            std::string reg = "000"; // DONE
            std::string size = "0"; // DONE
            std::string rva = "00"; // DONE
            std::string operand = "000000"; // NEED LABEL CONVERT FOR RVA 10

            if (code[i].operation != "JMP") // for every operation except "jmp" first operand always contains a register,
                                            // so we can set reg instantly.
                reg = registersMap[code[i].operand1];
            else
            {
                rva = "10"; // for jmp operand is always of address type, so we set rva to 10.
            }
            if (code[i].operand2.empty() && code[i].operation != "JMP")
            {
                rva = "11"; // the lack of second operand and that instruction is not "jmp" indicates its
                            // either INC, DEC, or NOT, which always have rva of 11.
            }
            else { // above if statement establishes that its one of INC, DEC, NOT or JMP, else its a full 2-operand operation.
                   // Now we figure out what kind of data type lie in operand.
                if (code[i].operand2.find('#') != std::string::npos){ // this indicates its numeric, so rva of 01
                    rva = "01";
                }
                else if (code[i].operand2.find('r') != std::string::npos){ // this indicates its a register, so rva of 00
                    rva = "00";
                }
                else {
                    rva = "10"; // if neither of above, then it can only be an address.
                                // (operand2 might be empty at all if its JMP instruction, still an address)
                }
            } // TODO: operand should already be replaced with a matching address number instead of a label
              // before this else statement, else false-positives are possible.
            if (rva == "00") // setting operand for register-operand as 000+registerID, as operand is 6 bytes and ID is only 3.
                operand = "000" + registersMap[code[i].operand2];
            else if (rva == "01" || rva == "10") // statement for numeric- and address-operand
            {
                std::string tmp = code[i].operand2; // copy original data in operand2 to avoid overwriting it,
                                                    // so original data could be seen in debugging if necessary.
                if (tmp.empty())
                    tmp = code[i].operand1; // operand2 could be empty if we are dealing with JMP, which means data is at operand1.
                if (rva == "01")
                {
                    tmp.erase(0, 1); // erase first character from numeric operand, which is always "#", for future conversion.
                }
                else if (!isNumber(tmp)) {
                    if (labelsMap.count(tmp) > 0)
                        tmp = std::to_string(labelsMap[(tmp)]);
                    else
                        std::cerr << "Unmapped label \"" << tmp << "\" encountered";
                }
                std::string binary;
                if (isNumber(tmp))
                    binary = DecimalToBinary(stoi(tmp)); // convert to binary, result in reverse order.
                else
                    binary = tmp; // just a stub as currently operand can contain text labels, which crashes stoi.
                if (binary.size() <= 6) // if converted binary is shorter than 6 bits
                {
                    while (binary.size() < 6)
                    {
                        binary += '0'; // append remaining bits to fill operand to 6 bits. We add to the back, as currently bit-order is reversed.
                    }
                    size = "0"; // size is 0, as operand doesnt exceed 6 bits.
                }
                else
                {
                    while (binary.size() < 22)
                        binary += '0'; // As above, but fill to 22 bits.
                    size = "1"; // size is 1, as operand is of extended type.
                }
                operand = reverse(binary); // flip the bit order.
            }
            std::string result;
            if (operand.size() <= 6)
                result = opcode + reg + size + rva + operand;
            else
            {
                std::string finalOperand;
                for (int i = 0; i < 6; i++) {
                    finalOperand += operand[0];
                    operand.erase(0, 1);
                }
                result = opcode + reg + size + rva + finalOperand;
            }
             // assemble the result from pieces.
            compiledOutput.push_back(result); // string vector for compiled lines. Convenient for future writing to file line-by-line with null-terminators.
            if (operand.size() == 16)
                compiledOutput.push_back(operand);
        }
    }
    for (int i = 0; i < compiledOutput.size(); i++)
    {
        std::cout << compiledOutput[i] << std::endl; // this is just a stub, to be removed after write to file is implemented.
    }
}

void Assembler::WriteFile()
{
    std::filesystem::path fs(Assembler::path);
    std::string path = (fs.parent_path().string().empty()) ? "" : fs.parent_path().string() + '/';
    std::string stem = fs.stem().string();
    std::string ext = ".out";
    std::string pathOut = path + stem + ext;
    std::cout << pathOut << std::endl;
    std::ofstream file(pathOut);
    for(size_t i{}; i<compiledOutput.size(); i++) {
        file << compiledOutput.at(i) << '\n';
    }
    file.close();
}

void Assembler::Reset()
{
    path.clear();
    dataStrings.clear();
    codeStrings.clear();
    data.clear();
    code.clear();
    compiledOutput.clear();
    labelsMap.clear();
}

void Assembler::Assemble(std::string path)
{
    Assembler::path = path;
    ParseFile(path);
    if (!dataStrings.empty())
        ParseDataStringVector(dataStrings);
    ParseCodeStringVector(codeStrings);
    if (!data.empty() || !code.empty())
        Compile();
    WriteFile();
    Reset();
}
