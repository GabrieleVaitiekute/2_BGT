#ifndef HASHFUNC_H
#define HASHFUNC_H
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>

static std::string hashInput(const std::string &input);
std::string padToLength(const std::string &input, size_t length);
std::string stringToHex(const std::string &output);
int calculateASCIIsum(const std::string &str);
unsigned char rotateRight(unsigned char value, int shift);
std::string avalancheXOR(const std::string &input);
std::vector<int> calculateSplitPoints(const std::string &output);
std::vector<std::string> splitString(const std::string &output);
std::string recombinePieces(std::vector<std::string> &pieces, const std::string &output);

#endif 