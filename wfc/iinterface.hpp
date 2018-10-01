//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/io/types.hpp>

namespace wfc{

/**
 * @brief Базовый интерфейс для прикладных (domain) объектов
 * @details Используется для регистрации в глобальном реестре объектов и предоставляет методы для обмена 
 * данными в сыром виде.  
 */
struct iinterface
{
  /** Тип сырых данных ```cpp std::vector<char>``` */
  typedef ::iow::io::data_type data_type;
  /** Буфер для сырых данных ```cpp std::unique_ptr<data_type>``` */
  typedef ::iow::io::data_ptr  data_ptr;
  /** Уникальный идентификатор объекта */
  typedef ::iow::io::io_id_t   io_id_t;
  /** Обработчик обратного вызова, для отправки ответа */
  typedef ::iow::io::output_handler_t output_handler_t;
  /** Входящий обработчик (аналогичен iinterface::perform_io) */
  typedef ::iow::io::input_handler_t input_handler_t;
 
  virtual ~iinterface() {}

  /**
   * @brief Регистрация объекта
   * @param io_id Идентификатор объекта
   * @param itf Указатель на объект с идентификатором io_id
   * @details Любой объект системы может уведомить этот объект, что получил на него ссылку, а так-же 
   * передать указатель на себя. Например, когда клиент подключается к системе, создается объект TCP-соединения, 
   * который может уведомить прикладной объект, о том, что клиент подключился, и передать указатель на себя, 
   * через который прикладной объект может передать данные клиенту, не дожидаясь запроса от него.
   */
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf);

  /**
   * @brief Уведомление о прекращении использования
   * @param io_id Идентификатор объекта
   * @details 
   */
  virtual void unreg_io(io_id_t io_id);

  /**
   * @brief Обработка сырых данных 
   * @param d - входящий буфер 
   * @param io_id - идентификатор исходящего объекта
   * @param handler - обработчик результата запроса 
   * @details 
   */
  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler);
};

}
