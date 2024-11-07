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
#include <thread>
#include <atomic>
#include <functional>

std::random_device rd; 
std::mt19937 g(rd());   

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
    std::vector<UTXO>& getUTXOs() { return utxos; }
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

    ~Transaction() {}

    std::string generateTransactionID() const 
    {
        std::stringstream ss;
        for (const auto& utxo : inputs) ss << utxo.utxoID;
        for (const auto& utxo : outputs) ss << utxo.utxoID;
        return hashInput(ss.str());
    }
    bool verifyTransactionHash() const {  return generateTransactionID() == transactionID; }


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


    std::string calculateBlockIDWithNonce(int customNonce) const {
        std::stringstream ss;
        ss << previous_hash << timestamp << version << customNonce << difficulty;
        for (const auto& tx : transactions) {
            ss << tx.getTransactionID();
        }
        return hashInput(ss.str());
    }


public:
    Block(const std::string& prev_hash, const std::vector<Transaction>& txs, int difficulty = 1 )
    : previous_hash(prev_hash), transactions(txs), timestamp(time(nullptr)), nonce(0), difficulty(difficulty) {
    blockID = calculateBlockID();
}

    ~Block() {}

    std::string getBlockID() const { return blockID; }
    const std::vector<Transaction>& getTransactions() const { return transactions; }
    std::string getPreviousHash() const { return previous_hash; }
    int getVersion() const { return version; }
    int getNonce() const { return nonce; }
    int getDifficulty() const { return difficulty; }
    std::string getFormattedTimestamp() const {
        std::ostringstream oss;
        std::tm* tmPtr = std::localtime(&timestamp);  
        oss << std::put_time(tmPtr, "%d-%m-%Y %H:%M:%S");  // Formatas: DD-MM-YYYY HH:MM:SS
        return oss.str(); 
    }
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

    std::string calculateMerkleRoot() const{
        std::vector<std::string> transactionHashes;

        for (const auto& tx : transactions) {
            transactionHashes.push_back(tx.getTransactionID()); 
        }

        while (transactionHashes.size() > 1) {
            if (transactionHashes.size() % 2 != 0) {
                transactionHashes.push_back(transactionHashes.back());
            }

            std::vector<std::string> newLevel;
            for (size_t i = 0; i < transactionHashes.size(); i += 2) {
                std::string combinedHash = hashInput(transactionHashes[i] + transactionHashes[i + 1]);
                newLevel.push_back(combinedHash);
            }
            transactionHashes = newLevel;  
        }

        return transactionHashes.empty() ? "" : transactionHashes[0];
    }
    void mineBlockParallel(int threadCount = 4) 
    {
        std::string target(difficulty, '0');
        std::atomic<bool> found(false); 
        std::string validHash;
        int validNonce = 0;

        
        auto mineRange = [&](int startNonce) 
        {
            int localNonce = startNonce;
            std::string localHash;
            while (!found.load() && localNonce < startNonce + 1000000) 
            { 
                localHash = calculateBlockIDWithNonce(localNonce);
                if (localHash.substr(0, difficulty) == target) 
                {
                    found.store(true);  
                    validHash = localHash;
                    validNonce = localNonce;
                    break;
                }
                localNonce++;
            }
        };

        std::vector<std::thread> threads;
        int nonceIncrement = 1000000 / threadCount; 
        for (int i = 0; i < threadCount; i++) 
        {
            threads.emplace_back(mineRange, i * nonceIncrement);
        }

        for (auto& t : threads) {
            t.join();
        }

        if (found) 
        {
            blockID = validHash;
            nonce = validNonce;
            std::cout << "Blokas iskastas su hash: " << blockID << "\n";
        } else {
            std::cout << "Nepavyko rasti tinkamo hash\n";
        }
    }



};

const std::vector<std::string> baseNames = {
    "Agne","Janina", "Aleksas", "Jonas", "Marius", "Laura", "Ieva", "Tomas", "Gabija", "Karolis", "Ruta", "Justas"
};

