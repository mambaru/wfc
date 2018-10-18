WFC - это фреймворк для разработки высконагруженных JSON-RPC сервисов (демонов) под различные профили нагрузки. Данный проект является частью этого фреймворка, но не им самим. 
Если вас интересует как быстро сделать свой первый проект на базе этого фреймворка обратитесь к документации [wfcroot](https://github.com/mambaru/wfcroot) или изучите 
[Демо-проект](https://github.com/mambaru/demod).

Документация [doxygen](https://mambaru.github.io/wfc/index.html).
Репозитарий на [github.com](https://github.com/mambaru/wfc).

# Сборка и установка

Для разработки собственных проектов вам нужен супермодуль [wfcroot](https://github.com/mambaru/wfcroot), который включает все необходимое, непосредственно скачивать и 
компилировать из этого репозитария нет необходимости. Но это можно сделать:

```bash
git clone https://github.com/mambaru/wfc.git
cd wfc
mkdir build
cd build
cmake -DBUILD_TESTING=ON ..
cmake --build .
ctest 
```

Если система сборки автоматически загрузит в директорию сборки ( если не найдет их в системе или на том же уровне в директории) следующие библиотеки: 
- [faslib](https://github.com/migashko/faslib) - метабиблиотека для работы со списками типов и АОП
- [wjson](https://github.com/mambaru/wjson) - очень быстрый декларативный JSON-сериализатор на шаблонах
- [wlog](https://github.com/mambaru/wlog) - логер с большими возможностями по кастомизации 
- [wflow](https://github.com/mambaru/wflow) - очереди и потоки 
- [iow](https://github.com/mambaru/iow) - сетевая библиотека
- [wjrpc](https://github.com/mambaru/wjrpc) - библиотека поддержки JSON-RPC
- [wrtstat](https://github.com/mambaru/wrtstat) - для сбора статистики


# Общая информация

Фреймворк позволяет за короткое время разработать высокопроизводительный демон, работающий по протоколу JSON-RPC, который может также, в асинхронном режиме, взаимодействовать с другими сервисами. Пропускная способность системы на одном хосте может достигать 100 тысяч JSON-RPC сообщений в секунду. Фреймворк избавляет программиста от необходимости взаимодействовать с сетевой подсистемой, а реализация JSON-RPC происходит на декларативном уровне, что позволяет сконцентрироваться исключительно на прикладной логике. Система конфигурации позволяет настроить демон для работы с различными профилями нагрузки. Например большие тяжелы запросы, очень много мелких запросов с коротким временем ответа или же смесь их, большой поток сообщений по одному каналу.

Большая часть функционала фремворка подключается в виде пакетов модулей. Из "коробки" [wfcroot](https://github.com/mambaru/wfcroot) вы получаете:
- [Модули ядра](https://github.com/mambaru/wfc_core):
  - *startup*
  - *config*
  - *core*
  - *logger*
  - *workflow*
- [Работа с сетью](https://github.com/mambaru/wfc_io):
  - *server-tcp*
  - *server-udp*
  - *client-tcp*
  - *io-queue*
- [Поддержка JSON-RPC](https://github.com/mambaru/wfc_jsonrpc):
  - *jsonrpc-queue
  - *jsonrpc-broker
  - *jsonrpc-repli
  - *jsonrpc-hub
  - *jsonrpc-batch
  - *jsonrpc-backlog
