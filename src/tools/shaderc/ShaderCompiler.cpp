#include "ShaderCompiler.h"

namespace IRun {
	namespace Tools {
		namespace Shaders {

            // From https://github.com/KhronosGroup/glslang/blob/main/glslang/ResourceLimits/ResourceLimits.cpp
            TBuiltInResource Resources;

            const TBuiltInResource DefaultTBuiltInResource = {
                /* .MaxLights = */ 32,
                /* .MaxClipPlanes = */ 6,
                /* .MaxTextureUnits = */ 32,
                /* .MaxTextureCoords = */ 32,
                /* .MaxVertexAttribs = */ 64,
                /* .MaxVertexUniformComponents = */ 4096,
                /* .MaxVaryingFloats = */ 64,
                /* .MaxVertexTextureImageUnits = */ 32,
                /* .MaxCombinedTextureImageUnits = */ 80,
                /* .MaxTextureImageUnits = */ 32,
                /* .MaxFragmentUniformComponents = */ 4096,
                /* .MaxDrawBuffers = */ 32,
                /* .MaxVertexUniformVectors = */ 128,
                /* .MaxVaryingVectors = */ 8,
                /* .MaxFragmentUniformVectors = */ 16,
                /* .MaxVertexOutputVectors = */ 16,
                /* .MaxFragmentInputVectors = */ 15,
                /* .MinProgramTexelOffset = */ -8,
                /* .MaxProgramTexelOffset = */ 7,
                /* .MaxClipDistances = */ 8,
                /* .MaxComputeWorkGroupCountX = */ 65535,
                /* .MaxComputeWorkGroupCountY = */ 65535,
                /* .MaxComputeWorkGroupCountZ = */ 65535,
                /* .MaxComputeWorkGroupSizeX = */ 1024,
                /* .MaxComputeWorkGroupSizeY = */ 1024,
                /* .MaxComputeWorkGroupSizeZ = */ 64,
                /* .MaxComputeUniformComponents = */ 1024,
                /* .MaxComputeTextureImageUnits = */ 16,
                /* .MaxComputeImageUniforms = */ 8,
                /* .MaxComputeAtomicCounters = */ 8,
                /* .MaxComputeAtomicCounterBuffers = */ 1,
                /* .MaxVaryingComponents = */ 60,
                /* .MaxVertexOutputComponents = */ 64,
                /* .MaxGeometryInputComponents = */ 64,
                /* .MaxGeometryOutputComponents = */ 128,
                /* .MaxFragmentInputComponents = */ 128,
                /* .MaxImageUnits = */ 8,
                /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
                /* .MaxCombinedShaderOutputResources = */ 8,
                /* .MaxImageSamples = */ 0,
                /* .MaxVertexImageUniforms = */ 0,
                /* .MaxTessControlImageUniforms = */ 0,
                /* .MaxTessEvaluationImageUniforms = */ 0,
                /* .MaxGeometryImageUniforms = */ 0,
                /* .MaxFragmentImageUniforms = */ 8,
                /* .MaxCombinedImageUniforms = */ 8,
                /* .MaxGeometryTextureImageUnits = */ 16,
                /* .MaxGeometryOutputVertices = */ 256,
                /* .MaxGeometryTotalOutputComponents = */ 1024,
                /* .MaxGeometryUniformComponents = */ 1024,
                /* .MaxGeometryVaryingComponents = */ 64,
                /* .MaxTessControlInputComponents = */ 128,
                /* .MaxTessControlOutputComponents = */ 128,
                /* .MaxTessControlTextureImageUnits = */ 16,
                /* .MaxTessControlUniformComponents = */ 1024,
                /* .MaxTessControlTotalOutputComponents = */ 4096,
                /* .MaxTessEvaluationInputComponents = */ 128,
                /* .MaxTessEvaluationOutputComponents = */ 128,
                /* .MaxTessEvaluationTextureImageUnits = */ 16,
                /* .MaxTessEvaluationUniformComponents = */ 1024,
                /* .MaxTessPatchComponents = */ 120,
                /* .MaxPatchVertices = */ 32,
                /* .MaxTessGenLevel = */ 64,
                /* .MaxViewports = */ 16,
                /* .MaxVertexAtomicCounters = */ 0,
                /* .MaxTessControlAtomicCounters = */ 0,
                /* .MaxTessEvaluationAtomicCounters = */ 0,
                /* .MaxGeometryAtomicCounters = */ 0,
                /* .MaxFragmentAtomicCounters = */ 8,
                /* .MaxCombinedAtomicCounters = */ 8,
                /* .MaxAtomicCounterBindings = */ 1,
                /* .MaxVertexAtomicCounterBuffers = */ 0,
                /* .MaxTessControlAtomicCounterBuffers = */ 0,
                /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
                /* .MaxGeometryAtomicCounterBuffers = */ 0,
                /* .MaxFragmentAtomicCounterBuffers = */ 1,
                /* .MaxCombinedAtomicCounterBuffers = */ 1,
                /* .MaxAtomicCounterBufferSize = */ 16384,
                /* .MaxTransformFeedbackBuffers = */ 4,
                /* .MaxTransformFeedbackInterleavedComponents = */ 64,
                /* .MaxCullDistances = */ 8,
                /* .MaxCombinedClipAndCullDistances = */ 8,
                /* .MaxSamples = */ 4,
                /* .maxMeshOutputVerticesNV = */ 256,
                /* .maxMeshOutputPrimitivesNV = */ 512,
                /* .maxMeshWorkGroupSizeX_NV = */ 32,
                /* .maxMeshWorkGroupSizeY_NV = */ 1,
                /* .maxMeshWorkGroupSizeZ_NV = */ 1,
                /* .maxTaskWorkGroupSizeX_NV = */ 32,
                /* .maxTaskWorkGroupSizeY_NV = */ 1,
                /* .maxTaskWorkGroupSizeZ_NV = */ 1,
                /* .maxMeshViewCountNV = */ 4,
                /* .maxMeshOutputVerticesEXT = */ 256,
                /* .maxMeshOutputPrimitivesEXT = */ 256,
                /* .maxMeshWorkGroupSizeX_EXT = */ 128,
                /* .maxMeshWorkGroupSizeY_EXT = */ 128,
                /* .maxMeshWorkGroupSizeZ_EXT = */ 128,
                /* .maxTaskWorkGroupSizeX_EXT = */ 128,
                /* .maxTaskWorkGroupSizeY_EXT = */ 128,
                /* .maxTaskWorkGroupSizeZ_EXT = */ 128,
                /* .maxMeshViewCountEXT = */ 4,
                /* .maxDualSourceDrawBuffersEXT = */ 1,

                /* .limits = */ {
                    /* .nonInductiveForLoops = */ 1,
                    /* .whileLoops = */ 1,
                    /* .doWhileLoops = */ 1,
                    /* .generalUniformIndexing = */ 1,
                    /* .generalAttributeMatrixVectorIndexing = */ 1,
                    /* .generalVaryingIndexing = */ 1,
                    /* .generalSamplerIndexing = */ 1,
                    /* .generalVariableIndexing = */ 1,
                    /* .generalConstantMatrixVectorIndexing = */ 1,
                } };


