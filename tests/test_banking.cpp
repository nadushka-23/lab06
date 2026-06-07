#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"

using ::testing::_;
using ::testing::Return;

// Тесты для класса Account
TEST(AccountTest, ConstructorInitializesIdAndBalance) {
    Account acc(1, 100);
    EXPECT_EQ(acc.GetBalance(), 100);
}

TEST(AccountTest, ChangeBalanceWorksOnlyWhenLocked) {
    Account acc(1, 100);
    EXPECT_THROW(acc.ChangeBalance(50), std::runtime_error);
    acc.Lock();
    acc.ChangeBalance(50);
    EXPECT_EQ(acc.GetBalance(), 150);
}

TEST(AccountTest, UnlockWorks) {
    Account acc(1, 100);
    acc.Lock();
    acc.Unlock();
    EXPECT_THROW(acc.ChangeBalance(50), std::runtime_error);
}

// Тесты для класса Transaction
TEST(TransactionTest, MakeThrowsIfSameAccount) {
    Account from(1, 100);
    Account to(1, 100);
    Transaction txn;
    EXPECT_THROW(txn.Make(from, to, 200), std::logic_error);
}

TEST(TransactionTest, MakeThrowsIfSumNegative) {
    Account from(1, 100);
    Account to(2, 200);
    Transaction txn;
    EXPECT_THROW(txn.Make(from, to, -50), std::invalid_argument);
}
