#include "HashFunc.h"
#include "blockchain.h"

int main() {
    srand(time(0));
    std::vector<User> users;
    std::vector<Transaction> transactions;
    std::vector<Block> blockchain;
 
    generateUsers(users);
    generateTransactions(users, transactions);
    generateBlocks(transactions, blockchain, users);

   

while (true) {
    std::cout<<"Ar norite atspausdinti:\n 1.transakcija\n 2.bloka\nBet kokia kita ivestis baigs programa!\nJusu pasirinkimas: ";
    int ivestis;
    std::cin >> ivestis;
    if(ivestis == 1)
    {
        std::string txID ;
        std::cout<<"Iveskite transakcijos id:";
        std::cin >> txID;
        printTransaction(transactions, txID);
    }
    else if(ivestis == 2)
    {
        std::string blockID;
        std::cout<<"Iveskite bloko id:";
        std::cin >> blockID;
        printBlock(blockchain, blockID);
    }
    else {
        std::cout<<"Darbas baigtas.";
        break;}
    }
    return 0;
}
