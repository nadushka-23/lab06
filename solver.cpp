#include <iostream>
#include "banking/Account.h"
#include "banking/Transaction.h"

int main() {
    Account from(1, 500);
    Account to(2, 100);
    Transaction txn;
    
    std::cout << "Before transaction: from=" << from.GetBalance() 
              << " to=" << to.GetBalance() << std::endl;
    
    bool success = txn.Make(from, to, 200);
    
    std::cout << "Transaction success: " << (success ? "yes" : "no") << std::endl;
    std::cout << "After transaction: from=" << from.GetBalance() 
              << " to=" << to.GetBalance() << std::endl;
    
    return 0;
}
