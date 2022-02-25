#pragma once

#if defined(_WIN32)
#include "../../../simple_library/public/simple_library.h"

#define Engine_Log(format,...)			log_log(format,__VA_ARGS__)
#define Engine_Log_Success(format,...)	log_success(format,__VA_ARGS__)
#define Engine_Log_Error(format,...)	log_error(format,__VA_ARGS__)
#define Engine_Log_Warning(format,...)	log_warning(format,__VA_ARGS__)
#elif 0

#define Engine_Log(format,...) 
#define Engine_Log_Success(format,...) 
#define Engine_Log_Error(format,...) 
#define Engine_Log_Warning(format,...) 

#endif