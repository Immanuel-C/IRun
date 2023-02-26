#pragma once

#include <fstream>
#include <vector>

#include <ILog.h>

namespace IRun {
	namespace Utils {
		std::vector<char> ReadFile(const std::string& filename, int flags = 0);
	}
}