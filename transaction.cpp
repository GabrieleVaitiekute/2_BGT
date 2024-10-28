#include "Transaction.h"
#include "HashFunk.h"

// Transaction constructor definition
Transaction::Transaction(const std::vector<UTXO>& inputs, const std::vector<UTXO>& outputs)
    : inputs(inputs), outputs(outputs) {
    txid = generateTxID(); 
}