void generateUsers(std::vector<User>& users) {
    std::ofstream file("vartotojai.txt");
    if (!file) {
        std::cerr << "Nepavyko atidaryto failo.\n";
        return;
    }
    std::uniform_int_distribution<> nameDist(0, baseNames.size() - 1);
    std::uniform_int_distribution<> numberDist(1, 99);

    std::cout << "Vartotojai generuojami...\n";
    for (int i = 0; i < 1000; ++i) {
        std::string name = baseNames[nameDist(g)] + std::to_string(numberDist(g));
        std::string public_key = hashInput(name);
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
    int targetTransactionCount = 10000;

    while (transactions.size() < targetTransactionCount) {
        User& sender = users[rand() % users.size()];
        User& recipient = users[rand() % users.size()];
        unsigned long amount = rand() % 1000 + 1;

        try {
            Transaction tx(sender.getPublicKey(), recipient.getPublicKey(), amount, sender.getUTXOs());
            transactions.push_back(tx);

            file << "Transaction ID: " << tx.getTransactionID()
                << ", Sender: " << sender.getPublicKey()
                << ", Recipient: " << recipient.getPublicKey()
                << ", Amount: " << amount << "\n";
        } catch (const std::runtime_error& e) {
           //std::cerr << "Transakcija negalima: " << e.what() << "\n";
        }
    }
    file.close();
    std::cout << "Transakcijos sugeneruotos ir isvestos faile transakcijos.txt\n";
}

void generateBlocks(std::vector<Transaction>& transactions, std::vector<Block>& blockchain, std::vector<User>& users) {
    std::ofstream file("blokai.txt");
    if (!file) {
        std::cerr << "Nepavyko atidaryti failo.\n";
        return;
    }

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
        std::vector<Transaction> blockTransactions(transactions.begin(), transactions.begin() + 100);;

        for (size_t i = 0; i < 100 && i < transactions.size(); ++i) {
            const auto& tx = transactions[i];
            if (tx.verifyTransactionHash()) {  
                blockTransactions.push_back(tx); 
            } else {
                std::cerr << "Klaida: Netinkamas transakcijos hash - " << tx.getTransactionID() << "\n";
            }
        }

        std::string prev_hash = blockchain.empty() ? "0" : blockchain.back().getBlockID();
        Block newBlock(prev_hash, blockTransactions, 1);  

        newBlock.mineBlockParallel();

        for (const auto& tx : blockTransactions) {
            for (auto& user : users) {
                if (user.getPublicKey() == tx.getSenderKey()) {
                    for (const auto& input : tx.getInputs()) {
                        user.removeUTXO(input.utxoID);
                    }
                }
                if (user.getPublicKey() == tx.getRecipientKey()) {
                    for (const auto& output : tx.getOutputs()) {
                        if (output.OwnerPublic_key == user.getPublicKey()) {
                            user.addUTXO(output);
                        }
                    }
                }
            }
        }

        transactions.erase(transactions.begin(), transactions.begin() + 100);

        blockchain.push_back(newBlock);

        // 
        file << "Block ID: " << newBlock.getBlockID() << ", Previous hash: " << newBlock.getPreviousHash() 
            << ", Timestamp: " << newBlock.getFormattedTimestamp() << ", Nonce: " << newBlock.getNonce() << "\n";
        file << "Transactions:\n";
        for (const auto& tx : blockTransactions) {
            file << "  Transaction ID: " << tx.getTransactionID()
                << ", Sender: " << tx.getSenderKey()
                << ", Recipient: " << tx.getRecipientKey() << "\n";
        }
        file << "----------------------------------\n";
        std::cout << "Blokas " << blockchain.size() << " sugeneruotas sekmingai.\n";

        std::ofstream vartotojuFile("vartotojai.txt"); 
        if (vartotojuFile) {
            for (auto& user : users) {
                vartotojuFile << "Name: " << user.getName() << ", Public key: " << user.getPublicKey() << "\n";
                for (const auto& utxo : user.getUTXOs()) {
                    vartotojuFile << "  UTXO ID: " << utxo.utxoID << ", Amount: " << utxo.amount << "\n";
                }
            }
            vartotojuFile.close();
        }

        std::ofstream transakcijuFile("transakcijos.txt");
        if (transakcijuFile) {
            for (const auto& tx : transactions) {
                transakcijuFile << "Transaction ID: " << tx.getTransactionID()
                                << ", Sender: " << tx.getSenderKey()
                                << ", Recipient: " << tx.getRecipientKey() << "\n";
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
            std::cout << std::left;
            std::cout << std::setw(20)<< "Transaction ID: " << tx.getTransactionID() << "\n" 
                    << std::setw(20)<< "Sender: " << tx.getSenderKey() << "\n"
                    << std::setw(20)<< "Recipient: " << tx.getRecipientKey() << "\n";
            unsigned long totalSent = 0;
            std::cout << std::setw(20)<< "Inputs:"<< "\n" ;
            for (const auto& input : tx.getInputs()) 
            {
                std::cout << std::setw(20) << "UTXO ID:" << input.utxoID << "\n"
                        << std::setw(20) << "Amount:" << input.amount << "\n";
                totalSent += input.amount;
            }
            std::cout << std::setw(20)<< "Total Sent: " << totalSent << "\n";

            unsigned long totalReceived = 0;
            std::cout << std::setw(20)<< "Outputs:" << "\n";
            for (const auto& output : tx.getOutputs()) {
                std::cout << std::setw(20) << "UTXO ID:" << output.utxoID << "\n"
                        << std::setw(20) << "Amount:" << output.amount << "\n";
                totalReceived += output.amount;
            }
            std::cout<< std::setw(20) << "Total Received: " << totalReceived << "\n";
            return;
        }
    }
    std::cout << "Transakcija nerasta\n";
}

void printBlock(const std::vector<Block>& blockchain, const std::string& blockID) {
    for (const auto& block : blockchain) {
        if (block.getBlockID() == blockID) {
            std::cout << std::left;
            std::cout<< std::setw(20) << "Block ID: " << block.getBlockID() << "\n"
                    << std::setw(20)<< "Previous hash: " << block.getPreviousHash()<< "\n"
                    << std::setw(20)<< "Merkle Root Hash: " << block.calculateMerkleRoot() << "\n"
                    << std::setw(20)<< "Timestamp: " << block.getFormattedTimestamp() << "\n"
                    << std::setw(20)<< "Nonce: " << block.getNonce()<< "\n"
                    << std::setw(20)<< "Difficulty: " << block.getDifficulty() << "\n";
            std::cout << std::setw(20) << "Transactions:" << "\n";
            for (const auto& tx : block.getTransactions()) {
                std::cout << "  " << std::setw(15) << "Transaction ID:" << tx.getTransactionID() << "\n"
                        << "  " << std::setw(15) << "Sender:" << tx.getSenderKey() << "\n"
                        << "  " << std::setw(15) << "Recipient:" << tx.getRecipientKey() << "\n";
            }
            return;
        }
    }
    std::cout << "Blokas nerastas.\n";
}

#endif