#pragma once

#include <string>
#include <fstream>
#include <ILog.h>
#include <filesystem>

#include "Flags.h"

namespace IRun {
	namespace Tools {

		enum struct IoFlags {
			None = 0x0ULL,
			CreateFile = 0x1ULL,
			Binary = 0x2ULL,
			Discard = 0x4ULL,
		};

		/// <summary>
		/// Read file and return as a string.
		/// </summary>
		/// <param name="filename">Path of the file that the function is going to read.</param>
		/// <param name="flags">These flags will be used when calling std::ifstream::open(). std::ios::ate is already defined to not use again.</param>
		/// <returns></returns>
		std::string ReadFile(const std::string& filename, IoFlags flags = IoFlags::None);
		std::wstring ReadFile(const std::wstring& filename, IoFlags flags = IoFlags::None);
		void WriteFile(const std::string& filename, const std::string& content, IoFlags flags = IoFlags::None);
		void WriteFile(const std::wstring& filename, const std::wstring& content, IoFlags flags = IoFlags::None);
		void WriteFile(const std::string& filename, const std::vector<char>& content, IoFlags flags = IoFlags::None);
		void WriteFile(const std::wstring& filename, const std::vector<wchar_t>& content, IoFlags flags = IoFlags::None);
	}
}

