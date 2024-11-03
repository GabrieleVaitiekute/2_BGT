#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "HashFunc.h"
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <random>

// UTXO struktūra apibrėžia: ID, savininko viešąjį raktą ir vertę
struct UTXO {
    std::string utxoID;
    std::string OwnerPublic_key;
    unsigned long amount;

    UTXO(const std::string& owner, unsigned long amount) : OwnerPublic_key(owner), amount(amount) {
        std::stringstream ss;
        ss << owner << amount;
        utxoID = hashInput(ss.str());
    }
};
class User {
private:
    std::string name;
    std::string public_key;
    std::vector<UTXO> utxos; 
public:
    User() = default;
    User(std::string name, std::string public_key, unsigned long initialBalance)
        : name(name), public_key(public_key) {
        // Sukuriamas pradinis UTXO su nurodytu balansu
        utxos.push_back(UTXO(public_key, initialBalance));
    }
    ~User(){}

    std::string getName() const { return name; }
    std::string getPublicKey() const { return public_key; }
    std::vector<UTXO>& getUTXOs()  { return utxos; }
    void addUTXO(const UTXO& utxo) { utxos.push_back(utxo); }
    void removeUTXO(const std::string& utxoID) {
        // Šalina UTXO pagal jo ID
        utxos.erase(std::remove_if(utxos.begin(), utxos.end(),
            [&utxoID](const UTXO& utxo) { return utxo.utxoID == utxoID; }),
            utxos.end());
    }
};


class Transaction {
private:
    std::string transactionID;
    std::string sender_public_key;
    std::string recipient_public_key;  
    std::vector<UTXO> inputs;   // Transakcijos įėjimai (naudojami UTXOs)
    std::vector<UTXO> outputs;  // Transakcijos išėjimai (nauji UTXOs)

public:
    Transaction() = default;
    Transaction(const std::string& sender, const std::string& recipient, unsigned long amount, std::vector<UTXO>& availableUTXOs)
        : sender_public_key(sender), recipient_public_key(recipient) {

        // Kaupiamas suma, kurią reikia pervesti
        unsigned long totalInput = 0;
        for (const auto& utxo : availableUTXOs) {
            inputs.push_back(utxo);
            totalInput += utxo.amount;
            if (totalInput >= amount) break; // Surenkama pakankama suma

            // Jei trūksta lėšų, įėjimai nesurenkami
            if (totalInput < amount) throw std::runtime_error("Nepakankamas balansas.");
        }

        // Sukuriamas gavėjo UTXO su reikiama suma
        outputs.push_back(UTXO(recipient, amount));
        
        // Jei yra likutis, jis grąžinamas kaip "grąžos" UTXO
        if (totalInput > amount) {
            unsigned long change = totalInput - amount;
            outputs.push_back(UTXO(sender, change));
        }

        // Transakcijos ID generuojamas iš įėjimų ir išėjimų
        transactionID = generateTransactionID();
    }

    std::string generateTransactionID() const {
        std::stringstream ss;
        for (const auto& utxo : inputs) ss << utxo.utxoID;
        for (const auto& utxo : outputs) ss << utxo.utxoID;
        return hashInput(ss.str());
    }
    ~Transaction() {}

   std::string getSenderKey()const { return sender_public_key;}
    std::string getRecipientKey()const { return recipient_public_key;}
    std::string getTransactionID() const { return transactionID; }
    const std::vector<UTXO>& getInputs() const { return inputs; }
    const std::vector<UTXO>& getOutputs() const { return outputs; }


};


class Block {
private:
    std::string blockID;
    std::string previous_hash;
    time_t timestamp; 
    int version = 1;
    int nonce;
    int difficulty = 1;
    std::vector<Transaction> transactions; 

public:
    Block(const std::string& prev_hash, const std::vector<Transaction>& txs, int difficulty = 1)
    : previous_hash(prev_hash), transactions(txs), timestamp(time(nullptr)), nonce(0), difficulty(difficulty) {
    blockID = calculateBlockID();
}

    ~Block() {}

    std::string getBlockID() const { return blockID; }
    const std::vector<Transaction>& getTransactions() const { return transactions; }
    std::string getPreviousHash() const { return previous_hash; }
    const time_t& getTimestamp() const { return timestamp; }
    int getVersion() const { return version; }
    int getNonce() const { return nonce; }
    int getDifficulty() const { return difficulty; }

    void setNonce(int newNonce) { 
        nonce = newNonce;
        blockID = calculateBlockID();  
    }


    std::string calculateBlockID() const {
        std::stringstream ss;
        ss << previous_hash << timestamp << version << nonce << difficulty;

        for (const auto& tx : transactions) {
            ss << tx.getTransactionID();
        }

        return hashInput(ss.str()); 
    }

    void mineBlock() {
        std::string target(difficulty, '0'); 
        while (blockID.substr(0, difficulty) != target) {
            nonce++;  
            blockID = calculateBlockID();  
        }
        std::cout << "Blokas iskastas su hash: " << blockID << "\n";
    }
};

void generateUsers(std::vector<User>& users) {
    std::ofstream file("vartotojai.txt");
    if (!file) {
        std::cerr << "Nepavyko atidaryto failo.\n";
        return;
    }

    std::cout << "Vartotojai generuojami...\n";
    for (int i = 0; i < 1000; ++i) {
        std::string name = "User" + std::to_string(i);
        std::string public_key = "public_key_" + std::to_string(i);
        unsigned long initialBalance = rand() % 1000000 + 100;
        users.emplace_back(name, public_key, initialBalance);
        file << "Name: " << name << ", Public Key: " << public_key << ", Balance: " << initialBalance << "\n";
    }
    file.close();
    std::cout << "Vartotojai sugeneruoti ir isvesti faile vartotojai.txt\n";
}