			std::vector<const char*> CompileToSpirvBinary(const std::string& fileName, ShaderType type, ShaderLanguage lang)
			{
				std::string sourceCode = ReadFile(fileName);

				glslang_input_t input{};
				input.language = (glslang_source_t)lang;
				input.stage = (glslang_stage_t)type;
				input.client = GLSLANG_CLIENT_VULKAN;
				input.client_version = GLSLANG_TARGET_VULKAN_1_3;
				input.target_language = GLSLANG_TARGET_SPV;
				input.target_language_version = GLSLANG_TARGET_SPV_1_6;
				input.code = sourceCode.c_str();
				input.default_version = 460;
				input.default_profile = GLSLANG_NO_PROFILE;
				input.force_default_version_and_profile = false;
				input.messages = GLSLANG_MSG_DEBUG_INFO_BIT;
                input.resource = (glslang_resource_t*)&DefaultTBuiltInResource;

                glslang_initialize_process();

                glslang_shader_t* shader = glslang_shader_create(&input);

                if (!glslang_shader_parse(shader, &input)) {
                    I_DEBUG_LOG_FATAL_ERROR("Failed to compile shader (%s):\n\n%s\n\nAbort!", fileName.c_str(), glslang_shader_get_info_log(shader));
                    I_DEBUG_LOG_FATAL_ERROR("(Debug Log): Failed to compile shader (%s):\n\n%s\n\nAbort!", fileName.c_str(), glslang_shader_get_info_debug_log(shader));
                    exit(EXIT_FAILURE);
                    return { };
                }

                glslang_program_t* program = glslang_program_create();
                glslang_program_add_shader(program, shader);

                if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
                    I_DEBUG_LOG_FATAL_ERROR("Failed to link shader (%s):\n\n%s\n\nAbort!", fileName.c_str(), glslang_shader_get_info_log(shader));
                    I_DEBUG_LOG_FATAL_ERROR("(Debug Log): Failed to link shader (%s):\n\n%s\n\nAbort!", fileName.c_str(), glslang_shader_get_info_debug_log(shader));
                    exit(EXIT_FAILURE);
                    return { };
                }

                glslang_program_SPIRV_generate(program, input.stage);

                if (glslang_program_SPIRV_get_messages(program))
                {
                    printf("%s", glslang_program_SPIRV_get_messages(program));
                }

                glslang_shader_delete(shader);

                std::vector<const char*> resultBin(glslang_program_SPIRV_get_size(program));
                memcpy(resultBin.data(), glslang_program_SPIRV_get_ptr(program), resultBin.size());

                glslang_program_delete(program);

				return resultBin;
			}
		}
	}
}
