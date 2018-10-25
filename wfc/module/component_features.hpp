//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

namespace wfc{

/**
 * @brief Дополнительные особенности компонента
 * @details с помощью этого набора флагов можно определять некоторые особенности компонента:
 * - скрыть поля enabled, startup_priority, shutdown_priority, suspend и workflow в json-конфигурации wfc::basic_instance_options
 * - включить поле в json-конфигурации wfc::basic_instance_options
 * - заставить игнорировать реконфигурацию компонента 
 *
 * Включение/отключение полей работает только в json-конфигурации, но пользователю доступны значения по умолчанию. Это имеет смысл 
 * если какие-то поля ни на что не влияют, например если не важен приоритет запуска или не поддерживается режим suspend(имитации). 
 * [workflow](https://mambaru.github.io/wflow/) прикладному объекту доступен всегда, и если он не используется или используется 
 * для простых заданий, то его тоже можно отключить, чтобы не вводить в заблуждение пользователя (при наличии этого поля возможно 
 * настроить отдельный [workflow](https://mambaru.github.io/wflow/) для объекта, но на слабых нагрузках это бессмысленная трата времени)
 * 
 * Поле cpu по умолчанию отключено из предположения, что в большинстве случаев [workflow](https://mambaru.github.io/wflow/) будет 
 * достаточно, его можно более гибко настраивать, но если в хотите запускать свои потоки и получить возможность распределять их по ядрам CPU 
 * то помимо EnableCPU вам необходимо зарегистрировать потоки с помощью domain_object::reg_thread
 */  
struct component_features
{
  enum {
    Defaults = 0,           ///< По умолчанию 
    Multiton = 0,           ///< Выставляетяся в wfc::multiton 
    Singleton = 1,          ///< Выставляетяся в wfc::singleton
    DisabledEnabled = 2,    ///< Скрыть поле enabled в json-конфигурации 
    DisabledPriority = 4,   ///< Скрыть поля startup_priority и shutdown_priority в json-конфигурации 
    DisabledSuspend  = 8,   ///< Скрыть поле suspend в json-конфигурации 
    DisabledWorkflow  = 16, ///< Скрыть поле workflow в json-конфигурации 
    EnableCPU = 32,         ///< Включить поле cpu в json-конфигурации 
    IgnoreReconfigure  = 64,///< Игнорировать сигналы реконфигурацию 
    EnableTracking = 128    ///< Включить дефолтную реализацию обертки iinterface для отслеживания источника
  };
};

}