void generateTransactions(std::vector<User>& users, std::vector<Transaction>& transactions) {
    std::ofstream file("transakcijos.txt");
    if (!file) {
        std::cerr << "Nepavyko atidaryti failo.\n";
        return;
    }

    std::cout << "Transakcijos generuojamos...\n";
    for (int i = 0; i < 10000; ++i) {
        User& sender = users[rand() % users.size()];
        User& recipient = users[rand() % users.size()];
        unsigned long amount = rand() % 1000 + 1;

        try {
            Transaction tx(sender.getPublicKey(), recipient.getPublicKey(), amount, sender.getUTXOs());
            for (const auto& utxo : tx.getInputs()) sender.removeUTXO(utxo.utxoID);
            for (const auto& utxo : tx.getOutputs()) recipient.addUTXO(utxo);
            transactions.push_back(tx);

            file << "Transaction ID: " << tx.getTransactionID()
                 << ", Sender: " << sender.getPublicKey()
                 << ", Recipient: " << recipient.getPublicKey()
                 << ", Amount: " << amount << "\n";
        } catch (const std::runtime_error& e) {
            std::cerr << "Transakcija negalima: " << e.what() << "\n";
        }
    }
    file.close();
    std::cout << "Transakcijos sugeneruotos ir isvestos faile transakcijos.txt\n";
}

void generateBlocks(std::vector<Transaction>& transactions, std::vector<Block>& blockchain, std::vector<User>& users) {
    std::ofstream file("blokai.txt");
    if (!file) {
        std::cerr << "Nepavyko atidaryto failo.\n";
        return;
    }

    std::random_device rd; 
    std::mt19937 g(rd());   

    std::string user_response;
    while (true) {

        if (transactions.size() < 100) {
            std::cout << "Nepakankamai transakciju.\n";
            break;
        }

        
        std::cout << "Ar norite sugeneruoti bloka? (t/n): ";
        std::cin >> user_response;

        if (user_response != "t") {
            std::cout << "Bloku generavimas sustabdytas.\n";
            break;
        }

    
        std::shuffle(transactions.begin(), transactions.end(), g);
    
        std::vector<Transaction> blockTransactions(transactions.begin(), transactions.begin() + 100);
        
        if (transactions.size() >= 100) {
            transactions.erase(transactions.begin(), transactions.begin() + 100);
        }


        std::string prev_hash = blockchain.empty() ? "0" : blockchain.back().getBlockID();
        Block newBlock(prev_hash, blockTransactions, 1);  
        newBlock.mineBlock();
        blockchain.push_back(newBlock);

        file << "Bloko ID: " << newBlock.getBlockID() << ", Buves Hash: " << newBlock.getPreviousHash() 
            << ", Laikas: " << newBlock.getTimestamp() << ", Nonce: " << newBlock.getNonce() << "\n";
        file << "Transakcijos:\n";
        for (const auto& tx : blockTransactions) {
            file << "  Transakcijos ID: " << tx.getTransactionID()
                << ", Siuntejas: " << tx.getSenderKey()
                << ", Gavejas: " << tx.getRecipientKey() << "\n";
        }
        file << "----------------------------------\n";

        std::cout << "Blokas " << blockchain.size() << " sugeneratas sekmingai.\n";

        // Atnaujinamasvartotojai.txt
        std::ofstream vartotojuFile("vartotojai.txt");
        if (vartotojuFile) {
            for (const auto& user : users) {
                vartotojuFile << "Vardas: " << user.getName() << ", Viesasis raktas: " << user.getPublicKey()<< "\n";
            }
            vartotojuFile.close();
        }

        // Atnaujinamas transakcijos.txt
        std::ofstream transakcijuFile("transakcijos.txt");
        if (transakcijuFile) {
            for (const auto& tx : transactions) {
                transakcijuFile << "Transakcijos ID: " << tx.getTransactionID()
                                << ", Siuntejas: " << tx.getSenderKey()
                                << ", Gavejas: " << tx.getRecipientKey() << "\n";
            }
            transakcijuFile.close();
        }
    }

    file.close();
    std::cout << "Sugeneruoti blokai isvesti faile blokai.txt\n";
}

void printTransaction(const std::vector<Transaction>& transactions, const std::string& txID) {
    for (const auto& tx : transactions) {
        if (tx.getTransactionID() == txID) {
            std::cout << "Transakcijos ID: " << tx.getTransactionID()<< "\nSiuntejas: " << tx.getSenderKey() << "\nGavejas: " << tx.getRecipientKey()<< "\n";
            return;
        }
    }
    std::cout << "Transakcija nerasta\n";
}

void printBlock(const std::vector<Block>& blockchain, const std::string& blockID) {
    for (const auto& block : blockchain) {
        if (block.getBlockID() == blockID) {
            std::cout << "Bloko ID: " << block.getBlockID()<< "\nBuves Hash: " << block.getPreviousHash()<< "\nLaikas: " << block.getTimestamp()<< "\nNonce: " << block.getNonce() << "\n";
            for (const auto& tx : block.getTransactions()) {
                std::cout << "  Transakcijos ID: " << tx.getTransactionID()<< "\nSiuntejas: " << tx.getSenderKey()<< "\nGavejas: " << tx.getRecipientKey()<< "\n";
            }
            return;
        }
    }
    std::cout << "Blokas nerastas.\n";
}

#endif 