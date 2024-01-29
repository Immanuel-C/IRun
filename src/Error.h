#pragma once

#include "tools/Flags.h"
#include <string>

namespace IRun {
	enum struct ErrorCode {
		Success = 0x0ULL,
		IoError = 0x1ULL,
		Corrupt = 0x2ULL,
		Max
	};
	CREATE_FLAGS_FROM_ENUM_STRUCT(ErrorCode, ErrorCode::Max);

	inline std::string ErrorCodeToString(ErrorCode err) {
		switch (err)
		{
		case IRun::ErrorCode::Success:
			return "IRun::ErrorCode::Success";
			break;
		case IRun::ErrorCode::IoError:
			return "IRun::ErrorCode::IoError";
			break;
		case IRun::ErrorCode::Corrupt:
			return "IRun::ErrorCode::Corrupt";
			break;
		default:
			return "Invalid Error Code!";
			break;
		}
	}
}


#ifdef DEBUG 
/// <summery>
/// Checks a function that returns IRun::ErrorCode and see's if it returns IRun::ErrorCode::Success, if not log and error and exit application.
/// Only works in debug mode. This macro is empty in release mode.
/// </summery>
#define IRUN_CHECK(fun, s)																												\
				{																														\
					IRun::ErrorCode result = fun;																						\
					if (!(x & IRun::ErrroCode::Success)) {																				\
						I_LOG_FATAL_ERROR("IRun function failed! Message:%s\nError: %s", s, IRun::ErrorCodeToString(result).c_str());	\
						exit(EXIT_FAILURE);																								\
					}																													\
				}
#else
#define IRUN_CHECK(fun, s) fun;
#endif
