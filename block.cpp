#include "Block.h"
#include "HashFunc.h"

#include <ctime>
#include <sstream>

Block::Block(int idx, const std::string& prev_hash, const std::vector<Transaction>& txs)
    : index(idx), previous_hash(prev_hash), transactions(txs), nonce(0) {
    timestamp = std::time(nullptr);
    hash = calculateHash();
}

std::string Block::calculateHash() const {
    
    std::stringstream ss;
    ss << index << previous_hash << timestamp << nonce;

    for (const auto& tx : transactions) {
        ss << tx.txid;  
    }

    return hashInput(ss.str());  
}

bool Block::mineBlock(int difficulty) {
    std::string target(difficulty, '0');
    while (hash.substr(0, difficulty) != target) {
        nonce++;
        hash = calculateHash();
    }
    return true;
}