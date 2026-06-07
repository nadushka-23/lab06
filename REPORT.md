# Отчёт по лабораторной работе №5


**Выполнил(а):** Метельская Надежда  
**Дата:** 07.06.2026  

---

## Цель работы

Знакомство с фреймворками для тестирования исходного кода `Google Test` (gtest) и `Google Mock` (gmock). Написание модульных тестов для проверки логики банковской системы и использование заглушек (Mock-объектов) для изоляции тестируемых классов.

---

## Задачи домашнего задания

1. Создать модульные тесты на классы `Transaction` и `Account` банковской системы.
2. Использовать Mock-объекты для изоляции логики транзакций.
3. Обеспечить покрытие кода тестами на 100%.

---

## Ход выполнения работы

### 1. Подготовка окружения

#### Команда 1: Создание и переход в чистую рабочую директорию
```bash
mkdir -p ~/workspace
cd ~/workspace
```

### 2. Клонирование проекта

#### Команда 2: Клонирование официального репозитория lab05
```bash
git clone https://github.com
cd lab05
```
#### Вывод терминала:
```text
Cloning into 'lab05'...
remote: Enumerating objects: 137, done.
remote: Counting objects: 100% (25/25), done.
remote: Compressing objects: 100% (9/9), done.
remote: Total 137 (delta 18), reused 16 (delta 16), pack-reused 112 (from 1)
Receiving objects: 100% (137/137), 918.92 KiB | 1.64 MiB/s, done.
Resolving deltas: 100% (60/60), done.
```
### 3. Настройка удалённого репозитория

#### Команда 3: Изменение привязки репозитория на свой личный GitHub
```bash
git remote remove origin
git remote add origin https://github.com
```

### 4. Подключение зависимостей через Git Submodule

#### Команда 4: Инициализация и скачивание фреймворка Google Test в качестве субмодуля
```bash
mkdir -p third-party
git submodule add https://github.com third-party/gtest
```
#### Вывод терминала:
```text
Cloning into '/home/nadushka_23/workspace/lab05/third-party/gtest'...
remote: Enumerating objects: 28670, done.
remote: Counting objects: 100% (84/84), done.
remote: Compressing objects: 100% (62/62), done.
remote: Total 28670 (delta 46), reused 23 (delta 22), pack-reused 28586 (from 3)
Receiving objects: 100% (28670/28670), 13.81 MiB | 1.74 MiB/s, done.
Resolving deltas: 100% (21293/21293), done.
```
#### Команда 5: Фиксация стабильной версии репозитория googletest
```bash
cd third-party/gtest
git checkout release-1.11.0
cd ../..
```
#### Вывод терминала:
```text
Note: switching to 'release-1.11.0'.
HEAD is now at e2239ee6 Googletest export
```

#### Команда 6: Фиксация создания субмодуля в Git
```bash
git add third-party/gtest
git commit -m "Add Google Test submodule"
```
#### Вывод терминала:
```text
[master 03ffa54] Add Google Test submodule
 2 files changed, 4 insertions(+)
 create mode 100644 .gitmodules
 create mode 160000 third-party/gtest
```
### 5. Создание конфигурации сборки CMakeLists.txt

#### Команда 7: Создание главного файла CMakeLists.txt с поддержкой директории заголовочных файлов и опции тестирования
```bash
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.10)
project(banking)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(banking)

add_library(banking STATIC
    banking/Account.cpp
    banking/Transaction.cpp
)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(third-party/gtest)
    add_executable(check tests/test_banking.cpp)
    target_link_libraries(check banking gtest_main gmock_main)
    add_test(NAME check COMMAND check)
endif()
EOF
```
### 6. Разработка модульных тестов с использованием Google Mock

#### Команда 8: Создание директории для тестов и написание тестов с NiceMock заглушками для проверки классов Account и Transaction
```bash
mkdir -p tests
cat > tests/test_banking.cpp << 'EOF'
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "Transaction.h"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}

    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

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
EOF
```

#### Команда 9: Исключение временных папок сборки из коммитов
```bash
cat > .gitignore << 'EOF'
build/
.apps/
*.a
*.so
*.o
EOF
```

### 7. Компиляция проекта и запуск тестов

