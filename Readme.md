
[![tag](https://img.shields.io/github/v/tag/mambaru/wfc.svg?sort=semver)](https://github.com/mambaru/wfc/tree/master)
[![Build Status](https://github.com/mambaru/wfc/workflows/C++%20CI/badge.svg?branch=master)](https://github.com/mambaru/wfc/tree/master)
[![Build Status](https://github.com/mambaru/wfc/workflows/C++%20CI/badge.svg?branch=mambaru)](https://github.com/mambaru/wfc/tree/mambaru)
[![Build Status](https://travis-ci.com/mambaru/wfc.svg?branch=master)](https://travis-ci.com/mambaru/wfc)
[![Build Status](https://travis-ci.com/mambaru/wfc.svg?branch=mambaru)](https://travis-ci.com/mambaru/wfc)
[![codecov](https://codecov.io/gh/mambaru/wfc/branch/master/graph/badge.svg)](https://codecov.io/gh/mambaru/wfc)

* Репозитарий на [github.com](https://github.com/mambaru/wfc).
* Документация [doxygen](https://mambaru.github.io/wfc/index.html).
* Отчет [coverage](https://mambaru.github.io/wfc/cov-report/index.html)


WFC - это фреймворк для разработки высконагруженных JSON-RPC сервисов (демонов) под различные профили нагрузки. Эта документация по разработке пакетов модулей на WFC. Как подключить разработанный пакет к проекту выполняемого демона описано здесь [wfcroot](https://github.com/mambaru/wfcroot) или изучите
[Демо-проект](https://github.com/mambaru/demod).

# Сборка и зависимости

## Зависимости:

* g++ 4.9 и выше или clang++
* boost-1.76 и выше
* cmake
* git
* lcov     (опционально) для отчетов coverage о покрытии тестами (make tests)
* valgrind (опционально) для запуска дополнительных тестов на утечки памяти (make coverage)
* cppcheck (опционально) для make cppcheck

## Сборка

WFC подключается к проекту как субмодуль, но можно его собрать независимо, чтобы убедиться что нет проблем со сборкой:

```bash
git clone https://github.com/mambaru/wfc.git
cd wfc
BOOST_ROOT=/path/to/boost make tests
```

Если скрипт сборки не обнаружит в стсеме boost, но он загрузит boost и установит необходимые модули в ./build

## Установить boost:

```bash
wget https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.gz
tar -xvf boost_1_76_0.tar.gz
cd boost_1_76_0
./bootstrap.sh --prefix=/path/to/boost
sudo ./b2 install --prefix=/path/to/boost
```
## Субмодули

При сборке автоматически инициализируются и загружаются следующие субмодули wfc:
- [cmake-ci](https://github.com/mambaru/cmake-ci) - вспомогательные скрипты для cmake и CI
- [faslib](https://github.com/migashko/faslib) - метабиблиотека для работы со списками типов и АОП
- [wjson](https://github.com/mambaru/wjson) - очень быстрый декларативный JSON-сериализатор на шаблонах
- [wlog](https://github.com/mambaru/wlog) - логер с большими возможностями по кастомизации
- [wflow](https://github.com/mambaru/wflow) - очереди и потоки
- [iow](https://github.com/mambaru/iow) - сетевая библиотека
- [wjrpc](https://github.com/mambaru/wjrpc) - библиотека поддержки JSON-RPC
- [wrtstat](https://github.com/mambaru/wrtstat) - для сбора статистики

## Инициализация проекта (пакет модулей )

```bash
mkdir wfc_demo
cd wfc_demo/
git init
# git remote add origin https://github.com/my-account/wfc_demo.git
git submodule add https://github.com/mambaru/cmake-ci external/cmake-ci
vim CMakeLists.txt
```
Файл CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.1)
project(wfc_demo)
include(cmake/ci.cmake)
wci_submodule(NAME wfc SUPERMODULE)
```
Убедитесь, что папки ../wfc не существует (например, осталась после пробной сборки wfc) иначе не сработает. Проблема может возникнуть только для локальных репозитариев и только на этапе инициализации проекта.
```bash
git add .
git commit -m "Initial commnit"
./external/cmake-ci/scripts/upgrade.sh auto
BOOST_ROOT=/path/to/boost make init
```
После апгрейда (`./external/cmake-ci/scripts/upgrade.sh auto`) в корневой папке появится папка **cmake/ci.cmake** и **Makefile** (тривиальная обертка запуска **cmake**) для упрощения сборки проекта. При первой сборке **wci_submodule** автоматически подключит **wfc** как субмодуль в **external/wfc**. Если он уже был подключен, то при необходимости инициализирует и обновит его, а так же все субмодули самой **wfc**.

Можно приступать к разработке. Пример пакета модулей для демона [demod](https://github.com/mambaru/demod) смотри [wfc_demo](https://github.com/mambaru/wfc_demo). Инструкции по инициализации пректа демона смотри [wfcroot](https://github.com/mambaru/wfcroot). Информация по **wci_submodule** и все что связано со сборкой и субмодулями в [cmake-ci](https://github.com/mambaru/cmake-ci)
