#pragma once

#include "ShaderModule.h"
#include "Device.h"
#include "SwapChain.h"

namespace IRun {
	namespace Vulkan {
		class GraphicsPipeline {
		public:
			GraphicsPipeline(const std::string& vertFileName, const std::string& fragFileName, Device& device, SwapChain& swapChain);
		private:

		};
	}
}
