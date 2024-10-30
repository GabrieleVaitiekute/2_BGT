#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include "block.h"

class Blockchain {
public:
    std::vector<Block> chain;

    Blockchain();

    Block createGenesisBlock();
    void addBlock(Block newBlock);
    bool isChainValid() const;
};

#endif