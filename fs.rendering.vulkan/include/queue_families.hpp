#include "firesteel/rendering/renderer.hpp"
#ifdef VULKAN_RENDERING
#ifndef VULKAN_QUEUE_FAMILIES_H
#define VULKAN_QUEUE_FAMILIES_H

#include "../include/common.hpp"

namespace vk_util {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;

		/// <summary>
		/// Returns whether all of the Queue family indices have been set.
		/// </summary>
		/// <returns>True if all of the Queue family indices have been set.</returns>
		bool is_complete() const {
			return graphics_family.has_value() && present_family.has_value();
		}
	};

	/// <summary>
	/// Find suitable queue family indices on the given physical device.
	/// </summary>
	/// <param name="t_phys_device">Physical device.</param>
	/// <param name="t_surface">Present queue surface.</param>
	/// <returns>A struct holding the queue family indices.</returns>
	QueueFamilyIndices find_queue_families(vk::PhysicalDevice t_phys_device, vk::SurfaceKHR t_surface) {
		//Get queue families.
		QueueFamilyIndices indices;
		std::vector<vk::QueueFamilyProperties> queue_families = t_phys_device.getQueueFamilyProperties();
		//Go through all families and get suitable.
		int i = 0;
		for (vk::QueueFamilyProperties queue_family : queue_families) {
			//Check if queue is suitable.
			if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
				indices.graphics_family = i;

			if (t_phys_device.getSurfaceSupportKHR(i, t_surface))
				indices.present_family = i;
			//If we got suitable queue family - return.
			if (indices.is_complete()) break;
			//Otherwise proceed.
			i++;
		}
		//Return suitable queue family (if found).
		return indices;
	}
}


#endif // VULKAN_QUEUE_FAMILIES_H
#endif // VULKAN_RENDERING