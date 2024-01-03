#pragma once

#include <vulkan\vulkan.h>

namespace IRun {
	namespace Tools {
		namespace Vk {
			struct VkBasicStruct {
				VkStructureType sType;
				void* pNext;
			};

			template<typename ParentStruct>
			VkBasicStruct* GetChildFromPNextChain(ParentStruct* parentStruct, VkStructureType structureType) {
				VkBasicStruct* extendingStruct = (VkBasicStruct*)(parentStruct->pNext);
				if ((VkStructureType)extendingStruct->sType != structureType)
					GetChildFromPNextChain<VkBasicStruct>(extendingStruct, structureType);

				return extendingStruct;
			}
		}
	}
}