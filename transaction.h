#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>

// UTXO strukturos deklaracija
struct UTXO {
    std::string txid;  // Transakciju ID
    unsigned long amount;
    std::string recipient_public_key;  
};

// Transakciju klases deklarasija
class Transaction {
public:
    std::string txid;
    std::vector<UTXO> inputs;
    std::vector<UTXO> outputs;

    // Konstruktoriau deklarasija
    Transaction(const std::vector<UTXO>& inputs, const std::vector<UTXO>& outputs);

private:

    std::string generateTxID();
};

#endif // TRANSACTION_H