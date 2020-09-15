
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

Зависимости:

* g++ 4.9 и выше или clang++
* boost-1.73 и выше
* cmake
* git
* valgrind (опционально)
* cppcheck (опционально)

Установить свежий boost:

```bash
wget https://dl.bintray.com/boostorg/release/1.63.0/source/boost_1_73_0.tar.gz
tar -xvf boost_1_73_0.tar.gz
cd boost_1_73_0
./bootstrap.sh --prefix=/usr/local/boost-1.73
sudo ./b2 install --prefix=/usr/local/boost-1.73
```

Пропишите в систему BOOST_ROOT или устанавливайте перед запуском, например:
```bash
BOOST_ROOT=/usr/local/boost-1.73 cmake ..
```

WFC подключается к проекту как субмодуль, но можно его собрать независимо, чтобы убедиться что нет проблем со сборкой:

```bash
git clone https://github.com/mambaru/wfc.git
cd wfc
make tests
```

При сборке автоматически инициализируются и загружаются следующие субмодули wfc:
- [cmake-ci](https://github.com/mambaru/cmake-ci) - вспомогательные скрипты для cmake и CI
- [faslib](https://github.com/migashko/faslib) - метабиблиотека для работы со списками типов и АОП
- [wjson](https://github.com/mambaru/wjson) - очень быстрый декларативный JSON-сериализатор на шаблонах
- [wlog](https://github.com/mambaru/wlog) - логер с большими возможностями по кастомизации
- [wflow](https://github.com/mambaru/wflow) - очереди и потоки
- [iow](https://github.com/mambaru/iow) - сетевая библиотека
- [wjrpc](https://github.com/mambaru/wjrpc) - библиотека поддержки JSON-RPC
- [wrtstat](https://github.com/mambaru/wrtstat) - для сбора статистики


# Разработка демонов, пакетов и библиотек на WFC

Можно выделить три вида проектов:
* Библиотеки ( Пример: [wlog](https://github.com/mambaru/wlog) )
* WFC-пакеты ( Пример: [wfc_demo](https://github.com/mambaru/wfc_demo) )
* Демоны ( Пример: [demod](https://github.com/mambaru/demod) )

## Разработка приложений (демонов) на базе WFC

[wfcroot](https://github.com/mambaru/wrtstat)

## Разработка WFC-пакетов

Для того чтобы легче визуально отличать пакеты от библиотек, пакетам дают префикс `wfc_ `, например `wfc_demo `. Для облегчения подключения wfc и написания cmake скриптов будем использовать [cmake-ci](https://github.com/mambaru/cmake-ci). Изначально это была папка со скриптами в WFC проекте, но потом эволюционировала в отдельный, независимый проект.

### Иницализация cmake-ci

### Подключение WFC

### Пакет

### Модуль

### Компонент

### Прикладной объект

### Прикладная логика

### Сервис

### Шлюз


