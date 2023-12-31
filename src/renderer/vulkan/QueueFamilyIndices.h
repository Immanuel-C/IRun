#pragma once

/// <summary>
/// Stores the location of all the queue families in vulkan
/// </summary>
struct QueueFamilyIndices {
	int graphicsFamily = -1, computeFamily = -1, transferFamily = -1, videoDecodeFamily = -1;

	bool IsValid() {
		return graphicsFamily >= 0 && computeFamily >= 0 && transferFamily >= 0 && videoDecodeFamily >= 0;
	}
};