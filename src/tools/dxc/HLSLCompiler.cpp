#include "HLSLCompiler.h"


namespace IRun {
	namespace Tools {
		namespace DXC {
			std::array<CComPtr<IDxcBlob>, 2> CompileHLSLtoSPRIV(const std::string& vertShaderFilename, const std::string& fragmentShaderFilename)
			{
				HRESULT result;

				// Init DXC
				CComPtr<IDxcLibrary> library;
				result = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
				if (FAILED(result)) {
					I_LOG_FATAL_ERROR("Failed to init dxc library! Abort!");
					exit(EXIT_FAILURE);
				}

				// Init DXC compiler
				CComPtr<IDxcCompiler3> compiler;
				result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
				if (FAILED(result)) {
					I_LOG_FATAL_ERROR("Failed to init dxc utility! Abort!");
					exit(EXIT_FAILURE);
				}

				// Initialize DXC utility
				CComPtr<IDxcUtils> utils;
				result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
				if (FAILED(result)) {
					I_LOG_FATAL_ERROR("Failed to init dxc compiler! Abort");
					exit(EXIT_FAILURE);
				}


				std::string vertShaderSource = ReadFile(vertShaderFilename);
				std::string fragShaderSource = ReadFile(fragmentShaderFilename);

				if (vertShaderSource == "" || fragShaderSource == "") {
					I_LOG_FATAL_ERROR("Failed to read shader file(s): %s,\n%s\nAbort!", vertShaderFilename, fragmentShaderFilename);
					exit(EXIT_FAILURE);
				}

				LPCWSTR vertArgs[] = {
					// Entry point
					L"-E", L"main",
					// Target profile
					L"-T", L"vs_6_1",
					// Compile to SPRIV
					L"-spirv"
				};

				LPCWSTR fragArgs[] = {
					// Entry point
					L"-E", L"main",
					// Target profile
					L"-T", L"ps_6_1",
					// Compile to SPRIV
					L"-spirv"
				};

				DxcBuffer vertBuf{};
				vertBuf.Encoding = DXC_CP_ACP;
				vertBuf.Ptr = vertShaderSource.c_str();
				vertBuf.Size = vertShaderSource.size();

				CComPtr<IDxcResult> vertBinSource{ nullptr };
				result = compiler->Compile(
					&vertBuf, 
					vertArgs, 
					sizeof(vertArgs) / sizeof(LPCWSTR), 
					nullptr, 
					IID_PPV_ARGS(&vertBinSource)
				);

				if (SUCCEEDED(result))
					vertBinSource->GetStatus(&result);

				if (FAILED(result) && (vertBinSource)) {
					CComPtr<IDxcBlobEncoding> errorBuf;
					result = vertBinSource->GetErrorBuffer(&errorBuf);
					if (SUCCEEDED(result) && errorBuf) {
						I_LOG_FATAL_ERROR("Failed to compile vertex shader:\n\n%s\n\nAbort!", errorBuf->GetBufferPointer());
						exit(EXIT_FAILURE);
					}
				
				}


				DxcBuffer fragBuf{};
				fragBuf.Encoding = DXC_CP_ACP;
				fragBuf.Ptr = fragShaderSource.c_str();
				fragBuf.Size = fragShaderSource.size();


				CComPtr<IDxcResult> fragBinSource{ nullptr };
				result = compiler->Compile(
					&fragBuf,
					fragArgs,
					sizeof(vertArgs) / sizeof(LPCWSTR),
					nullptr,
					IID_PPV_ARGS(&fragBinSource)
				);

				if (SUCCEEDED(result))
					fragBinSource->GetStatus(&result);

				if (FAILED(result) && (fragBinSource)) {
					CComPtr<IDxcBlobEncoding> errorBuf;
					result = fragBinSource->GetErrorBuffer(&errorBuf);
					if (SUCCEEDED(result) && errorBuf) {
						I_LOG_FATAL_ERROR("Failed to compile fragment shader:\n\n%s\n\nAbort!", errorBuf->GetBufferPointer());
						exit(EXIT_FAILURE);
					}
				}

				CComPtr<IDxcBlob> vertCode;
				vertBinSource->GetResult(&vertCode);
				CComPtr<IDxcBlob> fragCode;
				fragBinSource->GetResult(&fragCode);
				vertBinSource.Release();
				fragBinSource.Release();
				utils.Release();
				compiler.Release();


				return { vertCode, fragCode };
			}
		}
	}
}