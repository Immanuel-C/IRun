#pragma once

#include <string>
#include <fstream>
#include <ILog.h>
#include <filesystem>

#include "Flags.h"

namespace IRun {
	namespace Tools {

		enum struct IoFlags {
			/// <summary>
			/// No flags provided.
			/// </summary>
			None = 0x0ULL,
			/// <summary>
			/// Create file if it doesn't exist. Only works when writing to a file.
			/// </summary>
			Create = 0x1ULL,
			/// <summary>
			/// Treat the data passed in as binary data. 
			/// </summary>
			Binary = 0x2ULL,
			/// <summary>
			/// Discard any data in a file already. Only works when writing to a file.
			/// </summary>
			Discard = 0x4ULL,
			/// <summary>
			/// Max value of IoFlags.
			/// </summary>
			Max
		};
		CREATE_FLAGS_FROM_ENUM_STRUCT(IoFlags, IoFlags::Max)


		std::string ReadFile(const std::string& filename, IoFlags flags = IoFlags::None);
		std::wstring ReadFile(const std::wstring& filename, IoFlags flags = IoFlags::None);
		void WriteFile(const std::string& filename, const std::string& content, IoFlags flags = IoFlags::None);
		void WriteFile(const std::wstring& filename, const std::wstring& content, IoFlags flags = IoFlags::None);
		void WriteFile(const std::string& filename, const std::vector<char>& content, IoFlags flags = IoFlags::None);
		void WriteFile(const std::wstring& filename, const std::vector<wchar_t>& content, IoFlags flags = IoFlags::None);
	}
}

