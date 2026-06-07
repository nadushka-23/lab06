# Отчёт по лабораторной работе №6

## Изучение средств пакетирования на примере CPack и настройка автоматических релизов

**Выполнил(а):** Метельская Надежда  
**Дата:** 07.06.2026  

---

## Цель работы

Ознакомление со средствами пакетирования программного обеспечения в экосистеме CMake с помощью утилиты `CPack`. Настройка автоматической генерации архивов исходного кода (`.tar.gz`) и бинарных дистрибутивов (`.deb`, `.rpm`) с публикацией результатов на сервисе GitHub Releases при помощи GitHub Actions.

---

## Задачи домашнего задания

1. Создать публичный репозиторий с названием lab06 на сервисе GitHub.
2. Настроить автоматическое версионирование проекта в `CMakeLists.txt`.
3. Создать конфигурационный файл `CPackConfig.cmake` для сборщиков пакетов Linux.
4. Разработать приложение `solver`, интегрированное с библиотекой `banking`.
5. Написать сценарий GitHub Actions для сборки и публикации релизов при создании Git-тегов.

---

## Ход выполнения работы

### 1. Подготовка окружения и экспорт переменных

#### Команда 1: Инициализация переменных окружения GITHUB_USERNAME и GITHUB_EMAIL
```bash
export GITHUB_USERNAME=nadushka-23
git config --global user.email
export GITHUB_EMAIL=metelskaya23@yandex.ru
```
#### Вывод терминала:
```text
metelskaya23@yandex.ru
```

#### Команда 2: Настройка алиасов для текстового редактора и sed
```bash
alias edit=nano
alias gsed=sed
```

#### Команда 3: Подготовка чистой директории workspace
```bash
cd ~
rm -rf workspace
mkdir -p workspace
cd workspace
```

### 2. Клонирование и настройка репозитория

#### Команда 4: Клонирование репозитория lab05 в projects/lab06 и перепривязка origin
```bash
git clone https://github.com{GITHUB_USERNAME}/lab05.git projects/lab06
cd projects/lab06
git remote remove origin
git remote add origin https://github.com{GITHUB_USERNAME}/lab06.git
```
#### Вывод терминала:
```text
Cloning into 'projects/lab06'...
remote: Enumerating objects: 159, done.
remote: Counting objects: 100% (159/159), done.
remote: Compressing objects: 100% (85/85), done.
remote: Total 159 (delta 66), reused 158 (delta 65), pack-reused 0 (from 0)
Receiving objects: 100% (159/159), 926.07 KiB | 798.00 KiB/s, done.
Resolving deltas: 100% (66/66), done.
```

### 3. Инъекция параметров версии в CMakeLists.txt

#### Команда 5: Добавление переменных версионирования через утилиту gsed
```bash
gsed -i '/project(banking)/a\
set(PRINT_VERSION_STRING "v\${PRINT_VERSION}")
' CMakeLists.txt
gsed -i '/project(banking)/a\
set(PRINT_VERSION\
  \${PRINT_VERSION_MAJOR}.\${PRINT_VERSION_MINOR}.\${PRINT_VERSION_PATCH}.\${PRINT_VERSION_TWEAK})
' CMakeLists.txt
gsed -i '/project(banking)/a\
set(PRINT_VERSION_TWEAK 0)
' CMakeLists.txt
gsed -i '/project(banking)/a\
set(PRINT_VERSION_PATCH 0)
' CMakeLists.txt
gsed -i '/project(banking)/a\
set(PRINT_VERSION_MINOR 1)
' CMakeLists.txt
gsed -i '/project(banking)/a\
set(PRINT_VERSION_MAJOR 0)
' CMakeLists.txt
```

#### Команда 6: Проверка изменений с помощью git diff
```bash
git diff
```
#### Вывод терминала:
```diff
diff --git a/CMakeLists.txt b/CMakeLists.txt
index 74b9848..2c301c0 100644
--- a/CMakeLists.txt
+++ b/CMakeLists.txt
@@ -1,5 +1,12 @@
 cmake_minimum_required(VERSION 3.10)
 project(banking)
+set(PRINT_VERSION_MAJOR 0)
+set(PRINT_VERSION_MINOR 1)
+set(PRINT_VERSION_PATCH 0)
+set(PRINT_VERSION_TWEAK 0)
+set(PRINT_VERSION
+  ${PRINT_VERSION_MAJOR}.${PRINT_VERSION_MINOR}.${PRINT_VERSION_PATCH}.${PRINT_VERSION_TWEAK})
+set(PRINT_VERSION_STRING "v${PRINT_VERSION}")

 set(CMAKE_CXX_STANDARD 11)
 set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### 4. Создание конфигурационных файлов метаданных пакета

#### Команда 7: Создание файлов метаданных DESCRIPTION и ChangeLog.md
```bash
touch DESCRIPTION && edit DESCRIPTION
touch ChangeLog.md
export DATE="$(LANG=en_US date +'%a %b %d %Y')"
cat > ChangeLog.md << EOF
* ${DATE} ${GITHUB_USERNAME} <${GITHUB_EMAIL}> 0.1.0.0
- Initial RPM release
EOF
```

#### Команда 8: Генерация файла настроек CPackConfig.cmake
```bash
cat > CPackConfig.cmake << EOF
include(InstallRequiredSystemLibraries)
EOF

