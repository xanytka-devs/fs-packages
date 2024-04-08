#include "firesteel/rendering/renderer.hpp"
#ifdef VULKAN_RENDERING
#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include "../include/common.hpp"
#include "../include/queue_families.hpp"

namespace vk_init {
	//SwapChain capabilities.
	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;
	};

	//SwapChain image.
	struct SwapChainFrame {
		vk::Image image;
		vk::ImageView view;
	};

	//SwapChain data.
	struct SwapChainBundle {
		vk::SwapchainKHR swapchain;
		std::vector<SwapChainFrame> frames;
		vk::Format format;
		vk::Extent2D extent;
	};

	SwapChainSupportDetails query_swapchain_support(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
		SwapChainSupportDetails support;
		support.capabilities = device.getSurfaceCapabilitiesKHR(surface);
		support.formats = device.getSurfaceFormatsKHR(surface);
		support.present_modes = device.getSurfacePresentModesKHR(surface);
		return support;
	}

	vk::SurfaceFormatKHR choose_swapchain_surface_format(std::vector<vk::SurfaceFormatKHR> formats) {

		for (vk::SurfaceFormatKHR format : formats) {
			if (format.format == vk::Format::eB8G8R8A8Unorm
				&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return format;
			}
		}

		return formats[0];
	}

	vk::PresentModeKHR choose_swapchain_present_mode(std::vector<vk::PresentModeKHR> presentModes) {

		for (vk::PresentModeKHR presentMode : presentModes) {
			if (presentMode == vk::PresentModeKHR::eMailbox) {
				return presentMode;
			}
		}

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities) {

		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;
		else {
			vk::Extent2D extent = { width, height };
			extent.width = std::min(
				capabilities.maxImageExtent.width,
				std::max(capabilities.minImageExtent.width, extent.width)
			);
			extent.height = std::min(
				capabilities.maxImageExtent.height,
				std::max(capabilities.minImageExtent.height, extent.height)
			);
			return extent;
		}
	}

	SwapChainBundle create_swapchain(vk::Device t_logical_dev, vk::PhysicalDevice t_phys_dev, vk::SurfaceKHR t_surface, int t_width, int t_height) {

		SwapChainSupportDetails support = query_swapchain_support(t_phys_dev, t_surface);
		vk::SurfaceFormatKHR format = choose_swapchain_surface_format(support.formats);
		vk::PresentModeKHR presentMode = choose_swapchain_present_mode(support.present_modes);
		vk::Extent2D extent = choose_swapchain_extent(t_width, t_height, support.capabilities);

		uint32_t imageCount = std::min(
			support.capabilities.maxImageCount,
			support.capabilities.minImageCount + 1
		);

		vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(), t_surface, imageCount, format.format, format.colorSpace,
			extent, 1, vk::ImageUsageFlagBits::eColorAttachment
		);


		vk_util::QueueFamilyIndices indices = vk_util::find_queue_families(t_phys_dev, t_surface);
		uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.present_family.value() };

		if (indices.graphics_family != indices.present_family) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}

		createInfo.preTransform = support.capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

		SwapChainBundle bundle{};
		try {
			bundle.swapchain = t_logical_dev.createSwapchainKHR(createInfo);
		}
		catch (vk::SystemError err) {
			throw std::runtime_error("failed to create swap chain!");
		}

		std::vector<vk::Image> imgs = t_logical_dev.getSwapchainImagesKHR(bundle.swapchain);
		bundle.frames.resize(imgs.size());

		for (size_t i = 0; i < imgs.size(); i++) {
			vk::ImageViewCreateInfo view_create_info = {};
			view_create_info.image = imgs[i];
			view_create_info.viewType = vk::ImageViewType::e2D;
			view_create_info.components.r = vk::ComponentSwizzle::eIdentity;
			view_create_info.components.g = vk::ComponentSwizzle::eIdentity;
			view_create_info.components.b = vk::ComponentSwizzle::eIdentity;
			view_create_info.components.a = vk::ComponentSwizzle::eIdentity;
			view_create_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			view_create_info.subresourceRange.baseMipLevel = 0;
			view_create_info.subresourceRange.levelCount = 1;
			view_create_info.subresourceRange.baseArrayLayer = 0;
			view_create_info.subresourceRange.layerCount = 1;
			view_create_info.format = format.format;

			bundle.frames[i].image = imgs[i];
			bundle.frames[i].view = t_logical_dev.createImageView(view_create_info);
		}

		bundle.format = format.format;
		bundle.extent = extent;

		return bundle;
	}
}

#endif // VULKAN_SWAPCHAIN_H
#endif // VULKAN_RENDERING