#pragma once

#include <wfc/logger/global_log.hpp>

#define WRITE_LOG(C, E, X) ::wfc::global_log(C, E) << X << std::endl;

#define COMMON_LOG_PROGRESS( X ) ::wfc::global_log("common", "progress") << X << "\r";

#define CONFIG_LOG_ERROR( X )         WRITE_LOG("config", "error", X )
#define CONFIG_LOG_WARNING( X )       WRITE_LOG("config", "warning", X )
#define CONFIG_LOG_MESSAGE( X )       WRITE_LOG("config", "message", X )
#define CONFIG_LOG_FATAL( X )         WRITE_LOG("config", "fatal", X )
#define CONFIG_LOG_BEGIN( X )         WRITE_LOG("config", "begin", X )
#define CONFIG_LOG_END( X )           WRITE_LOG("config", "end", X )
#define CONFIG_LOG_BEGIN_PROCESS( X ) CONFIG_LOG_BEGIN( X )
#define CONFIG_LOG_END_PROCESS( X )   CONFIG_LOG_END( X )

#define DAEMON_LOG_ERROR( X )         WRITE_LOG("daemon", "error", X )
#define DAEMON_LOG_WARNING( X )       WRITE_LOG("daemon", "warning", X )
#define DAEMON_LOG_MESSAGE( X )       WRITE_LOG("daemon", "message", X )
#define DAEMON_LOG_FATAL( X )         WRITE_LOG("daemon", "fatal", X )
#define DAEMON_LOG_BEGIN( X )         WRITE_LOG("daemon", "begin", X )
#define DAEMON_LOG_END( X )           WRITE_LOG("daemon", "end", X )
#define DAEMON_LOG_BEGIN_PROCESS( X ) DAEMON_LOG_BEGIN( X )
#define DAEMON_LOG_END_PROCESS( X )   DAEMON_LOG_END( X )

#define COMMON_LOG_ERROR( X )         WRITE_LOG("common", "error", X )
#define COMMON_LOG_WARNING( X )       WRITE_LOG("common", "warning", X )
#define COMMON_LOG_MESSAGE( X )       WRITE_LOG("common", "message", X )
#define COMMON_LOG_FATAL( X )         WRITE_LOG("common", "fatal", X )
#define COMMON_LOG_BEGIN( X )         WRITE_LOG("common", "begin", X )
#define COMMON_LOG_END( X )           WRITE_LOG("common", "end", X )
#define COMMON_LOG_BEGIN_PROCESS( X ) COMMON_LOG_BEGIN( X )
#define COMMON_LOG_END_PROCESS( X )   COMMON_LOG_END( X )

#ifndef NDEBUG

#define DEBUG_LOG_ERROR( X )          WRITE_LOG("debug", "error", X )
#define DEBUG_LOG_WARNING( X )        WRITE_LOG("debug", "warning", X )
#define DEBUG_LOG_MESSAGE( X )        WRITE_LOG("debug", "message", X )
#define DEBUG_LOG_FATAL( X )          WRITE_LOG("debug", "fatal", X )
#define DEBUG_LOG_BEGIN( X )          WRITE_LOG("debug", "begin", X )
#define DEBUG_LOG_END( X )            WRITE_LOG("debug", "end", X )
#define DEBUG_LOG_BEGIN_PROCESS( X )  DEBUG_LOG_BEGIN( X )
#define DEBUG_LOG_END_PROCESS( X )    DEBUG_LOG_END( X )

#define TRACE_LOG_ERROR( X )          WRITE_LOG("trace", "error", X )
#define TRACE_LOG_WARNING( X )        WRITE_LOG("trace", "warning", X )
#define TRACE_LOG_MESSAGE( X )        WRITE_LOG("trace", "message", X )
#define TRACE_LOG_FATAL( X )          WRITE_LOG("trace", "fatal", X )
#define TRACE_LOG_BEGIN( X )          WRITE_LOG("trace", "begin", X )
#define TRACE_LOG_END( X )            WRITE_LOG("trace", "end", X )
#define TRACE_LOG_BEGIN_PROCESS( X )  TRACE_LOG_BEGIN( X )
#define TRACE_LOG_END_PROCESS( X )    TRACE_LOG_END( X )


#else

#define DEBUG_LOG_ERROR( X )
#define DEBUG_LOG_WARNING( X )
#define DEBUG_LOG_MESSAGE( X )
#define DEBUG_LOG_FATAL( X )
#define DEBUG_LOG_BEGIN( X )
#define DEBUG_LOG_END( X )
#define DEBUG_LOG_BEGIN_PROCESS( X )
#define DEBUG_LOG_END_PROCESS( X )

#define TRACE_LOG_ERROR( X )
#define TRACE_LOG_WARNING( X )
#define TRACE_LOG_MESSAGE( X )
#define TRACE_LOG_FATAL( X )
#define TRACE_LOG_BEGIN( X )
#define TRACE_LOG_END( X )
#define TRACE_LOG_BEGIN_PROCESS( X )
#define TRACE_LOG_END_PROCESS( X )

#endif
