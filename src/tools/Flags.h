#pragma once


#include <cstdint>

// All flags must be of type int64_t

#define CREATE_FLAGS_FROM_ENUM_STRUCT(Struct)																	\
		int64_t operator|(Struct left, Struct right) { return (int64_t)(left) | (int64_t)(right); }			\
		int64_t operator&(Struct left, Struct right) { return (int64_t)(left) & (int64_t)(right); }			

