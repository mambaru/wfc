#pragma once

#include <wlog/logging.hpp>

#define LOG_NAME "TANK"
#define TANK_LOG(TYPE, X)    WLOG(LOG_NAME, TYPE, X)
#define TANK_LOG_ERROR(X)    WLOG_LOG_ERROR( LOG_NAME, X )
#define TANK_LOG_WARNING(X)  WLOG_LOG_WARNING( LOG_NAME, X )
#define TANK_LOG_MESSAGE(X)  WLOG_LOG_MESSAGE( LOG_NAME, X )
#define TANK_LOG_FATAL(X)    WLOG_LOG_FATAL( LOG_NAME, X )
#define TANK_LOG_BEGIN(X)    WLOG_LOG_BEGIN( LOG_NAME, X )
#define TANK_LOG_END(X)      WLOG_LOG_END( LOG_NAME, X )
#define TANK_LOG_DEBUG(X)    WLOG_LOG_DEBUG( LOG_NAME, X )
#define TANK_LOG_TRACE(X)    WLOG_LOG_TRACE( LOG_NAME, X )
#define TANK_LOG_PROGRESS(X) WLOG_LOG_PROGRESS( LOG_NAME, X )
