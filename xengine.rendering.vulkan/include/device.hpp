#include "xengine/rendering/renderer.hpp"
#ifdef VULKAN_RENDERING
#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include "../include/common.hpp"
#include "../include/queue_families.hpp"

namespace vk_init {
	/// <summary>
	/// Log device properties.
	/// </summary>
	/// <param name="t_phys_device">Physical device.</param>
	void log_device_properties(const vk::PhysicalDevice& t_phys_device) {
		//Get device properties.
		vk::PhysicalDeviceProperties properties = t_phys_device.getProperties();
		std::cout << "\t" << properties.deviceName << " | ";
		//Get device type.
		const char* dev_name = "";
		switch (properties.deviceType) {
		case (vk::PhysicalDeviceType::eCpu):
			dev_name = "CPU";
			break;
		case (vk::PhysicalDeviceType::eDiscreteGpu):
			dev_name = "Discrete GPU";
			break;
		case (vk::PhysicalDeviceType::eIntegratedGpu):
			dev_name = "Integrated GPU";
			break;
		case (vk::PhysicalDeviceType::eVirtualGpu):
			dev_name = "Virtual GPU";
			break;
		default:
			dev_name = "Other";
		}
		//Log device type.
		std::cout << dev_name << "\n";
	}

	/// <summary>
	/// Check if device supports all extensions.
	/// </summary>
	/// <param name="t_phys_device">Physical device.</param>
	/// <param name="t_req_extensions">Requested extensions.</param>
	/// <returns>Does device support all extensions.</returns>
	bool check_device_ext_support(const vk::PhysicalDevice& t_phys_device,
		const std::vector<const char*>& t_req_extensions) {
		//Check if a given physical device can satisfy a list of requested device extensions.
		std::set<std::string> required_ext(t_req_extensions.begin(), t_req_extensions.end());
		for (vk::ExtensionProperties& extension : t_phys_device.enumerateDeviceExtensionProperties()) {
			//Remove this from the list of required extensions.
			required_ext.erase(extension.extensionName);
		}
		//If the set is empty then all requirements have been satisfied.
		return required_ext.empty();
	}

	/// <summary>
	/// Check if device is suitable.
	/// </summary>
	/// <param name="t_phys_device">Physical device.</param>
	/// <returns>Suitability of given device.</returns>
	bool is_suitable(const vk::PhysicalDevice& t_phys_device) {
		//A device is suitable if it can present to the screen.
		const std::vector<const char*> requested_ext = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		//Check support for extensions.
		if(check_device_ext_support(t_phys_device, requested_ext)) {
			XEngine::LOG_INFO("Device can support requested extensions.");
			return true;
		} else {
			XEngine::LOG_INFO("Device can't support requested extensions.");
			return false;
		}
	}

	/// <summary>
	/// Get suitable device.
	/// </summary>
	/// <param name="t_instance">Vulkan instance.</param>
	/// <returns>Suitable device.</returns>
	vk::PhysicalDevice get_device(vk::Instance& t_instance) {
		//Get devices.
		std::vector<vk::PhysicalDevice> devices = t_instance.enumeratePhysicalDevices();
		XEngine::LOG_INFO("There are ", char(devices.size()) + " physical device(s) available on this system:");
		//Check if a suitable device can be found.
		for (vk::PhysicalDevice device : devices) {
#ifndef NDEBUG
			log_device_properties(device);
#endif
			//Check if device is suitable.
			if(is_suitable(device)) return device;
		}
		return nullptr;
	}

	/// <summary>
	/// Create logical device.
	/// </summary>
	/// <param name="t_phys_device">Physical device.</param>
	/// <param name="t_surface">Present queue surface.</param>
	/// <returns>Logical device.</returns>
	vk::Device create_logical_device(vk::PhysicalDevice t_phys_device, vk::SurfaceKHR t_surface) {
		//Get device's queue families.
		vk_util::QueueFamilyIndices indices = vk_util::find_queue_families(t_phys_device, t_surface);
		std::vector<uint32_t> unique_indicies;
		//Add graphics family to unique indicies.
		unique_indicies.push_back(indices.graphics_family.value());
		//Add present family to unique indicies (if it isn't duplicate of graphics).
		if(indices.graphics_family.value() != indices.present_family.value())
			unique_indicies.push_back(indices.present_family.value());
		//Create device's info variables.
		std::vector<vk::DeviceQueueCreateInfo> queue_create_info;
		float queue_priority = 1.0f;
		for (uint32_t queue_family_index : unique_indicies) {
			queue_create_info.push_back(
				vk::DeviceQueueCreateInfo(
					vk::DeviceQueueCreateFlags(), queue_family_index, 1, &queue_priority
				)
			);
		}
		//Set device's extensions.
		std::vector<const char*> device_ext = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		//Get device's features.
		vk::PhysicalDeviceFeatures device_features = vk::PhysicalDeviceFeatures();
		std::vector<const char*> enabled_layers;
#ifndef NDEBUG
		enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
		//Create device info.
		vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(),
			static_cast<uint32_t>(queue_create_info.size()), queue_create_info.data(),
			static_cast<uint32_t>(enabled_layers.size()), enabled_layers.data(),
			static_cast<uint32_t>(device_ext.size()), device_ext.data(),
			&device_features
		);
		//Try to create logical device.
		try {
			vk::Device device = t_phys_device.createDevice(deviceInfo);
			XEngine::LOG_INFO("GPU has been successfully abstracted.");
			return device;
		}
		catch (vk::SystemError err) {
			XEngine::LOG_CRIT("Device creation failed.");
			return nullptr;
		}
	}

	/// <summary>
	/// Get the graphics queue.
	/// </summary>
	/// <param name="t_phys_device">Physical device.</param>
	/// <param name="t_device">Logical device.</param>
	/// <param name="t_surface">Present queue surface.</param>
	/// <returns>Graphics and present queue.</returns>
	std::array<vk::Queue, 2> get_queues(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface) {

		vk_util::QueueFamilyIndices indices = vk_util::find_queue_families(physicalDevice, surface);

		return { {
				device.getQueue(indices.graphics_family.value(), 0),
				device.getQueue(indices.present_family.value(), 0),
			} };
	}
}

#endif // VULKAN_DEVICE_H
#endif // VULKAN_RENDERING