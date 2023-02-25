#pragma once

#include <fstream>
#include <string>

#include <ILog.h>

namespace IRun {
	namespace Utils {
		std::string ReadFile(const std::string& filename, int flags = 0) {
			std::ifstream file{ filename, std::ios::ate | flags };

			if (!file.is_open()) {
				I_LOG_ERROR("Failed to open file: %s", filename.c_str());
			}

			// tellg() = size of file
			std::streampos fileSize = file.tellg();
			std::string contents{};
			contents.resize(fileSize);
			// Go the the start of the file
			file.seekg(0);
			file.read(contents.data(), fileSize);
			file.close();

			return contents;
		}
	}
}