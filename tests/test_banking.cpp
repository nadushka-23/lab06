#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

// Создаем Mock-класс для Account
class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}

    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

// =========================================================================
// ТЕСТЫ ДЛЯ КЛАССА ACCOUNT
// =========================================================================

TEST(AccountTest, ConstructorInitializesIdAndBalance) {
    Account acc(1, 100);
    EXPECT_EQ(acc.GetBalance(), 100);
    EXPECT_EQ(acc.id(), 1);
}

TEST(AccountTest, ChangeBalanceWorksOnlyWhenLocked) {
    Account acc(1, 100);
    EXPECT_THROW(acc.ChangeBalance(50), std::runtime_error);

    acc.Lock();
    acc.ChangeBalance(50);
    EXPECT_EQ(acc.GetBalance(), 150);
}

TEST(AccountTest, DoubleLockThrows) {
    Account acc(1, 100);
    acc.Lock();
    EXPECT_THROW(acc.Lock(), std::runtime_error);
}

TEST(AccountTest, UnlockWorks) {
    Account acc(1, 100);
    acc.Lock();
    acc.Unlock();
    EXPECT_THROW(acc.ChangeBalance(50), std::runtime_error);
}

// =========================================================================
// ТЕСТЫ ДЛЯ КЛАССА TRANSACTION (С ИСПОЛЬЗОВАНИЕМ MOCK-ОБЪЕКТОВ)
// =========================================================================

TEST(TransactionTest, MakeThrowsIfSameAccount) {
    NiceMock<MockAccount> from(1, 100);
    NiceMock<MockAccount> to(1, 200);
    Transaction txn;
    
    EXPECT_THROW(txn.Make(from, to, 50), std::logic_error);
}

TEST(TransactionTest, MakeThrowsIfSumNegative) {
    NiceMock<MockAccount> from(1, 100);
    NiceMock<MockAccount> to(2, 200);
    Transaction txn;
    
    EXPECT_THROW(txn.Make(from, to, -50), std::invalid_argument);
}

TEST(TransactionTest, MakeThrowsIfSumTooSmall) {
    NiceMock<MockAccount> from(1, 100);
    NiceMock<MockAccount> to(2, 200);
    Transaction txn;
    
    EXPECT_THROW(txn.Make(from, to, 50), std::logic_error);
}

TEST(TransactionTest, MakeReturnsFalseIfInsufficientFunds) {
    NiceMock<MockAccount> from(1, 100);
    NiceMock<MockAccount> to(2, 200);
    Transaction txn;
    
    ON_CALL(from, GetBalance()).WillByDefault(Return(100));
    ON_CALL(to, GetBalance()).WillByDefault(Return(200));
    
    // Метод должен вернуть false, так как 100 < 300
    EXPECT_FALSE(txn.Make(from, to, 300));
}

TEST(TransactionTest, SuccessfulTransactionWithMocks) {
    NiceMock<MockAccount> from(1, 500);
    NiceMock<MockAccount> to(2, 200);
    Transaction txn;

    ON_CALL(from, GetBalance()).WillByDefault(Return(500));
    ON_CALL(to, GetBalance()).WillByDefault(Return(200));
    
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);

    EXPECT_TRUE(txn.Make(from, to, 150));
}
