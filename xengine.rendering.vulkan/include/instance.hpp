#include "xengine/rendering/renderer.hpp"
#ifdef VULKAN_RENDERING
#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include "../include/common.hpp"

namespace vk_init {

	/// <summary>
	/// Check if all extensions and layers are supported on given instance.
	/// </summary>
	/// <param name="t_extensions">Requested extensions.</param>
	/// <param name="t_layers">Requested layers.</param>
	/// <returns>Does given instance support all extensions and layers.</returns>
	bool supported(std::vector<const char*>& t_extensions, std::vector<const char*>& t_layers) {
		//Check extension for support.
		std::vector<vk::ExtensionProperties> supported_ext = vk::enumerateInstanceExtensionProperties();
		bool found;
		for(const char* extension : t_extensions) {
			found = false;
			for(vk::ExtensionProperties sup_ext : supported_ext)
				if(strcmp(extension, sup_ext.extensionName) == 0)
					found = true;
			if(!found) return false;
		}
		//Check layers for support.
		std::vector<vk::LayerProperties> supported_layers = vk::enumerateInstanceLayerProperties();
		for(const char* layer : t_layers) {
			found = false;
			for (vk::LayerProperties sup_layer : supported_layers)
				if(strcmp(layer, sup_layer.layerName) == 0)
					found = true;
			if(!found) return false;
		}
		//If everything is ok - return true.
		return true;
	}

	/// <summary>
	/// Creates instance.
	/// </summary>
	/// <param name="t_name">Name of application.</param>
	/// <returns>New instance.</returns>
	vk::Instance make_instance(const char* t_name) {
		//Get Vulkan version.
		uint32_t version{ 0 };
		vkEnumerateInstanceVersion(&version);
#ifndef NDEBUG
		printf("Rendering | Vulkan Version\n");
		printf(("	Variant: " + std::to_string(VK_API_VERSION_VARIANT(version)) + "\n").c_str());
		printf(("	Major: " + std::to_string(VK_API_VERSION_MAJOR(version)) + "\n").c_str());
		printf(("	Minor: " + std::to_string(VK_API_VERSION_MINOR(version)) + "\n").c_str());
		printf(("	Patch: " + std::to_string(VK_API_VERSION_PATCH(version)) + "\n").c_str());
#endif
		version &= ~(0xFFFU);
		version = VK_MAKE_API_VERSION(0, 1, 0, 0);
		//Create application.
		vk::ApplicationInfo app_info = vk::ApplicationInfo(
			t_name,
			1,
			"XEngine",
			version,
			version
		);
		//Get all extensions.
		uint32_t ex_count = 0;
		const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&ex_count);
		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + ex_count);
#ifndef NDEBUG
		extensions.push_back("VK_EXT_debug_utils");
#endif
		//List all extensions.
#ifndef NDEBUG
		printf("Rendering | Extensions\n");
		for(const char* ex_name : extensions)
			std::cout << "\t\"" << ex_name << "\"\n";
#endif
		std::vector<const char*> layers;
#ifndef NDEBUG
		layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
		if(!supported(extensions, layers))
			return nullptr;
		//Create instance data.
		vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&app_info,
			static_cast<uint32_t>(layers.size()), layers.data(), //Enabled layers.
			static_cast<uint32_t>(extensions.size()), extensions.data() //Enabled extensions.
		);
		//Try to create Vulkan app.
		try {
			return vk::createInstance(createInfo);
		}
		catch (vk::SystemError err) {
			XEngine::LOG_CRIT("Failed to create Instance!");
			return nullptr;
		}
	}

}

#endif // VULKAN_INSTANCE_H
#endif // VULKAN_RENDERINGs