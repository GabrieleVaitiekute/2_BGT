#ifndef HASHFUNC_H
#define HASHFUNC_H
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
class HashFunc {
public:
    static std::string hashInput(const std::string &input);

private:
    static std::string padToLength(const std::string &input, size_t length);
    static std::string stringToHex(const std::string &output);
    static int calculateASCIIsum(const std::string &str);
    static unsigned char rotateRight(unsigned char value, int shift);
    static std::string avalancheXOR(const std::string &input);
    static std::vector<int> calculateSplitPoints(const std::string &output);
    static std::vector<std::string> splitString(const std::string &output);
    static std::string recombinePieces(std::vector<std::string> &pieces, const std::string &output);
};
#endif 