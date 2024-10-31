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

class User {
private:
    std::string name;
    std::string public_key;
    unsigned long balance;
public:
    User() = default;
    User(std::string name, std::string public_key, unsigned long balance)
    : name(name), public_key(public_key), balance(balance) {}

    ~User(){}

    std::string getName() const { return name; }
    std::string getPublicKey() const { return public_key; }
    unsigned long getBalance() const { return balance; }
    void setBalance( unsigned long newBalance) { balance = newBalance; }
};


class Transaction {
private:
    std::string transactionID;
    std::string sender_public_key;
    std::string recipient_public_key;  
    unsigned long amount;

public:
    Transaction() = default;
    Transaction(const std::string& sender, const std::string& recipient,  unsigned long amount)
        : sender_public_key(sender), recipient_public_key(recipient), amount(amount) {
        transactionID = generateTransactionID();
    }
    std::string generateTransactionID() const {
        std::stringstream ss;
        ss << sender_public_key << recipient_public_key << amount;
        return hashInput(ss.str()); 
    }
    bool validateTransaction(unsigned long senderBalance) const {
        return senderBalance >= amount;
    }
    ~Transaction() {}

    std::string  getTransactionID() const { return transactionID; }
    std::string  getSender_public_key() const { return sender_public_key; }
    std::string  getRecipient_public_key() const { return recipient_public_key; }
    unsigned long getAmount() const { return amount; }


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
        unsigned long balance = rand() % 1000000 + 100;
        users.emplace_back(name, public_key, balance);
        file << "Name: " << name << ", Public Key: " << public_key << ", Balance: " << balance << "\n";
    }
    file.close();
    std::cout << "Vartotojai sugeneruoti ir isvesti faile vartotojai.txt\n";
}


void generateTransactions( std::vector<User>& users, std::vector<Transaction>& transactions) {
    std::ofstream file("transakcijos.txt");
    if (!file) {
        std::cerr << "Nepavyko atidaryto failo.\n";
        return;
    }

    std::cout << "Transakcijos generuojamos...\n";
    for (int i = 0; i < 10000; ++i) {
        
        User& sender = users[rand() % users.size()];
        const User& recipient = users[rand() % users.size()];
        unsigned long amount = rand() % 1000 + 1;

        
        Transaction tx(sender.getPublicKey(), recipient.getPublicKey(), amount);
        if (tx.validateTransaction(sender.getBalance())) {
            sender.setBalance(sender.getBalance() - amount);
            transactions.push_back(tx);

            
            file << "Transaction ID: " << tx.getTransactionID()<< ", Sender: " << sender.getPublicKey()<< ", Recipient: " << recipient.getPublicKey()<< ", Amount: " << amount << "\n";
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
                << ", Siuntejas: " << tx.getSender_public_key()
                << ", Gavejas: " << tx.getRecipient_public_key()
                << ", Kiekis: " << tx.getAmount() << "\n";
        }
        file << "----------------------------------\n";

        std::cout << "Blokas " << blockchain.size() << " sugeneratas sekmingai.\n";
        
        // Atnaujinamasvartotojai.txt
        std::ofstream vartotojuFile("vartotojai.txt");
        if (vartotojuFile) {
            for (const auto& user : users) {
                vartotojuFile << "Vardas: " << user.getName() << ", Viesasis raktas: " << user.getPublicKey()
                        << ", Likutis: " << user.getBalance() << "\n";
            }
            vartotojuFile.close();
        }

        // Atnaujinamas transakcijos.txt
        std::ofstream transakcijuFile("transakcijos.txt");
        if (transakcijuFile) {
            for (const auto& tx : transactions) {
                transakcijuFile << "Transakcijos ID: " << tx.getTransactionID()
                                << ", Siuntejas: " << tx.getSender_public_key()
                                << ", Gavejas: " << tx.getRecipient_public_key()
                                << ", Kiekis: " << tx.getAmount() << "\n";
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
            std::cout << "Transakcijos ID: " << tx.getTransactionID()<< "\nSiuntejas: " << tx.getSender_public_key() << "\nGavejas: " << tx.getRecipient_public_key()<< "\nAmount: " << tx.getAmount() << "\n";
            return;
        }
    }
    std::cout << "Transakcija nerasta\n";
}

void printBlock(const std::vector<Block>& blockchain, const std::string& blockID) {
    for (const auto& block : blockchain) {
        if (block.getBlockID() == blockID) {
            std::cout << "Bloko ID: " << block.getBlockID()<< ", Buves Hash: " << block.getPreviousHash()<< ", Laikas: " << block.getTimestamp()<< ", Nonce: " << block.getNonce() << "\n";
            for (const auto& tx : block.getTransactions()) {
                std::cout << "  Transakcijos ID: " << tx.getTransactionID()<< "\nSiuntejas: " << tx.getSender_public_key()<< "\nGavejas: " << tx.getRecipient_public_key()<< "\nKiekis: " << tx.getAmount() << "\n";
            }
            return;
        }
    }
    std::cout << "Blokas nerastas.\n";
}

#endif 