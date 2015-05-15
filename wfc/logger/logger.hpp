#pragma once

#include <iow/logger/global_log.hpp>

#define CONFIG_LOG_ERROR(X)    IOW_LOG_ERROR( "config", X )
#define CONFIG_LOG_WARNING(X)  IOW_LOG_WARNING( "config", X )
#define CONFIG_LOG_MESSAGE(X)  IOW_LOG_MESSAGE( "config", X )
#define CONFIG_LOG_FATAL(X)    IOW_LOG_FATAL( "config", X )
#define CONFIG_LOG_BEGIN(X)    IOW_LOG_BEGIN( "config", X )
#define CONFIG_LOG_END(X)      IOW_LOG_END( "config", X )
#define CONFIG_LOG_DEBUG(X)    IOW_LOG_DEBUG( "config", X )
#define CONFIG_LOG_TRACE(X)    IOW_LOG_TRACE( "config", X )
#define CONFIG_LOG_PROGRESS(X) IOW_LOG_PROGRESS( "config", X )

#define DOMAIN_LOG_ERROR(X)    IOW_LOG_ERROR( "domain", X )
#define DOMAIN_LOG_WARNING(X)  IOW_LOG_WARNING( "domain", X )
#define DOMAIN_LOG_MESSAGE(X)  IOW_LOG_MESSAGE( "domain", X )
#define DOMAIN_LOG_FATAL(X)    IOW_LOG_FATAL( "domain", X )
#define DOMAIN_LOG_BEGIN(X)    IOW_LOG_BEGIN( "domain", X )
#define DOMAIN_LOG_END(X)      IOW_LOG_END( "domain", X )
#define DOMAIN_LOG_DEBUG(X)    IOW_LOG_DEBUG( "domain", X )
#define DOMAIN_LOG_TRACE(X)    IOW_LOG_TRACE( "domain", X )
#define DOMAIN_LOG_PROGRESS(X) IOW_LOG_PROGRESS( "domain", X )

#define COMMON_LOG_ERROR(X)    IOW_LOG_ERROR( "common", X )
#define COMMON_LOG_WARNING(X)  IOW_LOG_WARNING( "common", X )
#define COMMON_LOG_MESSAGE(X)  IOW_LOG_MESSAGE( "common", X )
#define COMMON_LOG_FATAL(X)    IOW_LOG_FATAL( "common", X )
#define COMMON_LOG_BEGIN(X)    IOW_LOG_BEGIN( "common", X )
#define COMMON_LOG_END(X)      IOW_LOG_END( "common", X )
#define COMMON_LOG_DEBUG(X)    IOW_LOG_DEBUG( "common", X )
#define COMMON_LOG_TRACE(X)    IOW_LOG_TRACE( "common", X )
#define COMMON_LOG_PROGRESS(X) IOW_LOG_PROGRESS( "common", X )
