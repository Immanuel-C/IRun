#pragma once

#include <vulkan\vulkan.h>

namespace IRun {
	namespace Tools {
		namespace Vk {
			struct VkBasicStruct {
				VkStructureType    sType;
				void* pNext;
			};

			template<typename ParentStruct>
			VkBasicStruct* GetChildFromPNextChain(ParentStruct* parentStruct, VkStructureType structureType) {
				VkBasicStruct* newStruct = (VkBasicStruct*)(parentStruct->pNext);
				if ((VkStructureType)newStruct->sType != structureType)
					GetChildFromPNextChain<VkBasicStruct>(newStruct, structureType);

				return newStruct;
			}
		}
	}
}