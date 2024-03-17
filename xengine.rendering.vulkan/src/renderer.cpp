#include "xengine/rendering/renderer.hpp"
#ifdef VULKAN_RENDERING

#include "../include/common.hpp"
#include "../include/instance.hpp"
#include "../include/logger.hpp"
#include "../include/device.hpp"
#include "../include/swapchain.hpp"

namespace XEngine {
	glm::vec4 Renderer::m_color(0.15f, 0.15f, 0.15f, 1);
	//Vulkan instance.
	vk::Instance instance{ nullptr };
	vk::DebugUtilsMessengerEXT debug_messenger{ nullptr };
	vk::DispatchLoaderDynamic dldi;
	vk::SurfaceKHR surface;
	//Device-related variables.
	vk::PhysicalDevice phys_device{ nullptr };
	vk::Device device{ nullptr };
	vk::Queue graphics_queue{ nullptr };
	vk::Queue present_queue{ nullptr };
	vk::SwapchainKHR swapchain{ nullptr };
	std::vector<vk_init::SwapChainFrame> swapchain_frames;
	vk::Format swapchain_format;
	vk::Extent2D swapchain_extent;

	bool Renderer::is_available() {
		return true;
	}

	void Renderer::initialize() {
		glfwInit();
	}

	bool Renderer::initialize_libs(GLFWwindow* t_window) {
		//Create Vulkan instance.
		instance = vk_init::make_instance("xengine_app");
		if(instance == nullptr)
			return false;
		//Create Vulkan dispatch loader.
		dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
#ifndef NDEBUG
		//Create Vulkan debug messanger.
		debug_messenger = vk_init::make_debug_messenger(instance, dldi);
#endif
		VkSurfaceKHR c_style_surface;
		if(glfwCreateWindowSurface(instance, t_window, nullptr, &c_style_surface) != VK_SUCCESS)
			LOG_WARN("Failed to abstract GLFW surface for Vulkan.");
		surface = c_style_surface;
		//Get device-related values.
		phys_device = vk_init::get_device(instance);
		vk_util::find_queue_families(phys_device, surface);
		device = vk_init::create_logical_device(phys_device, surface);
		//Get graphical and present queue.
		std::array<vk::Queue, 2> queues = vk_init::get_queues(phys_device, device, surface);
		graphics_queue = queues[0];
		present_queue = queues[1];
		//Initialize swapchain.
		vk_init::SwapChainBundle bundle = vk_init::create_swapchain(device, phys_device, surface, 800, 600);
		swapchain = bundle.swapchain;
		swapchain_frames = bundle.frames;
		swapchain_format = bundle.format;
		swapchain_extent = bundle.extent;
		return true;
	}

	void Renderer::terminate() {
		//Terminate Vulkan variables.
		for(vk_init::SwapChainFrame frame : swapchain_frames)
			device.destroyImageView(frame.view);
		device.destroySwapchainKHR(swapchain);
		instance.destroySurfaceKHR(surface);
		device.destroy();
#ifndef NDEBUG
		instance.destroyDebugUtilsMessengerEXT(debug_messenger, nullptr, dldi);
#endif
		instance.destroy();
		//Terminate GLFW.
		glfwTerminate();
	}

	void Renderer::switch_mode(RenderMode t_mode) {
	}

	double Renderer::get_time() {
		return 0;
	}

	void Renderer::print_host_info() {
		printf("Rendering | Vulkan (Vulkan & GLFW)\n");
		printf(("	Vendor: " + get_vendor() + "\n").c_str());
		printf(("	Renderer: " + get_renderer() + "\n").c_str());
		printf(("	Version: " + get_version() + "\n").c_str());
	}

	std::string Renderer::get_vendor() {
		//return vkGetPhysicalDeviceProperties();
		return "NULL";
	}

	std::string Renderer::get_renderer() {
		return "NULL";
	}

	std::string Renderer::get_version() {
		return "NULL";
	}
}

#endif // VULKAN_RENDERING