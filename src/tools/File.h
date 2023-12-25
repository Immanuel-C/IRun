#pragma once

#include <string>
#include <fstream>
#include <ILog.h>

namespace IRun {
	namespace Tools {
		/// <summary>
		/// Read file and return as a string.
		/// </summary>
		/// <param name="filename">Path of the file that the function is going to read.</param>
		/// <param name="flags">These flags will be used when calling std::ifstream::open(). std::ios::ate is already defined to not use again.</param>
		/// <returns></returns>
		std::string ReadFile(const std::string& filename, int flags = 0);
	}
}