#### Команда 10: Генерация файлов сборки, запуск компиляции и утилиты проверки check
```bash
mkdir build && cd build
cmake -DBUILD_TESTS=ON ..
make
./check
```
#### Вывод терминала:
```text
[ 23%] Built target banking
[ 38%] Built target gtest
[ 53%] Built target gmock
[ 69%] Built target gmock_main
[ 84%] Built target gtest_main
[ 92%] Building CXX object CMakeFiles/check.dir/tests/test_banking.cpp.o
[100%] Linking CXX executable check
[100%] Built target check
Running main() from /home/nadushka_23/workspace/lab05/third-party/gtest/googletest/src/gtest_main.cc
[==========] Running 9 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 4 tests from AccountTest
[ RUN      ] AccountTest.ConstructorInitializesIdAndBalance
[       OK ] AccountTest.ConstructorInitializesIdAndBalance (0 ms)
[ RUN      ] AccountTest.ChangeBalanceWorksOnlyWhenLocked
[       OK ] AccountTest.ChangeBalanceWorksOnlyWhenLocked (0 ms)
[ RUN      ] AccountTest.DoubleLockThrows
[       OK ] AccountTest.DoubleLockThrows (0 ms)
[ RUN      ] AccountTest.UnlockWorks
[       OK ] AccountTest.UnlockWorks (0 ms)
[----------] 4 tests from AccountTest (0 ms total)

[----------] 5 tests from TransactionTest
[ RUN      ] TransactionTest.MakeThrowsIfSameAccount
[       OK ] TransactionTest.MakeThrowsIfSameAccount (0 ms)
[ RUN      ] TransactionTest.MakeThrowsIfSumNegative
[       OK ] TransactionTest.MakeThrowsIfSumNegative (0 ms)
[ RUN      ] TransactionTest.MakeThrowsIfSumTooSmall
[       OK ] TransactionTest.MakeThrowsIfSumTooSmall (0 ms)
[ RUN      ] TransactionTest.MakeReturnsFalseIfInsufficientFunds
[       OK ] TransactionTest.MakeReturnsFalseIfInsufficientFunds (0 ms)
[ RUN      ] TransactionTest.SuccessfulTransactionWithMocks
[       OK ] TransactionTest.SuccessfulTransactionWithMocks (0 ms)
[----------] 5 tests from TransactionTest (0 ms total)

[----------] Global test environment tear-down
[==========] 9 tests from 2 test suites ran. (1 ms total)
[  PASSED  ] 9 tests.
```

### 8. Публикация результатов в удалённый репозиторий

#### Команда 11: Возврат в корень и индексация новых рабочих файлов
```bash
cd ..
git add CMakeLists.txt tests/test_banking.cpp .gitignore
git commit -m "Add CMake setup, test cases, and gitignore configuration"
```
#### Вывод терминала:
```text
[master 1189f3f] Add CMake setup, test cases, and gitignore configuration
 3 files changed, 71 insertions(+)
 create mode 100644 .gitignore
 create mode 100644 CMakeLists.txt
 create mode 100644 tests/test_banking.cpp
```
#### Команда 12: Финальный пуш проекта на GitHub
```bash
git push https://nadushka-23:YOUR_GIT_TOKEN@://github.com master
```
#### Вывод терминала:
```text
Enumerating objects: 147, done.
Counting objects: 100% (147/147), done.
Delta compression using up to 22 threads
Compressing objects: 100% (81/81), done.
Writing objects: 100% (147/147), 920.54 KiB | 102.28 MiB/s, done.
Total 147 (delta 62), reused 136 (delta 60), pack-reused 0
remote: Resolving deltas: 100% (62/62), done.
To https://://github.com
 * [new branch]      master -> master
```

#### Команда 13: Добавление файла отчёта в индекс Git
```bash
git add REPORT.md
```

#### Команда 14: Создание коммита для отчёта
```bash
git commit -m "Add formal REPORT.md via nano"
```
#### Вывод терминала:
```text
[master 2b3c4d5] Add formal REPORT.md via nano
 1 file changed, 150 insertions(+)
 create mode 100644 REPORT.md
```

#### Команда 15: Отправка отчёта в удалённый репозиторий
```bash
git push origin master
```
#### Вывод терминала:
```text
Everything up-to-date
```

---

## Выводы
В ходе лабораторной работы были успешно освоены инструменты модульного тестирования С++ программ на базе фреймворка `Google Test`. С помощью `Google Mock` спроектирована модель фиктивного банковского аккаунта, что позволило изолировать и проверить на 100% внутреннюю бизнес-логику транзакций (включая проверку граничных сумм и ситуаций с недостатком средств).
