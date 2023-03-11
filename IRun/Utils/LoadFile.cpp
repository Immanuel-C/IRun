#include "LoadFile.h"

namespace IRun {
	namespace Utils {
		std::vector<char> ReadFile(const std::string& filename, int flags) {
			std::ifstream file;

			if (flags != 0)
				file = std::ifstream{ filename, std::ios::ate | flags };
			else 
				file = std::ifstream{ filename, std::ios::ate };

			if (!file.is_open()) 
				I_LOG_ERROR("Failed to open file: %s", filename.c_str());
			

			// tellg() = size of file
			size_t fileSize = file.tellg();
			std::vector<char> buffer{};
			buffer.resize(fileSize);
			// Go the the start of the file
			file.seekg(0);
			file.read(buffer.data(), fileSize);
			file.close();

			return buffer;
		}
	}
}