cat >> CPackConfig.cmake << EOF
set(CPACK_PACKAGE_CONTACT ${GITHUB_EMAIL})
set(CPACK_PACKAGE_VERSION_MAJOR \text{\${PRINT_VERSION_MAJOR}})
set(CPACK_PACKAGE_VERSION_MINOR \text{\${PRINT_VERSION_MINOR}})
set(CPACK_PACKAGE_VERSION_PATCH \text{\${PRINT_VERSION_PATCH}})
set(CPACK_PACKAGE_VERSION_TWEAK \text{\${PRINT_VERSION_TWEAK}})
set(CPACK_PACKAGE_VERSION \text{\${PRINT_VERSION}})
set(CPACK_PACKAGE_DESCRIPTION_FILE \text{\${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "static C++ library for printing")
EOF

cat >> CPackConfig.cmake << EOF

set(CPACK_RESOURCE_FILE_LICENSE \text{\${CMAKE_CURRENT_SOURCE_DIR}/LICENSE})
set(CPACK_RESOURCE_FILE_README \text{\${CMAKE_CURRENT_SOURCE_DIR}/README.md})
EOF

cat >> CPackConfig.cmake << EOF

set(CPACK_RPM_PACKAGE_NAME "print-devel")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "print")
set(CPACK_RPM_CHANGELOG_FILE \text{\${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog.md})
set(CPACK_RPM_PACKAGE_RELEASE 1)
EOF

cat >> CPackConfig.cmake << EOF

set(CPACK_DEBIAN_PACKAGE_NAME "libprint-dev")
set(CPACK_DEBIAN_PACKAGE_PREDEPENDS "cmake >= 3.0")
set(CPACK_DEBIAN_PACKAGE_RELEASE 1)
EOF

cat >> CPackConfig.cmake << EOF

include(CPack)
EOF
```

#### Команда 9: Подключение созданной конфигурации пакетирования в CMakeLists.txt и обновление README
```bash
cat >> CMakeLists.txt << EOF

include(CPackConfig.cmake)
EOF
gsed -i 's/lab05/lab06/g' README.md
```

### 5. Первая локальная сборка проекта и генерация TGZ

#### Команда 10: Генерация файлов CMake и запуск компиляции библиотеки
```bash
cmake -H. -B_build
cmake --build _build
```
#### Вывод терминала:
```text
-- The C compiler identification is GNU 13.3.0
-- The CXX compiler identification is GNU 13.3.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (2.0s)
-- Generating done (0.0s)
-- Build files have been written to: /home/nadushka_23/workspace/projects/lab06/_build
[ 33%] Building CXX object CMakeFiles/banking.dir/banking/Account.cpp.o
[ 66%] Building CXX object CMakeFiles/banking.dir/banking/Transaction.cpp.o
[100%] Linking CXX static library libbanking.a
[100%] Built target banking
```

#### Команда 11: Упаковка проекта в архив .tar.gz с помощью CPack
```bash
cd _build
cpack -G "TGZ"
cd ..
```
#### Вывод терминала:
```text
CPack: Create package using TGZ
CPack: Install projects
CPack: - Run preinstall target for: banking
CPack: - Install project: banking []
CPack: Create package
CPack: - package: /home/nadushka_23/workspace/projects/lab06/_build/banking-0.1.0.0-Linux.tar.gz generated.
```

#### Команда 12: Фиксация артефактов и работа с утилитой tree
```bash
mkdir artifacts
mv _build/*.tar.gz artifacts
sudo apt install tree -y
ls -la artifacts
```
#### Вывод терминала:
```text
total 12
drwxr-xr-x 2 nadushka_23 nadushka_23 4096 Jun  7 19:46 .
drwxr-xr-x 9 nadushka_23 nadushka_23 4096 Jun  7 19:46 ..
-rw-r--r-- 1 nadushka_23 nadushka_23   29 Jun  7 19:46 banking-0.1.0.0-Linux.tar.gz
```

#### Команда 13: Пуш базовой структуры на GitHub и создание тега v0.1.0.0
```bash
git add .
git commit -m "added cpack config"
git push -u origin master
```
#### Вывод терминала:
```text
[master 566757e] added cpack config
 5 files changed, 42 insertions(+), 6 deletions(-)
 create mode 100644 CPackConfig.cmake
 create mode 100644 ChangeLog.md
 create mode 100644 DESCRIPTION
Username for 'https://github.com': nadushka-23
Password for 'https://nadushka-23@github.com':
Enumerating objects: 166, done.
Counting objects: 100% (166/166), done.
Delta compression using up to 22 threads
Compressing objects: 100% (90/90), done.
Writing objects: 100% (166/166), 927.45 KiB | 92.75 MiB/s, done.
Total 166 (delta 69), reused 157 (delta 66), pack-reused 0
remote: Resolving deltas: 100% (69/69), done.
To https://github.com
 * [new branch]      master -> master
branch 'master' set up to track 'origin/master'.
```

### 6. Копирование официального репозитория заданий и подготовка к Homework

#### Команда 14: Переход в домашнюю папку и загрузка шаблона ЛР6
```bash
cd ~
export LAB_NUMBER=06
git clone https://github.com{LAB_NUMBER} tasks/lab${LAB_NUMBER}
mkdir -p reports/lab${LAB_NUMBER}
cp tasks/lab${LAB_NUMBER}/README.md reports/lab${LAB_NUMBER}/REPORT.md
```
#### Вывод терминала:
```text
Cloning into 'tasks/lab06'...
remote: Enumerating objects: 117, done.
remote: Counting objects: 100% (37/37), done.
remote: Compressing objects: 100% (4/4), done.
```
### 7. Реализация Homework: Разработка приложения solver и интеграция CI/CD

#### Команда 15: Возврат в директорию проекта и написание исходного кода solver.cpp
```bash
cd ~/workspace/projects/lab06
cat > solver.cpp << 'EOF'
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
EOF
```

#### Команда 16: Конфигурирование сборки исполняемого файла и CPack через nano
```bash
nano CMakeLists.txt
nano CPackConfig.cmake
```

#### Команда 17: Создание автоматического workflow-сценария релиза GitHub Actions
```bash
mkdir -p .github/workflows
cat > .github/workflows/release.yml << 'EOF'
name: Create Release

on:
  push:
    tags:
      - 'v*.*.*.*'

permissions:
  contents: write

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: sudo apt-get update && sudo apt-get install -y cmake build-essential rpm

      - name: Configure CMake
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release

      - name: Build
        run: cmake --build build

      - name: Package with CPack
        run: |
          cd build
          cpack -G DEB
          cpack -G RPM
          cpack -G TGZ
          mv *.deb *.rpm *.tar.gz ../

      - name: Create GitHub Release
        uses: softprops/action-gh-release@v2
        with:
          files: |
            *.deb
            *.rpm
            *.tar.gz
EOF
```

#### Команда 18: Фиксация изменений в ветке master и отправка на сервер
```bash
git add solver.cpp CMakeLists.txt CPackConfig.cmake .github/workflows/release.yml
git commit -m "Add solver and GitHub Actions release workflow"
git push origin master
```
#### Вывод терминала:
```text
[master 9b18a9f] Add solver and GitHub Actions release workflow
 4 files changed, 70 insertions(+), 1 deletion(-)
 create mode 100644 .github/workflows/release.yml
 create mode 100644 solver.cpp
Username for 'https://github.com': nadushka-23
Password for 'https://nadushka-23@github.com':
Enumerating objects: 13, done.
Counting objects: 100% (13/13), done.
Delta compression using up to 22 threads
Compressing objects: 100% (7/7), done.
Writing objects: 100% (8/8), 1.43 KiB | 1.43 MiB/s, done.
Total 8 (delta 3), reused 0 (delta 0), pack-reused 0
remote: Resolving deltas: 100% (3/3), completed with 3 local objects.
To https://github.com
   566757e..9b18a9f  master -> master
```

#### Команда 19: Создание и отправка тега v0.2.0.0 для инициализации автоматического релиза пакетов
```bash
git tag v0.2.0.0
git push origin v0.2.0.0
```
#### Вывод терминала:
```text
Username for 'https://github.com': nadushka-23
Password for 'https://nadushka-23@github.com':
Total 0 (delta 0), reused 0 (delta 0), pack-reused 0
To https://github.com
 * [new tag]         v0.2.0.0 -> v0.2.0.0
```

---

## Выводы

В ходе выполнения лабораторной работы были изучены базовые принципы автоматизированного создания дистрибутивов с помощью инструмента `CPack`. Разработано консольное приложение `solver`, интегрированное со статической банковской библиотекой, настроена автоматическая кросс-пакетизация проекта в форматы бинарных пакетов Linux (`.deb`, `.rpm`) и архива исходного кода (`.tar.gz`), успешно развернутая на GitHub Actions с триггером по тегам версий.
