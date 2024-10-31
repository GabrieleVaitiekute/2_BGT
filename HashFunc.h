#ifndef HASHFUNC_H
#define HASHFUNC_H
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>

std::string padToLength(const std::string &input, size_t length)
{
    std::string padded = input;
    int s = 0;
    while (padded.length() < length)
    {
        padded += std::to_string(input.length() + s * 3);
        s++;
    }
    return padded; 
}

std::string stringToHex(const std::string &output)
{
    std::ostringstream hexStream;

    for (unsigned char c : output)
        hexStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);

    return hexStream.str();
}

int calculateASCIIsum(const std::string &str)
{
    int ascii_sum = 0;
    for (char ch : str)
        ascii_sum += static_cast<int>(ch);

    return ascii_sum;
}


unsigned char rotateRight(unsigned char value, int shift)
{
    return (value >> shift) | (value << (8 - shift));
}
// Funkcija, kuri naudoja XOR ir rotavimą, kad geriau išsklaidytų bitus
std::string avalancheXOR(const std::string &input)
{
    int rounds = 3;
    int prime = 257;
    std::string result = input;
    int dynamic_shift = (calculateASCIIsum(input) % 7) + 1; // Dinamiškas poslinkis

    for (int r = 0; r < rounds; r++)
    {
        for (size_t i = 0; i < result.length(); i++)
        {
            char xor_result = (result[i] ^ rotateRight(result[(i + 1) % result.length()], (dynamic_shift + r))) + (i % 7);

            // Papildoma modulo aritmetika
            xor_result = (xor_result + (i * prime)) % 256; // Modulo 256 išlaikyti char ribose

            xor_result ^= rotateRight(result[(i + 2) % result.length()], (5 + r + dynamic_shift)); // Papildomas poslinkis
            result[i] = xor_result;
        }
    }

    return result;
}


std::vector<int> calculateSplitPoints(const std::string &output)
{
    int max_splits = 10;
    std::vector<int> split_points;
    int totalLength = output.length();
    int ascii_sum = calculateASCIIsum(output);
    int rolling_hash = 0;

    for (int i = 1; i <= max_splits; i++)
    {
        rolling_hash = (rolling_hash + static_cast<int>(output[i % totalLength]) * i) % totalLength;
        int split_point = (ascii_sum * i + rolling_hash) % totalLength;
        if (split_point > 0 && split_point < totalLength && std::find(split_points.begin(), split_points.end(), split_point) == split_points.end())
        {
            split_points.push_back(split_point);
        }
    }

    std::sort(split_points.begin(), split_points.end());
    return split_points;
}

std::vector<std::string> splitString(const std::string &output)
{
    std::vector<int> splitPoints = calculateSplitPoints(output);
    std::vector<std::string> pieces;

    int start = 0;
    for (int split_point : splitPoints)
    {
        pieces.push_back(output.substr(start, split_point - start));
        start = split_point;
    }
    pieces.push_back(output.substr(start)); // Prideda paskutinę dalį

    return pieces;
}
std::string recombinePieces(std::vector<std::string> &pieces, const std::string &output)
{

    std::vector<std::pair<int, std::string>> piece_scores;
    int input_checksum = calculateASCIIsum(output);

    for (const auto &piece : pieces)
    {
        int score = calculateASCIIsum(piece) ^ input_checksum;
        piece_scores.push_back({score, piece});
    }

    std::sort(piece_scores.begin(), piece_scores.end(), [](const std::pair<int, std::string> &a, const std::pair<int, std::string> &b)
            { return a.first < b.first; });

    std::string recombinedString;
    for (const auto &piece : piece_scores)
        recombinedString += piece.second;

    return recombinedString;
}

std::string hashInput(const std::string &input)
{   
    std::string output = padToLength(input, 64);
    std::vector<std::string> pieces = splitString(output);
    output = recombinePieces(pieces, output);
    output = avalancheXOR(output); 
    return stringToHex(output).substr(0, 64);
}


#endif 