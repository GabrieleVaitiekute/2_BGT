#include "HashFunc.h"
#include "blockchain.h"

int main() {
    srand(time(0));
    std::vector<User> users;
    std::vector<Transaction> transactions;
    std::vector<Block> blockchain;
 
    generateUsers(users);
    generateTransactions(users, transactions);
    generateBlocks(transactions, blockchain);

    std::string txID ;
    std::cout<<"Iveskite transakcijos id:";
    std::cin >> txID;
    printTransaction(transactions, txID);

    std::string blockID;
    std::cout<<"Iveskite bloko id:";
    std::cin >> blockID;
    printBlock(blockchain, blockID);
    return 0;
}
