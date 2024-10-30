#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include "transaction.h"

class Block {
public:
    int index;
    std::string previous_hash;
    std::string hash;
    std::vector<Transaction> transactions;
    long timestamp;
    int nonce;

    Block(int idx, const std::string& prev_hash, const std::vector<Transaction>& txs);

    std::string calculateHash() const;
    bool mineBlock(int difficulty);
};

#endif 