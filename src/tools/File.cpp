#include "File.h"

namespace IRun {
	namespace Tools {

		std::string ReadFile(const std::string& filename, int flags)
		{
			std::ifstream file{};
			file.open(filename, std::ios::ate | flags);

			if (!file.is_open()) {
				I_LOG_ERROR("Failed to open file: %s", filename.c_str());
				return "";
			}

			std::string content{};
			content.resize((size_t)file.tellg());
			
			file.seekg(std::ios_base::beg);
			file.read((char*)content.c_str(), content.size());
			file.close();

			return content;
		}
	}
}
