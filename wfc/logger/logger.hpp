#pragma once

#include <wlog/logging.hpp>
#include <wfc/wfc_exit.hpp>
#include <sstream>

#define WFC_LOG_ERROR(NAME, X)    WLOG_LOG_ERROR( NAME, X )
#define WFC_LOG_WARNING(NAME,X)   WLOG_LOG_WARNING( NAME, X )
#define WFC_LOG_MESSAGE(NAME, X)  WLOG_LOG_MESSAGE( NAME, X )
#define WFC_LOG_FATAL(NAME, X)    WLOG_LOG_FATAL( NAME, X ) 
//#define WFC_LOG_FATAL(NAME, X)    {WLOG_LOG_FATAL( NAME, X ) std::stringstream _l_ss_; _l_ss_ << X;  wfc_exit_with_error(_l_ss_.str());}
#define WFC_LOG_BEGIN(NAME, X)    WLOG_LOG_BEGIN( NAME, X )
#define WFC_LOG_END(NAME, X)      WLOG_LOG_END( NAME, X )
#define WFC_LOG_DEBUG(NAME, X)    WLOG_LOG_DEBUG( NAME, X )
#define WFC_LOG_TRACE(NAME, X)    WLOG_LOG_TRACE( NAME, X )
#define WFC_LOG_PROGRESS(NAME, X) WLOG_LOG_PROGRESS( NAME, X )

#define CONFIG_LOG_ERROR(X)    WFC_LOG_ERROR( "CONFIG", X )
#define CONFIG_LOG_WARNING(X)  WFC_LOG_WARNING( "CONFIG", X )
#define CONFIG_LOG_MESSAGE(X)  WFC_LOG_MESSAGE( "CONFIG", X )
#define CONFIG_LOG_FATAL(X)    WFC_LOG_FATAL( "CONFIG", X )
#define CONFIG_LOG_BEGIN(X)    WFC_LOG_BEGIN( "CONFIG", X )
#define CONFIG_LOG_END(X)      WFC_LOG_END( "CONFIG", X )
#define CONFIG_LOG_DEBUG(X)    WFC_LOG_DEBUG( "CONFIG", X )
#define CONFIG_LOG_TRACE(X)    WFC_LOG_TRACE( "CONFIG", X )
#define CONFIG_LOG_PROGRESS(X) WFC_LOG_PROGRESS( "CONFIG", X )

#define DOMAIN_LOG_ERROR(X)    WFC_LOG_ERROR( "DOMAIN", X )
#define DOMAIN_LOG_WARNING(X)  WFC_LOG_WARNING( "DOMAIN", X )
#define DOMAIN_LOG_MESSAGE(X)  WFC_LOG_MESSAGE( "DOMAIN", X )
#define DOMAIN_LOG_FATAL(X)    WFC_LOG_FATAL( "DOMAIN", X )
#define DOMAIN_LOG_BEGIN(X)    WFC_LOG_BEGIN( "DOMAIN", X )
#define DOMAIN_LOG_END(X)      WFC_LOG_END( "DOMAIN", X )
#define DOMAIN_LOG_DEBUG(X)    WFC_LOG_DEBUG( "DOMAIN", X )
#define DOMAIN_LOG_TRACE(X)    WFC_LOG_TRACE( "DOMAIN", X )
#define DOMAIN_LOG_PROGRESS(X) WFC_LOG_PROGRESS( "DOMAIN", X )

#define COMMON_LOG_ERROR(X)    WFC_LOG_ERROR( "COMMON", X )
#define COMMON_LOG_WARNING(X)  WFC_LOG_WARNING( "COMMON", X )
#define COMMON_LOG_MESSAGE(X)  WFC_LOG_MESSAGE( "COMMON", X )
#define COMMON_LOG_FATAL(X)    WFC_LOG_FATAL( "COMMON", X )
#define COMMON_LOG_BEGIN(X)    WFC_LOG_BEGIN( "COMMON", X )
#define COMMON_LOG_END(X)      WFC_LOG_END( "COMMON", X )
#define COMMON_LOG_DEBUG(X)    WFC_LOG_DEBUG( "COMMON", X )
#define COMMON_LOG_TRACE(X)    WFC_LOG_TRACE( "COMMON", X )
#define COMMON_LOG_PROGRESS(X) WFC_LOG_PROGRESS( "COMMON", X )

// TODO: перенести в wjrpc
#define JSONRPC_LOG_ERROR(X)    WLOG_LOG_ERROR( "JSON-RPC", X )
#define JSONRPC_LOG_WARNING(X)  WLOG_LOG_WARNING( "JSON-RPC", X )
#define JSONRPC_LOG_MESSAGE(X)  WLOG_LOG_MESSAGE( "JSON-RPC", X )
#define JSONRPC_LOG_FATAL(X)    WLOG_LOG_FATAL( "JSON-RPC", X )
#define JSONRPC_LOG_BEGIN(X)    WLOG_LOG_BEGIN( "JSON-RPC", X )
#define JSONRPC_LOG_END(X)      WLOG_LOG_END( "JSON-RPC", X )
#define JSONRPC_LOG_DEBUG(X)    WLOG_LOG_DEBUG( "JSON-RPC", X )
#define JSONRPC_LOG_TRACE(X)    WLOG_LOG_TRACE( "JSON-RPC", X )
#define JSONRPC_LOG_PROGRESS(X) WLOG_LOG_PROGRESS( "JSON-RPC", X )

#ifndef NDEBUG

#define DEBUG_LOG_ERROR(X)    WFC_LOG_ERROR( "DEBUG", X )
#define DEBUG_LOG_WARNING(X)  WFC_LOG_WARNING( "DEBUG", X )
#define DEBUG_LOG_MESSAGE(X)  WFC_LOG_MESSAGE( "DEBUG", X )
#define DEBUG_LOG_FATAL(X)    WFC_LOG_FATAL( "DEBUG", X )
#define DEBUG_LOG_BEGIN(X)    WFC_LOG_BEGIN( "DEBUG", X )
#define DEBUG_LOG_END(X)      WFC_LOG_END( "DEBUG", X )
#define DEBUG_LOG_DEBUG(X)    WFC_LOG_DEBUG( "DEBUG", X )
#define DEBUG_LOG_TRACE(X)    WFC_LOG_TRACE( "DEBUG", X )
#define DEBUG_LOG_PROGRESS(X) WFC_LOG_PROGRESS( "DEBUG", X )

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
