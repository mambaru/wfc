#pragma once

#include <iow/logger/global_log.hpp>
#include <iow/logger/logger.hpp>

#define WFC_WRITE_LOG(NAME, TYPE, X) IOW_WRITE_LOG(NAME, TYPE,   X )

#define WFC_LOG_ERROR(NAME, X)    IOW_WRITE_LOG_ERROR( NAME, X )
#define WFC_LOG_WARNING(NAME,X)   IOW_WRITE_LOG_WARNING( NAME, X )
#define WFC_LOG_MESSAGE(NAME, X)  IOW_WRITE_LOG_MESSAGE( NAME, X )
#define WFC_LOG_FATAL(NAME, X)    IOW_WRITE_LOG_FATAL( NAME, X )
#define WFC_LOG_BEGIN(NAME, X)    IOW_WRITE_LOG_BEGIN( NAME, X )
#define WFC_LOG_END(NAME, X)      IOW_WRITE_LOG_END( NAME, X )
#define WFC_LOG_DEBUG(NAME, X)    IOW_WRITE_LOG_DEBUG( NAME, X )
#define WFC_LOG_TRACE(NAME, X)    IOW_WRITE_LOG_TRACE( NAME, X )
#define WFC_LOG_PROGRESS(NAME, X) IOW_WRITE_LOG_PROGRESS( NAME, X )

#define CONFIG_LOG_ERROR(X)    WFC_LOG_ERROR( "config", X )
#define CONFIG_LOG_WARNING(X)  WFC_LOG_WARNING( "config", X )
#define CONFIG_LOG_MESSAGE(X)  WFC_LOG_MESSAGE( "config", X )
#define CONFIG_LOG_FATAL(X)    WFC_LOG_FATAL( "config", X )
#define CONFIG_LOG_BEGIN(X)    WFC_LOG_BEGIN( "config", X )
#define CONFIG_LOG_END(X)      WFC_LOG_END( "config", X )
#define CONFIG_LOG_DEBUG(X)    WFC_LOG_DEBUG( "config", X )
#define CONFIG_LOG_TRACE(X)    WFC_LOG_TRACE( "config", X )
#define CONFIG_LOG_PROGRESS(X) WFC_LOG_PROGRESS( "config", X )

#define DOMAIN_LOG_ERROR(X)    WFC_LOG_ERROR( "domain", X )
#define DOMAIN_LOG_WARNING(X)  WFC_LOG_WARNING( "domain", X )
#define DOMAIN_LOG_MESSAGE(X)  WFC_LOG_MESSAGE( "domain", X )
#define DOMAIN_LOG_FATAL(X)    WFC_LOG_FATAL( "domain", X )
#define DOMAIN_LOG_BEGIN(X)    WFC_LOG_BEGIN( "domain", X )
#define DOMAIN_LOG_END(X)      WFC_LOG_END( "domain", X )
#define DOMAIN_LOG_DEBUG(X)    WFC_LOG_DEBUG( "domain", X )
#define DOMAIN_LOG_TRACE(X)    WFC_LOG_TRACE( "domain", X )
#define DOMAIN_LOG_PROGRESS(X) WFC_LOG_PROGRESS( "domain", X )

#define COMMON_LOG_ERROR(X)    WFC_LOG_ERROR( "common", X )
#define COMMON_LOG_WARNING(X)  WFC_LOG_WARNING( "common", X )
#define COMMON_LOG_MESSAGE(X)  WFC_LOG_MESSAGE( "common", X )
#define COMMON_LOG_FATAL(X)    WFC_LOG_FATAL( "common", X )
#define COMMON_LOG_BEGIN(X)    WFC_LOG_BEGIN( "common", X )
#define COMMON_LOG_END(X)      WFC_LOG_END( "common", X )
#define COMMON_LOG_DEBUG(X)    WFC_LOG_DEBUG( "common", X )
#define COMMON_LOG_TRACE(X)    WFC_LOG_TRACE( "common", X )
#define COMMON_LOG_PROGRESS(X) WFC_LOG_PROGRESS( "common", X )

#define SYSLOG_LOG_ERROR(X)    WFC_LOG_ERROR( "syslog", X )
#define SYSLOG_LOG_WARNING(X)  WFC_LOG_WARNING( "syslog", X )
#define SYSLOG_LOG_MESSAGE(X)  WFC_LOG_MESSAGE( "syslog", X )
#define SYSLOG_LOG_FATAL(X)    WFC_LOG_FATAL( "syslog", X )
#define SYSLOG_LOG_BEGIN(X)    WFC_LOG_BEGIN( "syslog", X )
#define SYSLOG_LOG_END(X)      WFC_LOG_END( "syslog", X )
#define SYSLOG_LOG_DEBUG(X)    WFC_LOG_DEBUG( "syslog", X )
#define SYSLOG_LOG_TRACE(X)    WFC_LOG_TRACE( "syslog", X )
#define SYSLOG_LOG_PROGRESS(X) WFC_LOG_PROGRESS( "syslog", X )

#ifndef NDEBUG

#define DEBUG_LOG_ERROR(X)    WFC_LOG_ERROR( "debug", X )
#define DEBUG_LOG_WARNING(X)  WFC_LOG_WARNING( "debug", X )
#define DEBUG_LOG_MESSAGE(X)  WFC_LOG_MESSAGE( "debug", X )
#define DEBUG_LOG_FATAL(X)    WFC_LOG_FATAL( "debug", X )
#define DEBUG_LOG_BEGIN(X)    WFC_LOG_BEGIN( "debug", X )
#define DEBUG_LOG_END(X)      WFC_LOG_END( "debug", X )
#define DEBUG_LOG_DEBUG(X)    WFC_LOG_DEBUG( "debug", X )
#define DEBUG_LOG_TRACE(X)    WFC_LOG_TRACE( "debug", X )
#define DEBUG_LOG_PROGRESS(X) WFC_LOG_PROGRESS( "debug", X )

#else

#define DEBUG_LOG_ERROR(X)
#define DEBUG_LOG_WARNING(X)
#define DEBUG_LOG_MESSAGE(X)
#define DEBUG_LOG_FATAL(X)
#define DEBUG_LOG_BEGIN(X)
#define DEBUG_LOG_END(X)
#define DEBUG_LOG_DEBUG(X)
#define DEBUG_LOG_TRACE(X)
#define DEBUG_LOG_PROGRESS(X)

#endif
