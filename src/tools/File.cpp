#include "File.h"

namespace IRun {
	namespace Tools {
		CREATE_FLAGS_FROM_ENUM_STRUCT(IoFlags)

		static int ConvertIoFlagsToStlFlags(IoFlags flags) {
			int finalFlags = 0;
			if (flags & IoFlags::Binary) finalFlags |= std::ios::binary;
			if (flags & IoFlags::Discard) finalFlags |= std::ios::trunc;
			return finalFlags;
		}

		std::string ReadFile(const std::string& filename, IoFlags flags)
		{
			std::ifstream file{};

			file.open(filename, std::ios::ate | ConvertIoFlagsToStlFlags(flags));

			if (!file.is_open()) {
				I_LOG_ERROR("Failed to open file: %s", filename.c_str());
				file.close();
				return "";
			}

			std::string content{};
			content.resize((size_t)file.tellg());
			
			file.seekg(std::ios_base::beg);
			file.read((char*)content.c_str(), content.size());
			file.close();

			return content;
		}

		std::wstring ReadFile(const std::wstring& filename, IoFlags flags)
		{
			std::wifstream file{};

			file.open(filename, std::ios::ate | ConvertIoFlagsToStlFlags(flags));

			if (!file.is_open()) {
				I_LOG_ERROR("Failed to open file: %s", filename.c_str());
				file.close();
				return L"";
			}

			std::wstring content{};
			content.resize((size_t)file.tellg());

			file.seekg(std::ios_base::beg);
			file.read((wchar_t*)content.c_str(), content.size());
			file.close();

			return content;
		}

		void WriteFile(const std::string& filename, const std::string& content, IoFlags flags) {
			WriteFile(filename, std::vector<char>{ content.begin(), content.end() }, flags);
		}

		void WriteFile(const std::wstring& filename, const std::wstring& content, IoFlags flags) {
			WriteFile(filename, std::vector<wchar_t>{ content.begin(), content.end() }, flags);
		}

		void WriteFile(const std::string& filename, const std::vector<char>& content, IoFlags flags) {
			std::fstream file{};

			if (!(flags & IoFlags::CreateFile)) {
				file.open(filename, std::ios::in);

				if (!file.is_open()) {
					I_LOG_ERROR("Failed to open file: %s", filename.c_str());
					file.close();
					return;
				}
			}

			file.open(filename, std::ios::out | ConvertIoFlagsToStlFlags(flags));

			try {
				file.write(content.data(), content.size() * sizeof(const char));
			}
			catch (std::exception e) {
				I_LOG_ERROR("WriteFile exception: %s", e.what());
			}

			file.close();
		}


		void WriteFile(const std::wstring& filename, const std::vector<wchar_t>& content, IoFlags flags) {
			std::wfstream file{};

			if (!(flags & IoFlags::CreateFile)) {
				file.open(filename, std::ios::in);

				if (!file.is_open()) {
					I_LOG_ERROR("Failed to open file: %s", filename.c_str());
					file.close();
					return;
				}
			}

			file.open(filename, std::ios::out | ConvertIoFlagsToStlFlags(flags));

			file.write(content.data(), content.size() * sizeof(const wchar_t));

			file.close();
		}
	}
}
