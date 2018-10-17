#pragma once

namespace wfc{
 
/**
 * @brief Интерфейс управления wfc::domain_object.
 * @details Методы этого интерфейса могут быть в прикладном объекте (с родительским wfc::domain_object) для 
 * того чтобы уточнить его поведения, на этапах конфигурирования, инициализации и запуска 
 */
class basic_domain
{
public:
  virtual ~basic_domain() {}
  
  /**
    * @brief create
    * @details create
    */
  virtual void create() {}

  /**
    * @brief create
    * @details create
    */
  virtual void configure() { this->reconfigure(); }

  /**
    * @brief create
    * @details create
    */
  virtual void initialize() {}

  /**
    * @brief create
    * @details create
    */
  virtual void start() { this->restart(); }

  /**
    * @brief create
    * @details create
    */
  virtual void restart() {};
  
  /**
    * @brief create
    * @details create
    */
  virtual void stop( )  {}

  /**
    * @brief create
    * @details create
    */
  virtual void reconfigure() { }

  /**
    * @brief create
    * @details create
    */
  virtual void reconfigure_basic() {}
};


}

