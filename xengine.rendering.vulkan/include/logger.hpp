#include "xengine/rendering/renderer.hpp"
#ifdef VULKAN_RENDERING
#ifndef VULKAN_LOGGER_H
#define VULKAN_LOGGER_H

#include "../include/common.hpp"

namespace vk_init {

	//Callback for logging.
	VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {
		//Print message.
		XEngine::LOG_WARN(std::string("Validation layer: ") + pCallbackData->pMessage + "\n");
		return VK_FALSE;
	}

	/// <summary>
	/// Creates debug logger.
	/// </summary>
	/// <param name="t_instance">Vulkan instance.</param>
	/// <param name="t_dldi">Dispatch loader.</param>
	/// <returns>New debug logger.</returns>
	vk::DebugUtilsMessengerEXT make_debug_messenger(vk::Instance& t_instance, vk::DispatchLoaderDynamic& t_dldi) {
		//Generate debug messanger.
		vk::DebugUtilsMessengerCreateInfoEXT create_info = vk::DebugUtilsMessengerCreateInfoEXT(
			vk::DebugUtilsMessengerCreateFlagsEXT(),
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			debug_callback,
			nullptr
		);
		//Return debug mesanger.
		XEngine::LOG_INFO("Debug logger created.");
		return t_instance.createDebugUtilsMessengerEXT(create_info, nullptr, t_dldi);
	}

}

#endif // VULKAN_LOGGER_H
#endif // VULKAN_RENDERING