#pragma once

#include <comet/logger/logger.hpp>

#ifndef NDEBUG

#define CONFIG_LOG_ERROR( X ) ::mamba::comet::glogger("config", "error") << X << std::endl
#define CONFIG_LOG_WARNING( X ) ::mamba::comet::glogger("config", "warning") << X << std::endl
#define CONFIG_LOG_MESSAGE( X ) ::mamba::comet::glogger("config", "message") << X << std::endl
#define CONFIG_LOG_FATAL( X ) ::mamba::comet::glogger("config", "fatal") << X << std::endl
#define CONFIG_LOG_BEGIN_PROCESS( X ) ::mamba::comet::glogger("config", "begin_process") << X << std::endl
#define CONFIG_LOG_END_PROCESS( X ) ::mamba::comet::glogger("config", "end_process") << X << std::endl

#define DAEMON_LOG_ERROR( X ) ::mamba::comet::glogger("daemon", "error") << X << std::endl
#define DAEMON_LOG_WARNING( X ) ::mamba::comet::glogger("daemon", "warning") << X << std::endl
#define DAEMON_LOG_MESSAGE( X ) ::mamba::comet::glogger("daemon", "message") << X << std::endl
#define DAEMON_LOG_FATAL( X ) ::mamba::comet::glogger("daemon", "fatal") << X << std::endl
#define DAEMON_LOG_BEGIN_PROCESS( X ) ::mamba::comet::glogger("daemon", "begin_process") << X << std::endl
#define DAEMON_LOG_END_PROCESS( X ) ::mamba::comet::glogger("daemon", "end_process") << X << std::endl

#define COMMON_LOG_ERROR( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::error>() << "COMMON: " << X;
#define COMMON_LOG_WARNING( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::warning>() << "COMMON: " << X;
#define COMMON_LOG_MESSAGE( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::message>() << "COMMON: " << X;
#define COMMON_LOG_FATAL( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::fatal>() << "COMMON: " << X;
#define COMMON_LOG_BEGIN_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::begin_process>() << "COMMON: " << X;
#define COMMON_LOG_END_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::end_process>() << "COMMON: " << X;

#define DEBUG_LOG_ERROR( X ) ::mamba::comet::logger< ::mamba::comet::_debug_, ::mamba::comet::log::error>() << "DEBUG: " << X;
#define DEBUG_LOG_WARNING( X ) ::mamba::comet::logger< ::mamba::comet::_debug_, ::mamba::comet::log::warning>() << "DEBUG: " << X;
#define DEBUG_LOG_MESSAGE( X ) ::mamba::comet::logger< ::mamba::comet::_debug_, ::mamba::comet::log::message>() << "DEBUG: " << X;
#define DEBUG_LOG_FATAL( X ) ::mamba::comet::logger< ::mamba::comet::_debug_, ::mamba::comet::log::fatal>() << "DEBUG: " << X;
#define DEBUG_LOG_BEGIN_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_debug_, ::mamba::comet::log::begin_process>() << "DEBUG: " <<  X;
#define DEBUG_LOG_END_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_debug_, ::mamba::comet::log::end_process>() << "DEBUG: " << X;

#define TRACE_LOG_ERROR( X ) ::mamba::comet::logger< ::mamba::comet::_trace_, ::mamba::comet::log::error>() << "TRACE: " <<  X;
#define TRACE_LOG_WARNING( X ) ::mamba::comet::logger< ::mamba::comet::_trace_, ::mamba::comet::log::warning>() << "TRACE: " << X;
#define TRACE_LOG_MESSAGE( X ) ::mamba::comet::logger< ::mamba::comet::_trace_, ::mamba::comet::log::message>() << "TRACE: " << X;
#define TRACE_LOG_FATAL( X ) ::mamba::comet::logger< ::mamba::comet::_trace_, ::mamba::comet::log::fatal>() << "TRACE: " << X;
#define TRACE_LOG_BEGIN_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_trace_, ::mamba::comet::log::begin_process>() << "TRACE: " << X;
#define TRACE_LOG_END_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_trace_, ::mamba::comet::log::end_process>() << "TRACE: " << X;


#else

#define DAEMON_LOG_ERROR( X ) mamba::comet::logger< ::mamba::comet::_daemon_, ::mamba::comet::log::error>() << "DAEMON: " << X;
#define DAEMON_LOG_WARNING( X ) ::mamba::comet::logger< ::mamba::comet::_daemon_, ::mamba::comet::log::warning>() << "DAEMON: " << X;
#define DAEMON_LOG_MESSAGE( X ) ::mamba::comet::logger< ::mamba::comet::_daemon_, ::mamba::comet::log::message>() << "DAEMON: " << X;
#define DAEMON_LOG_FATAL( X ) ::mamba::comet::logger< ::mamba::comet::_daemon_, ::mamba::comet::log::fatal>() << "DAEMON: " << X;
#define DAEMON_LOG_BEGIN_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_daemon_, ::mamba::comet::log::begin_process>() << "DAEMON: " << X;
#define DAEMON_LOG_END_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_daemon_, ::mamba::comet::log::end_process>() << "DAEMON: " << X;

#define COMMON_LOG_ERROR( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::error>() << "COMMON: " << X;
#define COMMON_LOG_WARNING( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::warning>() << "COMMON: " << X;
#define COMMON_LOG_MESSAGE( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::message>() << "COMMON: " << X;
#define COMMON_LOG_FATAL( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::fatal>() << "COMMON: " << X;
#define COMMON_LOG_BEGIN_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::begin_process>() << "COMMON: " << X;
#define COMMON_LOG_END_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_common_, ::mamba::comet::log::end_process>() << "COMMON: " << X;

#define DEBUG_LOG_ERROR( X )
#define DEBUG_LOG_WARNING( X )
#define DEBUG_LOG_MESSAGE( X )
#define DEBUG_LOG_FATAL( X )
#define DEBUG_LOG_BEGIN_PROCESS( X )
#define DEBUG_LOG_END_PROCESS( X )

#define TRACE_LOG_ERROR( X )
#define TRACE_LOG_WARNING( X )
#define TRACE_LOG_MESSAGE( X )
#define TRACE_LOG_FATAL( X )
#define TRACE_LOG_BEGIN_PROCESS( X )
#define TRACE_LOG_END_PROCESS( X )

#define CONFIG_LOG_ERROR( X ) ::mamba::comet::logger< ::mamba::comet::_config_, ::mamba::comet::log::error>() << "CONFIG: " << X;
#define CONFIG_LOG_WARNING( X ) ::mamba::comet::logger< ::mamba::comet::_config_, ::mamba::comet::log::warning>() << "CONFIG: " << X;
#define CONFIG_LOG_MESSAGE( X ) ::mamba::comet::logger< ::mamba::comet::_config_, ::mamba::comet::log::message>() << "CONFIG: " << X;
#define CONFIG_LOG_FATAL( X ) ::mamba::comet::logger< ::mamba::comet::_config_, ::mamba::comet::log::fatal>() << "CONFIG: " << X;
#define CONFIG_LOG_BEGIN_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_config_, ::mamba::comet::log::begin_process>() << "CONFIG: " << X;
#define CONFIG_LOG_END_PROCESS( X ) ::mamba::comet::logger< ::mamba::comet::_config_, ::mamba::comet::log::end_process>() << "CONFIG: " << X;

#endif
