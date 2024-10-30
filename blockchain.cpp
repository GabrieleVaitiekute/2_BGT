#include "blockchain.h"

Blockchain::Blockchain() {
    chain.push_back(createGenesisBlock());
}

Block Blockchain::createGenesisBlock() {
    std::vector<Transaction> genesisTransactions; 
    return Block(0, "0", genesisTransactions);
}

void Blockchain::addBlock(Block newBlock) {
    newBlock.previous_hash = chain.back().hash;
    newBlock.mineBlock(4);  
    chain.push_back(newBlock);
}

bool Blockchain::isChainValid() const {
    for (size_t i = 1; i < chain.size(); ++i) {
        if (chain[i].previous_hash != chain[i - 1].hash) {
            return false;
        }
        if (chain[i].hash != chain[i].calculateHash()) {
            return false;
        }
    }
    return true;
}