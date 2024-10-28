#include "User.h"

User::User(std::string name, std::string public_key, unsigned long balance)
    : name(name), public_key(public_key), balance(balance) {}