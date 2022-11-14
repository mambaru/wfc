//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/object_registry.hpp>
#include <wfc/core/fire_list.hpp>
#include <wfc/core/extended_args.hpp>
#include <wfc/core/ibuild_info.hpp>
#include <wfc/core/workflow.hpp>
#include <wfc/core/cpuset.hpp>
#include <wfc/asio.hpp>
#include <vector>
#include <atomic>

namespace wfc{

struct wfcglobal
{
  typedef std::shared_ptr<wfcglobal> ptr;
  typedef std::function<bool()> fire_handler;
  typedef fire_list< fire_handler > fakir;
  typedef boost::asio::io_context io_context_type;
  typedef std::function<void(const std::string&)> callback_handler_t;

  std::string program_name;
  std::string program_path;
  std::string config_name;
  std::string config_path;
  std::string instance_name;
  std::string working_directory;
  bool despace = false;

  std::shared_ptr<ibuild_info> program_build_info;
  std::shared_ptr<ibuild_info> wfc_build_info;

  extended_args args;
  std::vector<std::string> ini;

  fakir idle;
  fakir after_start;
  fakir before_stop;
  fakir after_stop;

  std::atomic_bool disable_statistics;

  callback_handler_t nocall_handler;
  callback_handler_t doublecall_handler;

  cpuset cpu;
  object_registry registry;

  io_context_type& io_context;
  std::shared_ptr< wflow::workflow > common_workflow;
  static ptr static_global;
  std::atomic_bool stop_signal_flag;

  explicit wfcglobal( io_context_type& io_context);
  virtual ~wfcglobal();
  virtual void clear();

  /**
   * @brief находит файл или директорию для конфигурации
   * @param filename путь к файлу или дериктории
   * @return  Для относительных путей ищет сначала в директории конфигурации, затем в рабочей
   * директоии и директории запуска.Возвращает полный путь или пустую строку,
   * если файл или директория не существуют.
   */
  std::string find_config(const std::string& filename) const;

  /**
   * @brief находит файл или директорию по рабочим путям
   * @param filename путь к файлу или дериктории
   * @return  Для относительных путей ищет сначала в рабочей директоии и директории запускf.
   * Возвращает полный путь или пустую строку, если файл или директория не существуют.
   */
  std::string find_working(const std::string& filename) const;

  std::string make_directory(const std::string& pathname, std::string* err ) const;

  std::string make_directory_for_file(const std::string& filename, std::string* err) const;

};

}
