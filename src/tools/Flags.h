#pragma once

#include <ILog.h>

#include <cstdint>

/// <summary>
/// Allow bitwise and/or operators to be used by provided enum struct.
/// All flags must be of type int64_t.
/// </summary>
/// <param name="Struct">Struct to create operators for.</param>
/// <param name="Struct">The max value of the struct.</param>
#define CREATE_FLAGS_FROM_ENUM_STRUCT(Struct, MaxValue)																	\
		inline Struct operator|(Struct left, Struct right) {														\
			I_DEBUG_ASSERT_FATAL_ERROR(left > MaxValue || right > MaxValue, "constexpr int64_t operator|(%s left, %s right): Enum Struct %s max value is %s (%i) but left (%i) or right (%i) is higher than max value", #Struct, #Struct, #Struct, #MaxValue, (int)MaxValue, (int)left, (int)right);											\
			return (Struct)((int64_t)(left) | (int64_t)(right));																	\
		}																	\
		inline Struct operator&(Struct left, Struct right) {														\
			I_DEBUG_ASSERT_FATAL_ERROR(left > MaxValue || right > MaxValue, "constexpr int64_t operator&(%s left, %s right): Enum Struct %s max value is %s (%i) but left (%i) or right (%i) is higher than max value", #Struct, #Struct, #Struct, #MaxValue, (int)MaxValue, (int)left, (int)right);											\
			return (Struct)((int64_t)(left) & (int64_t)(right));																	\
		}					
