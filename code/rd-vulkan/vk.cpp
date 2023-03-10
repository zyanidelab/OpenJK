#include "tr_local.h"
#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include <vector>
#include <array>

const int VERTEX_CHUNK_SIZE = 512 * 1024;

const int XYZ_SIZE      = 4 * VERTEX_CHUNK_SIZE;
const int COLOR_SIZE    = 1 * VERTEX_CHUNK_SIZE;
const int ST0_SIZE      = 2 * VERTEX_CHUNK_SIZE;
const int ST1_SIZE      = 2 * VERTEX_CHUNK_SIZE;

const int XYZ_OFFSET    = 0;
const int COLOR_OFFSET  = XYZ_OFFSET + XYZ_SIZE;
const int ST0_OFFSET    = COLOR_OFFSET + COLOR_SIZE;
const int ST1_OFFSET    = ST0_OFFSET + ST0_SIZE;

static const int VERTEX_BUFFER_SIZE = XYZ_SIZE + COLOR_SIZE + ST0_SIZE + ST1_SIZE;
static const int INDEX_BUFFER_SIZE = 2 * 1024 * 1024;

std::vector<int>           images_to_update_bindless =  std::vector<int>();

//
// Vulkan API functions used by the renderer.
//
/*PFN_vkGetInstanceProcAddr						vkGetInstanceProcAddr;
PFN_vkCreateInstance							vkCreateInstance;
PFN_vkEnumerateInstanceExtensionProperties		vkEnumerateInstanceExtensionProperties;

PFN_vkCreateDevice								vkCreateDevice;
PFN_vkDestroyInstance							vkDestroyInstance;
PFN_vkEnumerateDeviceExtensionProperties		vkEnumerateDeviceExtensionProperties;
PFN_vkEnumeratePhysicalDevices					vkEnumeratePhysicalDevices;
PFN_vkGetDeviceProcAddr							vkGetDeviceProcAddr;
PFN_vkGetPhysicalDeviceFeatures					vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceFormatProperties			vkGetPhysicalDeviceFormatProperties;
PFN_vkGetPhysicalDeviceMemoryProperties			vkGetPhysicalDeviceMemoryProperties;
PFN_vkGetPhysicalDeviceProperties				vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceQueueFamilyProperties	vkGetPhysicalDeviceQueueFamilyProperties;
//PFN_vkCreateWin32SurfaceKHR						vkCreateWin32SurfaceKHR;
PFN_vkDestroySurfaceKHR							vkDestroySurfaceKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR	vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR		vkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR	vkGetPhysicalDeviceSurfacePresentModesKHR;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR		vkGetPhysicalDeviceSurfaceSupportKHR;
PFN_vkCreateDebugReportCallbackEXT				vkCreateDebugReportCallbackEXT;
PFN_vkDestroyDebugReportCallbackEXT				vkDestroyDebugReportCallbackEXT;

PFN_vkAllocateCommandBuffers					vkAllocateCommandBuffers;
PFN_vkAllocateDescriptorSets					vkAllocateDescriptorSets;
PFN_vkAllocateMemory							vkAllocateMemory;
PFN_vkBeginCommandBuffer						vkBeginCommandBuffer;
PFN_vkBindBufferMemory							vkBindBufferMemory;
PFN_vkBindImageMemory							vkBindImageMemory;
PFN_vkCmdBeginRenderPass						vkCmdBeginRenderPass;
PFN_vkCmdBindDescriptorSets						vkCmdBindDescriptorSets;
PFN_vkCmdBindIndexBuffer						vkCmdBindIndexBuffer;
PFN_vkCmdBindPipeline							vkCmdBindPipeline;
PFN_vkCmdBindVertexBuffers						vkCmdBindVertexBuffers;
PFN_vkCmdBlitImage								vkCmdBlitImage;
PFN_vkCmdClearAttachments						vkCmdClearAttachments;
PFN_vkCmdCopyBufferToImage						vkCmdCopyBufferToImage;
PFN_vkCmdCopyImage								vkCmdCopyImage;
PFN_vkCmdDraw									vkCmdDraw;
PFN_vkCmdDrawIndexed							vkCmdDrawIndexed;
PFN_vkCmdEndRenderPass							vkCmdEndRenderPass;
PFN_vkCmdPipelineBarrier						vkCmdPipelineBarrier;
PFN_vkCmdPushConstants							vkCmdPushConstants;
PFN_vkCmdSetDepthBias							vkCmdSetDepthBias;
PFN_vkCmdSetScissor								vkCmdSetScissor;
PFN_vkCmdSetViewport							vkCmdSetViewport;
PFN_vkCreateBuffer								vkCreateBuffer;
PFN_vkCreateCommandPool							vkCreateCommandPool;
PFN_vkCreateDescriptorPool						vkCreateDescriptorPool;
PFN_vkCreateDescriptorSetLayout					vkCreateDescriptorSetLayout;
PFN_vkCreateFence								vkCreateFence;
PFN_vkCreateFramebuffer							vkCreateFramebuffer;
PFN_vkCreateGraphicsPipelines					vkCreateGraphicsPipelines;
PFN_vkCreateImage								vkCreateImage;
PFN_vkCreateImageView							vkCreateImageView;
PFN_vkCreatePipelineLayout						vkCreatePipelineLayout;
PFN_vkCreateRenderPass							vkCreateRenderPass;
PFN_vkCreateSampler								vkCreateSampler;
PFN_vkCreateSemaphore							vkCreateSemaphore;
PFN_vkCreateShaderModule						vkCreateShaderModule;
PFN_vkDestroyBuffer								vkDestroyBuffer;
PFN_vkDestroyCommandPool						vkDestroyCommandPool;
PFN_vkDestroyDescriptorPool						vkDestroyDescriptorPool;
PFN_vkDestroyDescriptorSetLayout				vkDestroyDescriptorSetLayout;
PFN_vkDestroyDevice								vkDestroyDevice;
PFN_vkDestroyFence								vkDestroyFence;
PFN_vkDestroyFramebuffer						vkDestroyFramebuffer;
PFN_vkDestroyImage								vkDestroyImage;
PFN_vkDestroyImageView							vkDestroyImageView;
PFN_vkDestroyPipeline							vkDestroyPipeline;
PFN_vkDestroyPipelineLayout						vkDestroyPipelineLayout;
PFN_vkDestroyRenderPass							vkDestroyRenderPass;
PFN_vkDestroySampler							vkDestroySampler;
PFN_vkDestroySemaphore							vkDestroySemaphore;
PFN_vkDestroyShaderModule						vkDestroyShaderModule;
PFN_vkDeviceWaitIdle							vkDeviceWaitIdle;
PFN_vkEndCommandBuffer							vkEndCommandBuffer;
PFN_vkFreeCommandBuffers						vkFreeCommandBuffers;
PFN_vkFreeDescriptorSets						vkFreeDescriptorSets;
PFN_vkFreeMemory								vkFreeMemory;
PFN_vkGetBufferMemoryRequirements				vkGetBufferMemoryRequirements;
PFN_vkGetDeviceQueue							vkGetDeviceQueue;
PFN_vkGetImageMemoryRequirements				vkGetImageMemoryRequirements;
PFN_vkGetImageSubresourceLayout					vkGetImageSubresourceLayout;
PFN_vkMapMemory									vkMapMemory;
PFN_vkQueueSubmit								vkQueueSubmit;
PFN_vkQueueWaitIdle								vkQueueWaitIdle;
PFN_vkResetDescriptorPool						vkResetDescriptorPool;
PFN_vkResetFences								vkResetFences;
PFN_vkUpdateDescriptorSets						vkUpdateDescriptorSets;
PFN_vkWaitForFences								vkWaitForFences;
PFN_vkAcquireNextImageKHR						vkAcquireNextImageKHR;
PFN_vkCreateSwapchainKHR						vkCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR						vkDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR						vkGetSwapchainImagesKHR;
PFN_vkQueuePresentKHR							vkQueuePresentKHR;*/
////////////////////////////////////////////////////////////////////////////


static uint32_t find_memory_type(VkPhysicalDevice physical_device, uint32_t memory_type_bits, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
		if ((memory_type_bits & (1 << i)) != 0 &&
			(memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	ri.Error(ERR_FATAL, "Vulkan: failed to find matching memory type with requested properties");
	return -1;
}

static VkFormat get_depth_format(VkPhysicalDevice physical_device) {
	VkFormat formats[2];
	if (r_stencilbits->integer > 0) {
		formats[0] = VK_FORMAT_D24_UNORM_S8_UINT;
		formats[1] = VK_FORMAT_D32_SFLOAT_S8_UINT;
		glConfig.stencilBits = 8;
	} else {
		formats[0] = VK_FORMAT_X8_D24_UNORM_PACK32;
		formats[1] = VK_FORMAT_D32_SFLOAT;
		glConfig.stencilBits = 0;
	}

	for (int i = 0; i < 2; i++) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physical_device, formats[i], &props);
		if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0) {
			return formats[i];
		}
	}
	ri.Error(ERR_FATAL, "get_depth_format: failed to find depth attachment format");
	return VK_FORMAT_UNDEFINED; // never get here
}

static VkSwapchainKHR create_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, VkSurfaceFormatKHR surface_format) {
	VkSurfaceCapabilitiesKHR surface_caps;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_caps));

	VkExtent2D image_extent = surface_caps.currentExtent;
	if (image_extent.width == 0xffffffff && image_extent.height == 0xffffffff) {
		image_extent.width = std::min(surface_caps.maxImageExtent.width, std::max(surface_caps.minImageExtent.width, 640u));
		image_extent.height = std::min(surface_caps.maxImageExtent.height, std::max(surface_caps.minImageExtent.height, 480u));
	}

	// VK_IMAGE_USAGE_TRANSFER_DST_BIT is required by image clear operations.
	if ((surface_caps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0)
		ri.Error(ERR_FATAL, "create_swapchain: VK_IMAGE_USAGE_TRANSFER_DST_BIT is not supported by the swapchain");

	// VK_IMAGE_USAGE_TRANSFER_SRC_BIT is required in order to take screenshots.
	if ((surface_caps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) == 0)
		ri.Error(ERR_FATAL, "create_swapchain: VK_IMAGE_USAGE_TRANSFER_SRC_BIT is not supported by the swapchain");

	// determine present mode and swapchain image count
	uint32_t present_mode_count;
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr));
	std::vector<VkPresentModeKHR> present_modes(present_mode_count);
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data()));

	bool mailbox_supported = false;
	bool immediate_supported = false;

	for (auto pm : present_modes) {
		if (pm == VK_PRESENT_MODE_MAILBOX_KHR)
			mailbox_supported = true;
		else if (pm == VK_PRESENT_MODE_IMMEDIATE_KHR)
			immediate_supported = true;
	}

	VkPresentModeKHR present_mode;
	uint32_t image_count;
	
	if(r_swapInterval->integer){
        present_mode = VK_PRESENT_MODE_FIFO_KHR;
	} else if (immediate_supported) {
	    present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	} else {
	    present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
	}
	
	/*if (mailbox_supported) {
		present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
		image_count = std::max(3u, surface_caps.minImageCount);
	} else {
		present_mode = immediate_supported ? VK_PRESENT_MODE_IMMEDIATE_KHR : VK_PRESENT_MODE_FIFO_KHR;
		image_count = std::max(2u, surface_caps.minImageCount);
	}*/

	if (surface_caps.maxImageCount > 0) {
		image_count = std::min(image_count, surface_caps.maxImageCount);
	}

	// create swap chain
	VkSwapchainCreateInfoKHR desc;
	desc.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	desc.pNext = nullptr;
	desc.flags = 0;
	desc.surface = surface;
	desc.minImageCount = image_count;
	desc.imageFormat = surface_format.format;
	desc.imageColorSpace = surface_format.colorSpace;
	desc.imageExtent = image_extent;
	desc.imageArrayLayers = 1;
	desc.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	desc.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	desc.queueFamilyIndexCount = 0;
	desc.pQueueFamilyIndices = nullptr;
	desc.preTransform = surface_caps.currentTransform;
	desc.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	desc.presentMode = present_mode;
	desc.clipped = VK_TRUE;
	desc.oldSwapchain = VK_NULL_HANDLE;

	VkSwapchainKHR swapchain;
	VK_CHECK(vkCreateSwapchainKHR(device, &desc, nullptr, &swapchain));
	return swapchain;
}

static VkRenderPass create_render_pass(VkDevice device, VkFormat color_format, VkFormat depth_format) {
	VkAttachmentDescription attachments[2];
	attachments[0].flags = 0;
	attachments[0].format = color_format;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	attachments[1].flags = 0;
	attachments[1].format = depth_format;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference color_attachment_ref;
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_ref;
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass;
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	VkRenderPassCreateInfo desc;
	desc.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	desc.pNext = nullptr;
	desc.flags = 0;
	desc.attachmentCount = sizeof(attachments) / sizeof(attachments[0]);
	desc.pAttachments = attachments;
	desc.subpassCount = 1;
	desc.pSubpasses = &subpass;
	desc.dependencyCount = 0;
	desc.pDependencies = nullptr;

	VkRenderPass render_pass;
	VK_CHECK(vkCreateRenderPass(device, &desc, nullptr, &render_pass));
	return render_pass;
}

static void record_and_run_commands(VkCommandPool command_pool, VkQueue queue, std::function<void(VkCommandBuffer)> recorder) {

	VkCommandBufferAllocateInfo alloc_info;
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.pNext = nullptr;
	alloc_info.commandPool = command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	VK_CHECK(vkAllocateCommandBuffers(vk.device, &alloc_info, &command_buffer));

	VkCommandBufferBeginInfo begin_info;
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.pNext = nullptr;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	begin_info.pInheritanceInfo = nullptr;

	VK_CHECK(vkBeginCommandBuffer(command_buffer, &begin_info));
	recorder(command_buffer);
	VK_CHECK(vkEndCommandBuffer(command_buffer));

	VkSubmitInfo submit_info;
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext = nullptr;
	submit_info.waitSemaphoreCount = 0;
	submit_info.pWaitSemaphores = nullptr;
	submit_info.pWaitDstStageMask = nullptr;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores = nullptr;

	VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE));
	VK_CHECK(vkQueueWaitIdle(queue));
	vkFreeCommandBuffers(vk.device, command_pool, 1, &command_buffer);
}

static void record_image_layout_transition(VkCommandBuffer command_buffer, VkImage image, VkImageAspectFlags image_aspect_flags,
	VkAccessFlags src_access_flags, VkImageLayout old_layout, VkAccessFlags dst_access_flags, VkImageLayout new_layout) {

	VkImageMemoryBarrier barrier;
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = nullptr;
	barrier.srcAccessMask = src_access_flags;
	barrier.dstAccessMask = dst_access_flags;
	barrier.oldLayout = old_layout;
	barrier.newLayout = new_layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = image_aspect_flags;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,
		0, nullptr, 0, nullptr, 1, &barrier);
}

static void allocate_and_bind_image_memory(VkImage image) {
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(vk.device, image, &memory_requirements);

	if (memory_requirements.size > IMAGE_CHUNK_SIZE) {
		ri.Error(ERR_FATAL, "Vulkan: could not allocate memory, image is too large.");
	}

	Vk_World::Chunk* chunk = nullptr;

	// Try to find an existing chunk of sufficient capacity.
	const auto mask = ~(memory_requirements.alignment - 1);
	for (int i = 0; i < vk_world.num_image_chunks; i++) {
		// ensure that memory region has proper alignment
		VkDeviceSize offset = (vk_world.image_chunks[i].used + memory_requirements.alignment - 1) & mask;

		if (offset + memory_requirements.size <= IMAGE_CHUNK_SIZE) {
			chunk = &vk_world.image_chunks[i];
			chunk->used = offset + memory_requirements.size;
			break;
		}
	}

	// Allocate a new chunk in case we couldn't find suitable existing chunk.
	if (chunk == nullptr) {
		if (vk_world.num_image_chunks >= MAX_IMAGE_CHUNKS) {
			ri.Error(ERR_FATAL, "Vulkan: image chunk limit has been reached");
		}

		VkMemoryAllocateInfo alloc_info;
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.pNext = nullptr;
		alloc_info.allocationSize = IMAGE_CHUNK_SIZE;
		alloc_info.memoryTypeIndex = find_memory_type(vk.physical_device, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VkDeviceMemory memory;
		VK_CHECK(vkAllocateMemory(vk.device, &alloc_info, nullptr, &memory));

		chunk = &vk_world.image_chunks[vk_world.num_image_chunks];
		vk_world.num_image_chunks++;
		chunk->memory = memory;
		chunk->used = memory_requirements.size;
	}

	VK_CHECK(vkBindImageMemory(vk.device, image, chunk->memory, chunk->used - memory_requirements.size));
}

static void ensure_staging_buffer_allocation(VkDeviceSize size) {
	if (vk_world.staging_buffer_size >= size)
		return;

	if (vk_world.staging_buffer != VK_NULL_HANDLE)
		vkDestroyBuffer(vk.device, vk_world.staging_buffer, nullptr);

	if (vk_world.staging_buffer_memory != VK_NULL_HANDLE)
		vkFreeMemory(vk.device, vk_world.staging_buffer_memory, nullptr);

	vk_world.staging_buffer_size = size;

	VkBufferCreateInfo buffer_desc;
	buffer_desc.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_desc.pNext = nullptr;
	buffer_desc.flags = 0;
	buffer_desc.size = size;
	buffer_desc.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	buffer_desc.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buffer_desc.queueFamilyIndexCount = 0;
	buffer_desc.pQueueFamilyIndices = nullptr;
	VK_CHECK(vkCreateBuffer(vk.device, &buffer_desc, nullptr, &vk_world.staging_buffer));

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(vk.device, vk_world.staging_buffer, &memory_requirements);

	uint32_t memory_type = find_memory_type(vk.physical_device, memory_requirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkMemoryAllocateInfo alloc_info;
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.pNext = nullptr;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = memory_type;
	VK_CHECK(vkAllocateMemory(vk.device, &alloc_info, nullptr, &vk_world.staging_buffer_memory));
	VK_CHECK(vkBindBufferMemory(vk.device, vk_world.staging_buffer, vk_world.staging_buffer_memory, 0));

	void* data;
	VK_CHECK(vkMapMemory(vk.device, vk_world.staging_buffer_memory, 0, VK_WHOLE_SIZE, 0, &data));
	vk_world.staging_buffer_ptr = (byte*)data;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT object_type, uint64_t object, size_t location,
	int32_t message_code, const char* layer_prefix, const char* message, void* user_data) {
	
#ifdef _WIN32
	OutputDebugString(message);
	OutputDebugString("\n");
	DebugBreak();
#endif
	return VK_FALSE;
}

static void create_instance() {
	
    ri.VK_GetExtensions(&vk.instance, nullptr);
    //VK_CHECK(vkCreateInstance(desc, nullptr, &vk.instance));
/*
	const char* instance_extensions[] = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		//VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#ifndef NDEBUG
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#endif
	};

	// check extensions availability
	{
		uint32_t count = 0;
		VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
		std::vector<VkExtensionProperties> extension_properties(count);
		VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &count, extension_properties.data()));

		for (auto name : instance_extensions) {
			bool supported = false;
			for (const auto& property : extension_properties) {
				if (!strcmp(property.extensionName, name)) {
					supported = true;
					break;
				}
			}
			if (!supported)
				ri.Error(ERR_FATAL, "Vulkan: required instance extension is not available: %s", name);
		}
	}

	// create instance
	{
		VkInstanceCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		desc.pNext = nullptr;
		desc.flags = 0;
		desc.pApplicationInfo = nullptr;
		desc.enabledLayerCount = 0;
		desc.ppEnabledLayerNames = nullptr;
		desc.enabledExtensionCount = sizeof(instance_extensions)/sizeof(instance_extensions[0]);
		desc.ppEnabledExtensionNames = instance_extensions;

#ifndef NDEBUG
		const char* validation_layer_name = "VK_LAYER_KHRONOS_validation";
		desc.enabledLayerCount = 1;
		desc.ppEnabledLayerNames = &validation_layer_name;
#endif

		VK_CHECK(vkCreateInstance(&desc, nullptr, &vk.instance));
	}*/
}

static void create_device() {

	VkPhysicalDeviceDescriptorIndexingFeatures indexing_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES, nullptr };
	VkPhysicalDeviceFeatures2 device_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &indexing_features };

	// select physical device
	{
		uint32_t count;
		VK_CHECK(vkEnumeratePhysicalDevices(vk.instance, &count, nullptr));

		if (count == 0)
			ri.Error(PRINT_ERROR, "Vulkan: no physical device found");
        else
            ri.Printf( PRINT_ALL, "Found %i devices\n", count );
		std::vector<VkPhysicalDevice> physical_devices(count);
		VK_CHECK(vkEnumeratePhysicalDevices(vk.instance, &count, physical_devices.data()));
		vk.physical_device = physical_devices[0];
	}

	//Check bindless support
	{

		vkGetPhysicalDeviceFeatures2( vk.physical_device, &device_features );
		if(!indexing_features.descriptorBindingPartiallyBound || !indexing_features.runtimeDescriptorArray)
			ri.Error(ERR_FATAL, "Vulkan: bindless is not supported by the device!");
	}

	ri.VK_CreateSurface(&vk.instance, &vk.surface);

	// select surface format
	{
		uint32_t format_count;
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vk.physical_device, vk.surface, &format_count, nullptr));
		assert(format_count > 0);

		std::vector<VkSurfaceFormatKHR> candidates(format_count);
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vk.physical_device, vk.surface, &format_count, candidates.data()));

		
		if (candidates.size() == 1 && candidates[0].format == VK_FORMAT_UNDEFINED) { // special case that means we can choose any format
			vk.surface_format.format = VK_FORMAT_R8G8B8A8_UNORM;
			vk.surface_format.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		} else {
			vk.surface_format = candidates[0];
		}
	}

	// select queue family
	{
		uint32_t queue_family_count;
		vkGetPhysicalDeviceQueueFamilyProperties(vk.physical_device, &queue_family_count, nullptr);

		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(vk.physical_device, &queue_family_count, queue_families.data());

		// select queue family with presentation and graphics support
		vk.queue_family_index = -1;
		for (uint32_t i = 0; i < queue_family_count; i++) {
			VkBool32 presentation_supported;
			VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(vk.physical_device, i, vk.surface, &presentation_supported));

			if (presentation_supported && (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
				vk.queue_family_index = i;
				break;
			}
		}
		if (vk.queue_family_index == -1)
			ri.Error(ERR_FATAL, "Vulkan: failed to find queue family");
	}

	// create VkDevice
	{
		const char* device_extensions[] = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		uint32_t count = 0;
		VK_CHECK(vkEnumerateDeviceExtensionProperties(vk.physical_device, nullptr, &count, nullptr));
		std::vector<VkExtensionProperties> extension_properties(count);
		VK_CHECK(vkEnumerateDeviceExtensionProperties(vk.physical_device, nullptr, &count, extension_properties.data()));

		for (auto name : device_extensions) {
			bool supported = false;
			for (const auto& property : extension_properties) {
				if (!strcmp(property.extensionName, name)) {
					supported = true;
					break;
				}
			}
			if (!supported)
				ri.Error(ERR_FATAL, "Vulkan: required device extension is not available: %s", name);
		}

		const float priority = 1.0;
		VkDeviceQueueCreateInfo queue_desc;
		queue_desc.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_desc.pNext = nullptr;
		queue_desc.flags = 0;
		queue_desc.queueFamilyIndex = vk.queue_family_index;
		queue_desc.queueCount = 1;
		queue_desc.pQueuePriorities = &priority;

		VkPhysicalDeviceFeatures features;
		Com_Memset(&features, 0, sizeof(features));
		features.shaderClipDistance = VK_TRUE;
		features.fillModeNonSolid = VK_TRUE;
        features.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo device_desc;
		device_desc.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_desc.pNext = &device_features;
		device_desc.flags = 0;
		device_desc.queueCreateInfoCount = 1;
		device_desc.pQueueCreateInfos = &queue_desc;
		device_desc.enabledLayerCount = 0;
		device_desc.ppEnabledLayerNames = nullptr;
		device_desc.enabledExtensionCount = sizeof(device_extensions)/sizeof(device_extensions[0]);
		device_desc.ppEnabledExtensionNames = device_extensions;
		device_desc.pEnabledFeatures = NULL;
		//device_desc.pEnabledFeatures = &features;
		VK_CHECK(vkCreateDevice(vk.physical_device, &device_desc, nullptr, &vk.device));
	}
}

#define INIT_INSTANCE_FUNCTION(func) func = (PFN_ ## func)vkGetInstanceProcAddr(vk.instance, #func);
#define INIT_DEVICE_FUNCTION(func) func = (PFN_ ## func)vkGetDeviceProcAddr(vk.device, #func);

static void init_vulkan_library() {
	//
	// Get functions that do not depend on VkInstance (vk.instance == nullptr at this point).
	//
/*	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)SDL_Vulkan_GetVkGetInstanceProcAddr();
	INIT_INSTANCE_FUNCTION(vkCreateInstance)
	INIT_INSTANCE_FUNCTION(vkEnumerateInstanceExtensionProperties)*/

	//
	// Get instance level functions.
	//
	create_instance();
	ri.Printf(PRINT_DEVELOPER, "Instance created\n");
	/*INIT_INSTANCE_FUNCTION(vkCreateDevice)
	INIT_INSTANCE_FUNCTION(vkDestroyInstance)
	INIT_INSTANCE_FUNCTION(vkEnumerateDeviceExtensionProperties)
	INIT_INSTANCE_FUNCTION(vkEnumeratePhysicalDevices)
	INIT_INSTANCE_FUNCTION(vkGetDeviceProcAddr)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceFeatures)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceFormatProperties)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceMemoryProperties)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceProperties)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties)
	//INIT_INSTANCE_FUNCTION(vkCreateWin32SurfaceKHR)
	INIT_INSTANCE_FUNCTION(vkDestroySurfaceKHR)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR)
	INIT_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR)
	INIT_INSTANCE_FUNCTION(vkCreateDebugReportCallbackEXT)
	INIT_INSTANCE_FUNCTION(vkDestroyDebugReportCallbackEXT)*/

	//
	// Create debug callback.
	// 
#if 0
	{
		VkDebugReportCallbackCreateInfoEXT desc;
		desc.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		desc.pNext = nullptr;
		desc.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT |
					 VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
					 VK_DEBUG_REPORT_ERROR_BIT_EXT;
		desc.pfnCallback = &debug_callback;
		desc.pUserData = nullptr;

		VK_CHECK(vkCreateDebugReportCallbackEXT(vk.instance, &desc, nullptr, &vk.debug_callback));
	}
#endif

	//
	// Get device level functions.
	//
	create_device();
	/*INIT_DEVICE_FUNCTION(vkAllocateCommandBuffers)
	INIT_DEVICE_FUNCTION(vkAllocateDescriptorSets)
	INIT_DEVICE_FUNCTION(vkAllocateMemory)
	INIT_DEVICE_FUNCTION(vkBeginCommandBuffer)
	INIT_DEVICE_FUNCTION(vkBindBufferMemory)
	INIT_DEVICE_FUNCTION(vkBindImageMemory)
	INIT_DEVICE_FUNCTION(vkCmdBeginRenderPass)
	INIT_DEVICE_FUNCTION(vkCmdBindDescriptorSets)
	INIT_DEVICE_FUNCTION(vkCmdBindIndexBuffer)
	INIT_DEVICE_FUNCTION(vkCmdBindPipeline)
	INIT_DEVICE_FUNCTION(vkCmdBindVertexBuffers)
	INIT_DEVICE_FUNCTION(vkCmdBlitImage)
	INIT_DEVICE_FUNCTION(vkCmdClearAttachments)
	INIT_DEVICE_FUNCTION(vkCmdCopyBufferToImage)
	INIT_DEVICE_FUNCTION(vkCmdCopyImage)
	INIT_DEVICE_FUNCTION(vkCmdDraw)
	INIT_DEVICE_FUNCTION(vkCmdDrawIndexed)
	INIT_DEVICE_FUNCTION(vkCmdEndRenderPass)
	INIT_DEVICE_FUNCTION(vkCmdPipelineBarrier)
	INIT_DEVICE_FUNCTION(vkCmdPushConstants)
	INIT_DEVICE_FUNCTION(vkCmdSetDepthBias)
	INIT_DEVICE_FUNCTION(vkCmdSetScissor)
	INIT_DEVICE_FUNCTION(vkCmdSetViewport)
	INIT_DEVICE_FUNCTION(vkCreateBuffer)
	INIT_DEVICE_FUNCTION(vkCreateCommandPool)
	INIT_DEVICE_FUNCTION(vkCreateDescriptorPool)
	INIT_DEVICE_FUNCTION(vkCreateDescriptorSetLayout)
	INIT_DEVICE_FUNCTION(vkCreateFence)
	INIT_DEVICE_FUNCTION(vkCreateFramebuffer)
	INIT_DEVICE_FUNCTION(vkCreateGraphicsPipelines)
	INIT_DEVICE_FUNCTION(vkCreateImage)
	INIT_DEVICE_FUNCTION(vkCreateImageView)
	INIT_DEVICE_FUNCTION(vkCreatePipelineLayout)
	INIT_DEVICE_FUNCTION(vkCreateRenderPass)
	INIT_DEVICE_FUNCTION(vkCreateSampler)
	INIT_DEVICE_FUNCTION(vkCreateSemaphore)
	INIT_DEVICE_FUNCTION(vkCreateShaderModule)
	INIT_DEVICE_FUNCTION(vkDestroyBuffer)
	INIT_DEVICE_FUNCTION(vkDestroyCommandPool)
	INIT_DEVICE_FUNCTION(vkDestroyDescriptorPool)
	INIT_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout)
	INIT_DEVICE_FUNCTION(vkDestroyDevice)
	INIT_DEVICE_FUNCTION(vkDestroyFence)
	INIT_DEVICE_FUNCTION(vkDestroyFramebuffer)
	INIT_DEVICE_FUNCTION(vkDestroyImage)
	INIT_DEVICE_FUNCTION(vkDestroyImageView)
	INIT_DEVICE_FUNCTION(vkDestroyPipeline)
	INIT_DEVICE_FUNCTION(vkDestroyPipelineLayout)
	INIT_DEVICE_FUNCTION(vkDestroyRenderPass)
	INIT_DEVICE_FUNCTION(vkDestroySampler)
	INIT_DEVICE_FUNCTION(vkDestroySemaphore)
	INIT_DEVICE_FUNCTION(vkDestroyShaderModule)
	INIT_DEVICE_FUNCTION(vkDeviceWaitIdle)
	INIT_DEVICE_FUNCTION(vkEndCommandBuffer)
	INIT_DEVICE_FUNCTION(vkFreeCommandBuffers)
	INIT_DEVICE_FUNCTION(vkFreeDescriptorSets)
	INIT_DEVICE_FUNCTION(vkFreeMemory)
	INIT_DEVICE_FUNCTION(vkGetBufferMemoryRequirements)
	INIT_DEVICE_FUNCTION(vkGetDeviceQueue)
	INIT_DEVICE_FUNCTION(vkGetImageMemoryRequirements)
	INIT_DEVICE_FUNCTION(vkGetImageSubresourceLayout)
	INIT_DEVICE_FUNCTION(vkMapMemory)
	INIT_DEVICE_FUNCTION(vkQueueSubmit)
	INIT_DEVICE_FUNCTION(vkQueueWaitIdle)
	INIT_DEVICE_FUNCTION(vkResetDescriptorPool)
	INIT_DEVICE_FUNCTION(vkResetFences)
	INIT_DEVICE_FUNCTION(vkUpdateDescriptorSets)
	INIT_DEVICE_FUNCTION(vkWaitForFences)
	INIT_DEVICE_FUNCTION(vkAcquireNextImageKHR)
	INIT_DEVICE_FUNCTION(vkCreateSwapchainKHR)
	INIT_DEVICE_FUNCTION(vkDestroySwapchainKHR)
	INIT_DEVICE_FUNCTION(vkGetSwapchainImagesKHR)
	INIT_DEVICE_FUNCTION(vkQueuePresentKHR)*/
}

#undef INIT_INSTANCE_FUNCTION
#undef INIT_DEVICE_FUNCTION

static void deinit_vulkan_library() {
	/*vkCreateInstance                            = nullptr;
	vkEnumerateInstanceExtensionProperties		= nullptr;

	vkCreateDevice								= nullptr;
	vkDestroyInstance							= nullptr;
	vkEnumerateDeviceExtensionProperties		= nullptr;
	vkEnumeratePhysicalDevices					= nullptr;
	vkGetDeviceProcAddr							= nullptr;
	vkGetPhysicalDeviceFeatures					= nullptr;
	vkGetPhysicalDeviceFormatProperties			= nullptr;
	vkGetPhysicalDeviceMemoryProperties			= nullptr;
	vkGetPhysicalDeviceProperties				= nullptr;
	vkGetPhysicalDeviceQueueFamilyProperties	= nullptr;
	//vkCreateWin32SurfaceKHR						= nullptr;
	vkDestroySurfaceKHR							= nullptr;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR	= nullptr;
	vkGetPhysicalDeviceSurfaceFormatsKHR		= nullptr;
	vkGetPhysicalDeviceSurfacePresentModesKHR	= nullptr;
	vkGetPhysicalDeviceSurfaceSupportKHR		= nullptr;
#ifndef NDEBUG
	vkCreateDebugReportCallbackEXT				= nullptr;
	vkDestroyDebugReportCallbackEXT				= nullptr;
#endif

	vkAllocateCommandBuffers					= nullptr;
	vkAllocateDescriptorSets					= nullptr;
	vkAllocateMemory							= nullptr;
	vkBeginCommandBuffer						= nullptr;
	vkBindBufferMemory							= nullptr;
	vkBindImageMemory							= nullptr;
	vkCmdBeginRenderPass						= nullptr;
	vkCmdBindDescriptorSets						= nullptr;
	vkCmdBindIndexBuffer						= nullptr;
	vkCmdBindPipeline							= nullptr;
	vkCmdBindVertexBuffers						= nullptr;
	vkCmdBlitImage								= nullptr;
	vkCmdClearAttachments						= nullptr;
	vkCmdCopyBufferToImage						= nullptr;
	vkCmdCopyImage								= nullptr;
	vkCmdDraw									= nullptr;
	vkCmdDrawIndexed							= nullptr;
	vkCmdEndRenderPass							= nullptr;
	vkCmdPipelineBarrier						= nullptr;
	vkCmdPushConstants							= nullptr;
	vkCmdSetDepthBias							= nullptr;
	vkCmdSetScissor								= nullptr;
	vkCmdSetViewport							= nullptr;
	vkCreateBuffer								= nullptr;
	vkCreateCommandPool							= nullptr;
	vkCreateDescriptorPool						= nullptr;
	vkCreateDescriptorSetLayout					= nullptr;
	vkCreateFence								= nullptr;
	vkCreateFramebuffer							= nullptr;
	vkCreateGraphicsPipelines					= nullptr;
	vkCreateImage								= nullptr;
	vkCreateImageView							= nullptr;
	vkCreatePipelineLayout						= nullptr;
	vkCreateRenderPass							= nullptr;
	vkCreateSampler								= nullptr;
	vkCreateSemaphore							= nullptr;
	vkCreateShaderModule						= nullptr;
	vkDestroyBuffer								= nullptr;
	vkDestroyCommandPool						= nullptr;
	vkDestroyDescriptorPool						= nullptr;
	vkDestroyDescriptorSetLayout				= nullptr;
	vkDestroyDevice								= nullptr;
	vkDestroyFence								= nullptr;
	vkDestroyFramebuffer						= nullptr;
	vkDestroyImage								= nullptr;
	vkDestroyImageView							= nullptr;
	vkDestroyPipeline							= nullptr;
	vkDestroyPipelineLayout						= nullptr;
	vkDestroyRenderPass							= nullptr;
	vkDestroySampler							= nullptr;
	vkDestroySemaphore							= nullptr;
	vkDestroyShaderModule						= nullptr;
	vkDeviceWaitIdle							= nullptr;
	vkEndCommandBuffer							= nullptr;
	vkFreeCommandBuffers						= nullptr;
	vkFreeDescriptorSets						= nullptr;
	vkFreeMemory								= nullptr;
	vkGetBufferMemoryRequirements				= nullptr;
	vkGetDeviceQueue							= nullptr;
	vkGetImageMemoryRequirements				= nullptr;
	vkGetImageSubresourceLayout					= nullptr;
	vkMapMemory									= nullptr;
	vkQueueSubmit								= nullptr;
	vkQueueWaitIdle								= nullptr;
	vkResetDescriptorPool						= nullptr;
	vkResetFences								= nullptr;
	vkUpdateDescriptorSets						= nullptr;
	vkWaitForFences								= nullptr;
	vkAcquireNextImageKHR						= nullptr;
	vkCreateSwapchainKHR						= nullptr;
	vkDestroySwapchainKHR						= nullptr;
	vkGetSwapchainImagesKHR						= nullptr;
	vkQueuePresentKHR							= nullptr;*/
}

static VkPipeline create_pipeline(const Vk_Pipeline_Def&);

void vk_initialize() {
	init_vulkan_library();

	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(vk.physical_device, &features);
	if (features.shaderClipDistance == VK_FALSE)
		ri.Error(ERR_FATAL, "vk_create_instance: shaderClipDistance feature is not supported");
	if (features.fillModeNonSolid == VK_FALSE)
		ri.Error(ERR_FATAL, "vk_create_instance: fillModeNonSolid feature is not supported");

	vkGetDeviceQueue(vk.device, vk.queue_family_index, 0, &vk.queue);
    //Hardware Info
    {
        
        VkPhysicalDeviceProperties props;
        char * vk_vendor_string;
        
        vkGetPhysicalDeviceProperties(vk.physical_device, &props);
        ri.Printf( PRINT_ALL, "Device : %s\n", props.deviceName);
		glConfig.maxTextureSize = props.limits.maxImageDimension2D;
    }
	//
	// Swapchain.
	//
	{
		vk.swapchain = create_swapchain(vk.physical_device, vk.device, vk.surface, vk.surface_format);

		VK_CHECK(vkGetSwapchainImagesKHR(vk.device, vk.swapchain, &vk.swapchain_image_count, nullptr));
		vk.swapchain_image_count = std::min(vk.swapchain_image_count, (uint32_t)MAX_SWAPCHAIN_IMAGES);
		VK_CHECK(vkGetSwapchainImagesKHR(vk.device, vk.swapchain, &vk.swapchain_image_count, vk.swapchain_images));

		for (uint32_t i = 0; i < vk.swapchain_image_count; i++) {
			VkImageViewCreateInfo desc;
			desc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			desc.pNext = nullptr;
			desc.flags = 0;
			desc.image = vk.swapchain_images[i];
			desc.viewType = VK_IMAGE_VIEW_TYPE_2D;
			desc.format = vk.surface_format.format;
			desc.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			desc.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			desc.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			desc.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			desc.subresourceRange.baseMipLevel = 0;
			desc.subresourceRange.levelCount = 1;
			desc.subresourceRange.baseArrayLayer = 0;
			desc.subresourceRange.layerCount = 1;
			VK_CHECK(vkCreateImageView(vk.device, &desc, nullptr, &vk.swapchain_image_views[i]));
		}
	}

	//
	// Sync primitives.
	//
	{
		VkSemaphoreCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		desc.pNext = nullptr;
		desc.flags = 0;

		VK_CHECK(vkCreateSemaphore(vk.device, &desc, nullptr, &vk.image_acquired));
		VK_CHECK(vkCreateSemaphore(vk.device, &desc, nullptr, &vk.rendering_finished));

		VkFenceCreateInfo fence_desc;
		fence_desc.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_desc.pNext = nullptr;
		fence_desc.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VK_CHECK(vkCreateFence(vk.device, &fence_desc, nullptr, &vk.rendering_finished_fence));
	}

	//
	// Command pool.
	//
	{
		VkCommandPoolCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		desc.pNext = nullptr;
		desc.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		desc.queueFamilyIndex = vk.queue_family_index;

		VK_CHECK(vkCreateCommandPool(vk.device, &desc, nullptr, &vk.command_pool));
	}

	//
	// Command buffer.
	//
	{
		VkCommandBufferAllocateInfo alloc_info;
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.pNext = nullptr;
		alloc_info.commandPool = vk.command_pool;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandBufferCount = 1;
		VK_CHECK(vkAllocateCommandBuffers(vk.device, &alloc_info, &vk.command_buffer));
	}

	//
	// Depth attachment image.
	// 
	{
		VkFormat depth_format = get_depth_format(vk.physical_device);

		// create depth image
		{
			VkImageCreateInfo desc;
			desc.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			desc.pNext = nullptr;
			desc.flags = 0;
			desc.imageType = VK_IMAGE_TYPE_2D;
			desc.format = depth_format;
			desc.extent.width = glConfig.vidWidth;
			desc.extent.height = glConfig.vidHeight;
			desc.extent.depth = 1;
			desc.mipLevels = 1;
			desc.arrayLayers = 1;
			desc.samples = VK_SAMPLE_COUNT_1_BIT;
			desc.tiling = VK_IMAGE_TILING_OPTIMAL;
			desc.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			desc.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			desc.queueFamilyIndexCount = 0;
			desc.pQueueFamilyIndices = nullptr;
			desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VK_CHECK(vkCreateImage(vk.device, &desc, nullptr, &vk.depth_image));
		}

		// allocate depth image memory
		{
			VkMemoryRequirements memory_requirements;
			vkGetImageMemoryRequirements(vk.device, vk.depth_image, &memory_requirements);

			VkMemoryAllocateInfo alloc_info;
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.pNext = nullptr;
			alloc_info.allocationSize = memory_requirements.size;
			alloc_info.memoryTypeIndex = find_memory_type(vk.physical_device, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VK_CHECK(vkAllocateMemory(vk.device, &alloc_info, nullptr, &vk.depth_image_memory));
			VK_CHECK(vkBindImageMemory(vk.device, vk.depth_image, vk.depth_image_memory, 0));
		}

		// create depth image view
		{
			VkImageViewCreateInfo desc;
			desc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			desc.pNext = nullptr;
			desc.flags = 0;
			desc.image = vk.depth_image;
			desc.viewType = VK_IMAGE_VIEW_TYPE_2D;
			desc.format = depth_format;
			desc.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			desc.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			desc.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			desc.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			desc.subresourceRange.baseMipLevel = 0;
			desc.subresourceRange.levelCount = 1;
			desc.subresourceRange.baseArrayLayer = 0;
			desc.subresourceRange.layerCount = 1;
			VK_CHECK(vkCreateImageView(vk.device, &desc, nullptr, &vk.depth_image_view));
		}

		VkImageAspectFlags image_aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (r_stencilbits->integer)
			image_aspect_flags |= VK_IMAGE_ASPECT_STENCIL_BIT;

		record_and_run_commands(vk.command_pool, vk.queue, [&image_aspect_flags](VkCommandBuffer command_buffer) {
			record_image_layout_transition(command_buffer, vk.depth_image, image_aspect_flags, 0, VK_IMAGE_LAYOUT_UNDEFINED,
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		});
	}

	//
	// Renderpass.
	//
	{
		VkFormat depth_format = get_depth_format(vk.physical_device);
		vk.render_pass = create_render_pass(vk.device, vk.surface_format.format, depth_format);
	}

	//
	// Framebuffers for each swapchain image.
	//
	{
		VkImageView attachments[2] = {VK_NULL_HANDLE, vk.depth_image_view};

		VkFramebufferCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		desc.pNext = nullptr;
		desc.flags = 0;
		desc.renderPass = vk.render_pass;
		desc.attachmentCount = 2;
		desc.pAttachments = attachments;
		desc.width = glConfig.vidWidth;
		desc.height = glConfig.vidHeight;
		desc.layers = 1;

		for (uint32_t i = 0; i < vk.swapchain_image_count; i++) {
			attachments[0] = vk.swapchain_image_views[i]; // set color attachment
			VK_CHECK(vkCreateFramebuffer(vk.device, &desc, nullptr, &vk.framebuffers[i]));
		}
	}

	//
	// Descriptor pool.
	//
	{
		VkDescriptorPoolSize pool_size;
		pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_size.descriptorCount = MAX_DRAWIMAGES;

		VkDescriptorPoolCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		desc.pNext = nullptr;
		desc.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT; // used by the cinematic images
		desc.maxSets = MAX_DRAWIMAGES;
		desc.poolSizeCount = 1;
		desc.pPoolSizes = &pool_size;

		VK_CHECK(vkCreateDescriptorPool(vk.device, &desc, nullptr, &vk.descriptor_pool));
	}

	//
	// Descriptor set layout.
	//
	{
		VkDescriptorSetLayoutBinding descriptor_binding;
		descriptor_binding.binding = 0;
		descriptor_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_binding.descriptorCount = MAX_VK_IMAGES;
		descriptor_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptor_binding.pImmutableSamplers = nullptr;

		VkDescriptorBindingFlags bindless_flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | /*VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT |*/ VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
		VkDescriptorBindingFlags binding_flags[ 1 ];

		binding_flags[ 0 ] = bindless_flags;

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extended_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT, nullptr };
		extended_info.bindingCount = 1;
		extended_info.pBindingFlags = binding_flags;

		VkDescriptorSetLayoutCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		desc.pNext = &extended_info;
		desc.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
		desc.bindingCount = 1;
		desc.pBindings = &descriptor_binding;

		VK_CHECK(vkCreateDescriptorSetLayout(vk.device, &desc, nullptr, &vk.set_layout));
	}

	// create associated descriptor set
	{
		VkDescriptorSetAllocateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		desc.pNext = nullptr;
		desc.descriptorPool = vk.descriptor_pool;
		desc.descriptorSetCount = 1;
		desc.pSetLayouts = &vk.set_layout;
		VK_CHECK(vkAllocateDescriptorSets(vk.device, &desc, &vk.bindless_descriptor_set));
	}

	//
	// Pipeline layout.
	//
	{
		VkPushConstantRange push_range;
		push_range.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;
		push_range.offset = 0;
		push_range.size = (VK_PUSH_CONSTANT_SIZE * 4); // 36 floats

		VkDescriptorSetLayout set_layouts[1] = {vk.set_layout };

		VkPipelineLayoutCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		desc.pNext = nullptr;
		desc.flags = 0;
		desc.setLayoutCount = 1;
		desc.pSetLayouts = set_layouts;
		desc.pushConstantRangeCount = 1;
		desc.pPushConstantRanges = &push_range;

		VK_CHECK(vkCreatePipelineLayout(vk.device, &desc, nullptr, &vk.pipeline_layout));
	}

	//
	// Geometry buffers.
	//
	{
		VkBufferCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		desc.pNext = nullptr;
		desc.flags = 0;
		desc.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		desc.queueFamilyIndexCount = 0;
		desc.pQueueFamilyIndices = nullptr;

		desc.size = VERTEX_BUFFER_SIZE;
		desc.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		VK_CHECK(vkCreateBuffer(vk.device, &desc, nullptr, &vk.vertex_buffer));

		desc.size = INDEX_BUFFER_SIZE;
		desc.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		VK_CHECK(vkCreateBuffer(vk.device, &desc, nullptr, &vk.index_buffer));

		VkMemoryRequirements vb_memory_requirements;
		vkGetBufferMemoryRequirements(vk.device, vk.vertex_buffer, &vb_memory_requirements);

		VkMemoryRequirements ib_memory_requirements;
		vkGetBufferMemoryRequirements(vk.device, vk.index_buffer, &ib_memory_requirements);

		VkDeviceSize mask = ~(ib_memory_requirements.alignment - 1);
		VkDeviceSize index_buffer_offset = (vb_memory_requirements.size + ib_memory_requirements.alignment - 1) & mask;

		uint32_t memory_type_bits = vb_memory_requirements.memoryTypeBits & ib_memory_requirements.memoryTypeBits;
		uint32_t memory_type = find_memory_type(vk.physical_device, memory_type_bits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VkMemoryAllocateInfo alloc_info;
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.pNext = nullptr;
		alloc_info.allocationSize = index_buffer_offset + ib_memory_requirements.size;
		alloc_info.memoryTypeIndex = memory_type;
		VK_CHECK(vkAllocateMemory(vk.device, &alloc_info, nullptr, &vk.geometry_buffer_memory));

		vkBindBufferMemory(vk.device, vk.vertex_buffer, vk.geometry_buffer_memory, 0);
		vkBindBufferMemory(vk.device, vk.index_buffer, vk.geometry_buffer_memory, index_buffer_offset);

		void* data;
		VK_CHECK(vkMapMemory(vk.device, vk.geometry_buffer_memory, 0, VK_WHOLE_SIZE, 0, &data));
		vk.vertex_buffer_ptr = (byte*)data;
		vk.index_buffer_ptr = (byte*)data + index_buffer_offset;
	}

	//
	// Shader modules.
	//
	{
		vk.single_texture_vs = vk_create_shader_module_from_file("single_texture.vert");
		vk.single_texture_clipping_plane_vs = vk_create_shader_module_from_file("single_texture_clipping_plane.vert");
		vk.single_texture_fs = vk_create_shader_module_from_file("single_texture.frag");
		vk.multi_texture_vs = vk_create_shader_module_from_file("multi_texture.vert");
		vk.multi_texture_clipping_plane_vs = vk_create_shader_module_from_file("multi_texture_clipping_plane.vert");
		vk.multi_texture_mul_fs = vk_create_shader_module_from_file("multi_texture_mul.frag");
		vk.multi_texture_add_fs = vk_create_shader_module_from_file("multi_texture_add.frag");
	}

	//
	// Standard pipelines.
	//
	{
		// skybox
		{
			Vk_Pipeline_Def def;
			def.shader_type = Vk_Shader_Type::single_texture;
			def.state_bits = 0;
			def.face_culling = CT_FRONT_SIDED;
			def.polygon_offset = false;
			def.clipping_plane = false;
			def.mirror = false;
			vk.skybox_pipeline = create_pipeline(def);
		}

		// Q3 stencil shadows
		{
			{
				Vk_Pipeline_Def def;
				def.polygon_offset = false;
				def.state_bits = 0;
				def.shader_type = Vk_Shader_Type::single_texture;
				def.clipping_plane = false;
				def.shadow_phase = Vk_Shadow_Phase::shadow_edges_rendering;

				cullType_t cull_types[2] = {CT_FRONT_SIDED, CT_BACK_SIDED};
				bool mirror_flags[2] = {false, true};

				for (int i = 0; i < 2; i++) {
					def.face_culling = cull_types[i];
					for (int j = 0; j < 2; j++) {
						def.mirror = mirror_flags[j];
						vk.shadow_volume_pipelines[i][j] = create_pipeline(def);
					}
				}
			}

			{
				Vk_Pipeline_Def def;
				def.face_culling = CT_FRONT_SIDED;
				def.polygon_offset = false;
				def.state_bits = GLS_DEPTHMASK_TRUE | GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_ZERO;
				def.shader_type = Vk_Shader_Type::single_texture;
				def.clipping_plane = false;
				def.mirror = false;
				def.shadow_phase = Vk_Shadow_Phase::fullscreen_quad_rendering;
				vk.shadow_finish_pipeline = create_pipeline(def);
			}
		}

		// fog and dlights
		{
			Vk_Pipeline_Def def;
			def.shader_type = Vk_Shader_Type::single_texture;
			def.clipping_plane = false;
			def.mirror = false;

			unsigned int fog_state_bits[2] = {
				GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA | GLS_DEPTHFUNC_EQUAL,
				GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA
			};
			unsigned int dlight_state_bits[2] = {
				GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_EQUAL,
				GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_EQUAL
			};
			bool polygon_offset[2] = {false, true};

			for (int i = 0; i < 2; i++) {
				unsigned fog_state = fog_state_bits[i];
				unsigned dlight_state = dlight_state_bits[i];

				for (int j = 0; j < 3; j++) {
					def.face_culling = j; // cullType_t value

					for (int k = 0; k < 2; k++) {
						def.polygon_offset = polygon_offset[k];

						def.state_bits = fog_state;
						vk.fog_pipelines[i][j][k] = create_pipeline(def);

						def.state_bits = dlight_state;
						vk.dlight_pipelines[i][j][k] = create_pipeline(def);
					}
				}
			}
		}

		// debug pipelines
		{
			Vk_Pipeline_Def def;
			def.state_bits = GLS_POLYMODE_LINE | GLS_DEPTHMASK_TRUE;
			vk.tris_debug_pipeline = create_pipeline(def);
		}
		{
			Vk_Pipeline_Def def;
			def.state_bits = GLS_POLYMODE_LINE | GLS_DEPTHMASK_TRUE;
			def.face_culling = CT_BACK_SIDED;
			vk.tris_mirror_debug_pipeline = create_pipeline(def);
		}
		{
			Vk_Pipeline_Def def;
			def.state_bits = GLS_DEPTHMASK_TRUE;
			def.line_primitives = true;
			vk.normals_debug_pipeline = create_pipeline(def);
		}
		{
			Vk_Pipeline_Def def;
			def.state_bits = GLS_DEPTHMASK_TRUE | GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE;
			vk.surface_debug_pipeline_solid = create_pipeline(def);
		}
		{
			Vk_Pipeline_Def def;
			def.state_bits = GLS_POLYMODE_LINE | GLS_DEPTHMASK_TRUE | GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE;
			def.line_primitives = true;
			vk.surface_debug_pipeline_outline = create_pipeline(def);
		}
		{
			Vk_Pipeline_Def def;
			def.state_bits = GLS_DEPTHTEST_DISABLE | GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA;
			vk.images_debug_pipeline = create_pipeline(def);
		}
	}
	vk.active = true;
}

VkShaderModule
vk_create_shader_module_from_file(const char *name)
{
    char path[1024];
    snprintf(path, sizeof path, SHADER_PATH_TEMPLATE, name);

    char *data;
    size_t size;

    ri.Printf(PRINT_ALL, "Loading shader module %s!\n", path );

    size = ri.FS_ReadFile ( path, (void **)&data);
    if(!data) {
        ri.Printf(PRINT_WARNING, "Couldn't find shader module %s!\n", path );
        return VK_NULL_HANDLE;
    }

    VkShaderModule module;

    VkShaderModuleCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = size,
            .pCode = (uint32_t *) data,
    };

    VK_CHECK(vkCreateShaderModule(vk.device, &create_info, NULL, &module));

    ri.Z_Free(data);

    return module;
}

void vk_shutdown() {
	vkDestroyImage(vk.device, vk.depth_image, nullptr);
	vkFreeMemory(vk.device, vk.depth_image_memory, nullptr);
	vkDestroyImageView(vk.device, vk.depth_image_view, nullptr);

	for (uint32_t i = 0; i < vk.swapchain_image_count; i++)
		vkDestroyFramebuffer(vk.device, vk.framebuffers[i], nullptr);

	vkDestroyRenderPass(vk.device, vk.render_pass, nullptr);

	vkDestroyCommandPool(vk.device, vk.command_pool, nullptr);

	for (uint32_t i = 0; i < vk.swapchain_image_count; i++)
		vkDestroyImageView(vk.device, vk.swapchain_image_views[i], nullptr);

	vkDestroyDescriptorPool(vk.device, vk.descriptor_pool, nullptr);
	vkDestroyDescriptorSetLayout(vk.device, vk.set_layout, nullptr);
	vkDestroyPipelineLayout(vk.device, vk.pipeline_layout, nullptr);
	vkDestroyBuffer(vk.device, vk.vertex_buffer, nullptr);
	vkDestroyBuffer(vk.device, vk.index_buffer, nullptr);
	vkFreeMemory(vk.device, vk.geometry_buffer_memory, nullptr);
	vkDestroySemaphore(vk.device, vk.image_acquired, nullptr);
	vkDestroySemaphore(vk.device, vk.rendering_finished, nullptr);
	vkDestroyFence(vk.device, vk.rendering_finished_fence, nullptr);

	vkDestroyShaderModule(vk.device, vk.single_texture_vs, nullptr);
	vkDestroyShaderModule(vk.device, vk.single_texture_clipping_plane_vs, nullptr);
	vkDestroyShaderModule(vk.device, vk.single_texture_fs, nullptr);
	vkDestroyShaderModule(vk.device, vk.multi_texture_vs, nullptr);
	vkDestroyShaderModule(vk.device, vk.multi_texture_clipping_plane_vs, nullptr);
	vkDestroyShaderModule(vk.device, vk.multi_texture_mul_fs, nullptr);
	vkDestroyShaderModule(vk.device, vk.multi_texture_add_fs, nullptr);

	vkDestroyPipeline(vk.device, vk.skybox_pipeline, nullptr);
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++) {
			vkDestroyPipeline(vk.device, vk.shadow_volume_pipelines[i][j], nullptr);
		}
	vkDestroyPipeline(vk.device, vk.shadow_finish_pipeline, nullptr);
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 2; k++) {
				vkDestroyPipeline(vk.device, vk.fog_pipelines[i][j][k], nullptr);
				vkDestroyPipeline(vk.device, vk.dlight_pipelines[i][j][k], nullptr);
			}
	vkDestroyPipeline(vk.device, vk.tris_debug_pipeline, nullptr);
	vkDestroyPipeline(vk.device, vk.tris_mirror_debug_pipeline, nullptr);
	vkDestroyPipeline(vk.device, vk.normals_debug_pipeline, nullptr);
	vkDestroyPipeline(vk.device, vk.surface_debug_pipeline_solid, nullptr);
	vkDestroyPipeline(vk.device, vk.surface_debug_pipeline_outline, nullptr);
	vkDestroyPipeline(vk.device, vk.images_debug_pipeline, nullptr);

	vkDestroySwapchainKHR(vk.device, vk.swapchain, nullptr);
	vkDestroyDevice(vk.device, nullptr);
	vkDestroySurfaceKHR(vk.instance, vk.surface, nullptr);

#if 0
	vkDestroyDebugReportCallbackEXT(vk.instance, vk.debug_callback, nullptr);
#endif

	vkDestroyInstance(vk.instance, nullptr);

	Com_Memset(&vk, 0, sizeof(vk));
	deinit_vulkan_library();
}

void vk_release_resources() {
	vkDeviceWaitIdle(vk.device);

	for (int i = 0; i < vk_world.num_image_chunks; i++)
		vkFreeMemory(vk.device, vk_world.image_chunks[i].memory, nullptr);

	if (vk_world.staging_buffer != VK_NULL_HANDLE)
		vkDestroyBuffer(vk.device, vk_world.staging_buffer, nullptr);

	if (vk_world.staging_buffer_memory != VK_NULL_HANDLE)
		vkFreeMemory(vk.device, vk_world.staging_buffer_memory, nullptr);

	for (int i = 0; i < vk_world.num_samplers; i++)
		vkDestroySampler(vk.device, vk_world.samplers[i], nullptr);

	for (int i = 0; i < vk_world.num_pipelines; i++)
		vkDestroyPipeline(vk.device, vk_world.pipelines[i], nullptr);

	vk_world.pipeline_create_time = 0.0f;

	for (int i = 0; i < MAX_VK_IMAGES; i++) {
		Vk_Image& image = vk_world.images[i];

		if (image.handle != VK_NULL_HANDLE) {
			vkDestroyImage(vk.device, image.handle, nullptr);
			vkDestroyImageView(vk.device, image.view, nullptr);
		}
	}

	Com_Memset(&vk_world, 0, sizeof(vk_world));

	VK_CHECK(vkResetDescriptorPool(vk.device, vk.descriptor_pool, 0));

	// Reset geometry buffer's current offsets.
	vk.xyz_elements = 0;
	vk.color_st_elements = 0;
	vk.index_buffer_offset = 0;
}

void vk_clear_images(){
/*
    vkDeviceWaitIdle(vk.device);
    for (int i = 0; i < MAX_VK_IMAGES; i++) {
		Vk_Image& image = vk_world.images[i];

		if (image.handle != VK_NULL_HANDLE) {
			vkDestroyImage(vk.device, image.handle, nullptr);
			vkDestroyImageView(vk.device, image.view, nullptr);
		}
	}
	vk_world.num_images = 0;*/
}

static void record_buffer_memory_barrier(VkCommandBuffer cb, VkBuffer buffer,
		VkPipelineStageFlags src_stages, VkPipelineStageFlags dst_stages,
		VkAccessFlags src_access, VkAccessFlags dst_access) {

	VkBufferMemoryBarrier barrier;
	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;;
	barrier.pNext = nullptr;
	barrier.srcAccessMask = src_access;
	barrier.dstAccessMask = dst_access;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.buffer = buffer;
	barrier.offset = 0;
	barrier.size = VK_WHOLE_SIZE;

	vkCmdPipelineBarrier(cb, src_stages, dst_stages, 0, 0, nullptr, 1, &barrier, 0, nullptr);
}

Vk_Image vk_create_image(int width, int height, VkFormat format, int mip_levels, bool repeat_texture, int index) {
	Vk_Image image;

	// create image
	{
		VkImageCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		desc.pNext = nullptr;
		desc.flags = 0;
		desc.imageType = VK_IMAGE_TYPE_2D;
		desc.format = format;
		desc.extent.width = width;
		desc.extent.height = height;
		desc.extent.depth = 1;
		desc.mipLevels = mip_levels;
		desc.arrayLayers = 1;
		desc.samples = VK_SAMPLE_COUNT_1_BIT;
		desc.tiling = VK_IMAGE_TILING_OPTIMAL;
		desc.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		desc.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		desc.queueFamilyIndexCount = 0;
		desc.pQueueFamilyIndices = nullptr;
		desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VK_CHECK(vkCreateImage(vk.device, &desc, nullptr, &image.handle));
		allocate_and_bind_image_memory(image.handle);
	}

	// create image view
	{
		VkImageViewCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		desc.pNext = nullptr;
		desc.flags = 0;
		desc.image = image.handle;
		desc.viewType = VK_IMAGE_VIEW_TYPE_2D;
		desc.format = format;
		desc.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		desc.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		desc.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		desc.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		desc.subresourceRange.baseMipLevel = 0;
		desc.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		desc.subresourceRange.baseArrayLayer = 0;
		desc.subresourceRange.layerCount = 1;
		VK_CHECK(vkCreateImageView(vk.device, &desc, nullptr, &image.view));
	}

	image.mipmap = mip_levels > 1;
	image.repeat_texture = repeat_texture;
	// create associated descriptor set
	{
		//vk_world.current_descriptor_sets[glState.currenttmu] = image.descriptor_set;
		images_to_update_bindless.push_back(index);
	}

	return image;
}

void vk_upload_image_data(VkImage image, int width, int height, bool mipmap, const uint8_t* pixels, int bytes_per_pixel) {
	VkBufferImageCopy regions[16];
	int num_regions = 0;

	int buffer_size = 0;

	while (true) {
		VkBufferImageCopy region;
		region.bufferOffset = buffer_size;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = num_regions;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = VkOffset3D{ 0, 0, 0 };
		region.imageExtent = VkExtent3D{ (uint32_t)width, (uint32_t)height, 1 };

		regions[num_regions] = region;
		num_regions++;

		buffer_size += width * height * bytes_per_pixel;

		if (!mipmap || (width == 1 && height == 1))
			break;

		width >>= 1;
		if (width < 1) width = 1;

		height >>= 1;
		if (height < 1) height = 1;
	}

	ensure_staging_buffer_allocation(buffer_size);
	Com_Memcpy(vk_world.staging_buffer_ptr, pixels, buffer_size);

	record_and_run_commands(vk.command_pool, vk.queue,
		[&image, &num_regions, &regions](VkCommandBuffer command_buffer) {

		record_buffer_memory_barrier(command_buffer, vk_world.staging_buffer,
			VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT);

		record_image_layout_transition(command_buffer, image, VK_IMAGE_ASPECT_COLOR_BIT,
			0, VK_IMAGE_LAYOUT_UNDEFINED, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		vkCmdCopyBufferToImage(command_buffer, vk_world.staging_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, num_regions, regions);

		record_image_layout_transition(command_buffer, image, VK_IMAGE_ASPECT_COLOR_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	});
}

static VkPipeline create_pipeline(const Vk_Pipeline_Def& def) {
	auto get_shader_stage_desc = [](VkShaderStageFlagBits stage, VkShaderModule shader_module, const char* entry) {
		VkPipelineShaderStageCreateInfo desc;
		desc.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		desc.pNext = nullptr;
		desc.flags = 0;
		desc.stage = stage;
		desc.module = shader_module;
		desc.pName = entry;
		desc.pSpecializationInfo = nullptr;
		return desc;
	};

	struct Specialization_Data {
		int32_t alpha_test_func;
	} specialization_data;

	if ((def.state_bits & GLS_ATEST_BITS) == 0)
		specialization_data.alpha_test_func = 0;
	else if (def.state_bits & GLS_ATEST_GT_0)
		specialization_data.alpha_test_func = 1;
	else if (def.state_bits & GLS_ATEST_LT_80)
		specialization_data.alpha_test_func = 2;
	else if (def.state_bits & GLS_ATEST_GE_80)
		specialization_data.alpha_test_func = 3;
	else if (def.state_bits & GLS_ATEST_GE_C0)
		specialization_data.alpha_test_func = 4;
	else
		ri.Error(ERR_DROP, "create_pipeline: invalid alpha test state bits\n");

	VkSpecializationMapEntry specialization_entries[1];
	specialization_entries[0].constantID = 0;
	specialization_entries[0].offset = offsetof(struct Specialization_Data, alpha_test_func);
	specialization_entries[0].size = sizeof(int32_t);

	VkSpecializationInfo specialization_info;
	specialization_info.mapEntryCount = 1;
	specialization_info.pMapEntries = specialization_entries;
	specialization_info.dataSize = sizeof(Specialization_Data);
	specialization_info.pData = &specialization_data;

	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_state;

	VkShaderModule* vs_module, *fs_module;
	if (def.shader_type == Vk_Shader_Type::single_texture) {
		vs_module = def.clipping_plane ? &vk.single_texture_clipping_plane_vs :  &vk.single_texture_vs;
		fs_module = &vk.single_texture_fs;
	} else if (def.shader_type == Vk_Shader_Type::multi_texture_mul) {
		vs_module = def.clipping_plane ? &vk.multi_texture_clipping_plane_vs : &vk.multi_texture_vs;
		fs_module = &vk.multi_texture_mul_fs;
	} else if (def.shader_type == Vk_Shader_Type::multi_texture_add) {
		vs_module = def.clipping_plane ? &vk.multi_texture_clipping_plane_vs : &vk.multi_texture_vs;
		fs_module = &vk.multi_texture_add_fs;
	}
	shader_stages_state.push_back(get_shader_stage_desc(VK_SHADER_STAGE_VERTEX_BIT, *vs_module, "main"));
	shader_stages_state.push_back(get_shader_stage_desc(VK_SHADER_STAGE_FRAGMENT_BIT, *fs_module, "main"));

	if (def.state_bits & GLS_ATEST_BITS)
		shader_stages_state.back().pSpecializationInfo = &specialization_info;

	//
	// Vertex input
	//
	VkVertexInputBindingDescription bindings[4];
	// xyz array
	bindings[0].binding = 0;
	bindings[0].stride = sizeof(vec4_t);
	bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// color array
	bindings[1].binding = 1;
	bindings[1].stride = sizeof(color4ub_t);
	bindings[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// st0 array
	bindings[2].binding = 2;
	bindings[2].stride = sizeof(vec2_t);
	bindings[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// st1 array
	bindings[3].binding = 3;
	bindings[3].stride = sizeof(vec2_t);
	bindings[3].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription attribs[4];
	// xyz
	attribs[0].location = 0;
	attribs[0].binding = 0;
	attribs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attribs[0].offset = 0;

	// color
	attribs[1].location = 1;
	attribs[1].binding = 1;
	attribs[1].format = VK_FORMAT_R8G8B8A8_UNORM;
	attribs[1].offset = 0;

	// st0
	attribs[2].location = 2;
	attribs[2].binding = 2;
	attribs[2].format = VK_FORMAT_R32G32_SFLOAT;
	attribs[2].offset = 0;

	// st1
	attribs[3].location = 3;
	attribs[3].binding = 3;
	attribs[3].format = VK_FORMAT_R32G32_SFLOAT;
	attribs[3].offset = 0;

	VkPipelineVertexInputStateCreateInfo vertex_input_state;
	vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_state.pNext = nullptr;
	vertex_input_state.flags = 0;
	vertex_input_state.vertexBindingDescriptionCount = (def.shader_type == Vk_Shader_Type::single_texture) ? 3 : 4;
	vertex_input_state.pVertexBindingDescriptions = bindings;
	vertex_input_state.vertexAttributeDescriptionCount = (def.shader_type == Vk_Shader_Type::single_texture) ? 3 : 4;
	vertex_input_state.pVertexAttributeDescriptions = attribs;

	//
	// Primitive assembly.
	//
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
	input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_state.pNext = nullptr;
	input_assembly_state.flags = 0;
	input_assembly_state.topology = def.line_primitives ? VK_PRIMITIVE_TOPOLOGY_LINE_LIST : VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_state.primitiveRestartEnable = VK_FALSE;

	//
	// Viewport.
	//
	VkPipelineViewportStateCreateInfo viewport_state;
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.pNext = nullptr;
	viewport_state.flags = 0;
	viewport_state.viewportCount = 1;
	viewport_state.pViewports = nullptr; // dynamic viewport state
	viewport_state.scissorCount = 1;
	viewport_state.pScissors = nullptr; // dynamic scissor state

	//
	// Rasterization.
	//
	VkPipelineRasterizationStateCreateInfo rasterization_state;
	rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state.pNext = nullptr;
	rasterization_state.flags = 0;
	rasterization_state.depthClampEnable = VK_FALSE;
	rasterization_state.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state.polygonMode = (def.state_bits & GLS_POLYMODE_LINE) ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;

	if (def.face_culling == CT_TWO_SIDED)
		rasterization_state.cullMode = VK_CULL_MODE_NONE;
	else if (def.face_culling == CT_FRONT_SIDED)
		rasterization_state.cullMode = (def.mirror ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_BACK_BIT);
	else if (def.face_culling == CT_BACK_SIDED)
		rasterization_state.cullMode = (def.mirror ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_FRONT_BIT);
	else
		ri.Error(ERR_DROP, "create_pipeline: invalid face culling mode\n");

	rasterization_state.frontFace = VK_FRONT_FACE_CLOCKWISE; // Q3 defaults to clockwise vertex order

	rasterization_state.depthBiasEnable = def.polygon_offset ? VK_TRUE : VK_FALSE;
	rasterization_state.depthBiasConstantFactor = 0.0f; // dynamic depth bias state
	rasterization_state.depthBiasClamp = 0.0f; // dynamic depth bias state
	rasterization_state.depthBiasSlopeFactor = 0.0f; // dynamic depth bias state
	rasterization_state.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisample_state;
	multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state.pNext = nullptr;
	multisample_state.flags = 0;
	multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample_state.sampleShadingEnable = VK_FALSE;
	multisample_state.minSampleShading = 1.0f;
	multisample_state.pSampleMask = nullptr;
	multisample_state.alphaToCoverageEnable = VK_FALSE;
	multisample_state.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depth_stencil_state;
	depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_state.pNext = nullptr;
	depth_stencil_state.flags = 0;
	depth_stencil_state.depthTestEnable = (def.state_bits & GLS_DEPTHTEST_DISABLE) ? VK_FALSE : VK_TRUE;
	depth_stencil_state.depthWriteEnable = (def.state_bits & GLS_DEPTHMASK_TRUE) ? VK_TRUE : VK_FALSE;
	depth_stencil_state.depthCompareOp = (def.state_bits & GLS_DEPTHFUNC_EQUAL) ? VK_COMPARE_OP_EQUAL : VK_COMPARE_OP_LESS_OR_EQUAL;
	depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_state.stencilTestEnable = (def.shadow_phase != Vk_Shadow_Phase::disabled) ? VK_TRUE : VK_FALSE;

	if (def.shadow_phase == Vk_Shadow_Phase::shadow_edges_rendering) {
		depth_stencil_state.front.failOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.front.passOp = (def.face_culling == CT_FRONT_SIDED) ? VK_STENCIL_OP_INCREMENT_AND_CLAMP : VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		depth_stencil_state.front.depthFailOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.front.compareOp = VK_COMPARE_OP_ALWAYS;
		depth_stencil_state.front.compareMask = 255;
		depth_stencil_state.front.writeMask = 255;
		depth_stencil_state.front.reference = 0;

		depth_stencil_state.back = depth_stencil_state.front;

	}  else if (def.shadow_phase == Vk_Shadow_Phase::fullscreen_quad_rendering) {
		depth_stencil_state.front.failOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.front.passOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.front.depthFailOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.front.compareOp = VK_COMPARE_OP_NOT_EQUAL;
		depth_stencil_state.front.compareMask = 255;
		depth_stencil_state.front.writeMask = 255;
		depth_stencil_state.front.reference = 0;

		depth_stencil_state.back = depth_stencil_state.front;

	} else {
		depth_stencil_state.front = {};
		depth_stencil_state.back = {};
	}

	depth_stencil_state.minDepthBounds = 0.0;
	depth_stencil_state.maxDepthBounds = 0.0;

	VkPipelineColorBlendAttachmentState attachment_blend_state = {};
	attachment_blend_state.blendEnable = (def.state_bits & (GLS_SRCBLEND_BITS | GLS_DSTBLEND_BITS)) ? VK_TRUE : VK_FALSE;

	if (def.shadow_phase == Vk_Shadow_Phase::shadow_edges_rendering)
		attachment_blend_state.colorWriteMask = 0;
	else
		attachment_blend_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	
	if (attachment_blend_state.blendEnable) {
		switch (def.state_bits & GLS_SRCBLEND_BITS) {
			case GLS_SRCBLEND_ZERO:
				attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
				break;
			case GLS_SRCBLEND_ONE:
				attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
				break;
			case GLS_SRCBLEND_DST_COLOR:
				attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
				break;
			case GLS_SRCBLEND_ONE_MINUS_DST_COLOR:
				attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
				break;
			case GLS_SRCBLEND_SRC_ALPHA:
				attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				break;
			case GLS_SRCBLEND_ONE_MINUS_SRC_ALPHA:
				attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				break;
			case GLS_SRCBLEND_DST_ALPHA:
				attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
				break;
			case GLS_SRCBLEND_ONE_MINUS_DST_ALPHA:
				attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
				break;
			case GLS_SRCBLEND_ALPHA_SATURATE:
				attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
				break;
			default:
				ri.Error( ERR_DROP, "create_pipeline: invalid src blend state bits\n" );
				break;
		}
		switch (def.state_bits & GLS_DSTBLEND_BITS) {
			case GLS_DSTBLEND_ZERO:
				attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
				break;
			case GLS_DSTBLEND_ONE:
				attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
				break;
			case GLS_DSTBLEND_SRC_COLOR:
				attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
				break;
			case GLS_DSTBLEND_ONE_MINUS_SRC_COLOR:
				attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
				break;
			case GLS_DSTBLEND_SRC_ALPHA:
				attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				break;
			case GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA:
				attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				break;
			case GLS_DSTBLEND_DST_ALPHA:
				attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
				break;
			case GLS_DSTBLEND_ONE_MINUS_DST_ALPHA:
				attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
				break;
			default:
				ri.Error( ERR_DROP, "create_pipeline: invalid dst blend state bits\n" );
				break;
		}

		attachment_blend_state.srcAlphaBlendFactor = attachment_blend_state.srcColorBlendFactor;
		attachment_blend_state.dstAlphaBlendFactor = attachment_blend_state.dstColorBlendFactor;
		attachment_blend_state.colorBlendOp = VK_BLEND_OP_ADD;
		attachment_blend_state.alphaBlendOp = VK_BLEND_OP_ADD;
	}

	VkPipelineColorBlendStateCreateInfo blend_state;
	blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blend_state.pNext = nullptr;
	blend_state.flags = 0;
	blend_state.logicOpEnable = VK_FALSE;
	blend_state.logicOp = VK_LOGIC_OP_COPY;
	blend_state.attachmentCount = 1;
	blend_state.pAttachments = &attachment_blend_state;
	blend_state.blendConstants[0] = 0.0f;
	blend_state.blendConstants[1] = 0.0f;
	blend_state.blendConstants[2] = 0.0f;
	blend_state.blendConstants[3] = 0.0f;

	VkPipelineDynamicStateCreateInfo dynamic_state;
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.pNext = nullptr;
	dynamic_state.flags = 0;
	dynamic_state.dynamicStateCount = 3;
	VkDynamicState dynamic_state_array[3] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_BIAS };
	dynamic_state.pDynamicStates = dynamic_state_array;

	VkGraphicsPipelineCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	create_info.pNext = nullptr;
	create_info.flags = 0;
	create_info.stageCount = static_cast<uint32_t>(shader_stages_state.size());
	create_info.pStages = shader_stages_state.data();
	create_info.pVertexInputState = &vertex_input_state;
	create_info.pInputAssemblyState = &input_assembly_state;
	create_info.pTessellationState = nullptr;
	create_info.pViewportState = &viewport_state;
	create_info.pRasterizationState = &rasterization_state;
	create_info.pMultisampleState = &multisample_state;
	create_info.pDepthStencilState = &depth_stencil_state;
	create_info.pColorBlendState = &blend_state;
	create_info.pDynamicState = &dynamic_state;
	create_info.layout = vk.pipeline_layout;
	create_info.renderPass = vk.render_pass;
	create_info.subpass = 0;
	create_info.basePipelineHandle = VK_NULL_HANDLE;
	create_info.basePipelineIndex = -1;

	VkPipeline pipeline;
	VK_CHECK(vkCreateGraphicsPipelines(vk.device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline));

	//vkCmdBindDescriptorSets(vk.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk.pipeline_layout, 0, 1, &vk.bindless_descriptor_set, 0, nullptr);
	
	return pipeline;
}

VkSampler vk_find_sampler(const Vk_Sampler_Def& def) {
	// Look for sampler among existing samplers.
	for (int i = 0; i < vk_world.num_samplers; i++) {
		const auto& cur_def = vk_world.sampler_defs[i];

		if (cur_def.repeat_texture == def.repeat_texture &&
			cur_def.gl_mag_filter == def.gl_mag_filter && 
			cur_def.gl_min_filter == def.gl_min_filter)
		{
			return vk_world.samplers[i];
		}
	}

	// Create new sampler.
	if (vk_world.num_samplers >= MAX_VK_SAMPLERS) {
		ri.Error(ERR_DROP, "vk_find_sampler: MAX_VK_SAMPLERS hit\n");
	}

	VkSamplerAddressMode address_mode = def.repeat_texture ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

	VkFilter mag_filter;
	if (def.gl_mag_filter == VK_FILTER_NEAREST) {
		mag_filter = VK_FILTER_NEAREST;
	} else if (def.gl_mag_filter == VK_FILTER_LINEAR) {
		mag_filter = VK_FILTER_LINEAR;
	} else {
		ri.Error(ERR_FATAL, "vk_find_sampler: invalid gl_mag_filter");
	}

	VkFilter min_filter;
	VkSamplerMipmapMode mipmap_mode;
	bool max_lod_0_25 = false; // used to emulate OpenGL's GL_LINEAR/GL_NEAREST minification filter
	if (def.gl_min_filter == VK_FILTER_NEAREST) {
		min_filter = VK_FILTER_NEAREST;
		mipmap_mode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		max_lod_0_25 = true;
	} else if (def.gl_min_filter == VK_FILTER_LINEAR) {
		min_filter = VK_FILTER_LINEAR;
		mipmap_mode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		max_lod_0_25 = true;
	} else if (def.gl_min_filter == VK_SAMPLER_MIPMAP_MODE_NEAREST) {
		min_filter = VK_FILTER_NEAREST;
		mipmap_mode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	} else if (def.gl_min_filter == VK_SAMPLER_MIPMAP_MODE_NEAREST) {
		min_filter = VK_FILTER_LINEAR;
		mipmap_mode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	} else if (def.gl_min_filter == VK_SAMPLER_MIPMAP_MODE_LINEAR) {
		min_filter = VK_FILTER_NEAREST;
		mipmap_mode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	} else if (def.gl_min_filter == VK_SAMPLER_MIPMAP_MODE_LINEAR) {
		min_filter = VK_FILTER_LINEAR;
		mipmap_mode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	} else {
		ri.Error(ERR_FATAL, "vk_find_sampler: invalid gl_min_filter");
	}

	VkSamplerCreateInfo desc;
	desc.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	desc.pNext = nullptr;
	desc.flags = 0;
	desc.magFilter = mag_filter;
	desc.minFilter = min_filter;
	desc.mipmapMode = mipmap_mode;
	desc.addressModeU = address_mode;
	desc.addressModeV = address_mode;
	desc.addressModeW = address_mode;
	desc.mipLodBias = 0.0f;
	
	if (r_ext_texture_filter_anisotropic->integer) {
        desc.anisotropyEnable = VK_TRUE;
        desc.maxAnisotropy = 16;
	} else {
        desc.anisotropyEnable = VK_FALSE;
        desc.maxAnisotropy = 1;
	}
	
	desc.compareEnable = VK_FALSE;
	desc.compareOp = VK_COMPARE_OP_ALWAYS;
	desc.minLod = 0.0f;
	desc.maxLod = max_lod_0_25 ? 0.25f : 12.0f;
	desc.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	desc.unnormalizedCoordinates = VK_FALSE;

	VkSampler sampler;
	VK_CHECK(vkCreateSampler(vk.device, &desc, nullptr, &sampler));

	vk_world.sampler_defs[vk_world.num_samplers] = def;
	vk_world.samplers[vk_world.num_samplers] = sampler;
	vk_world.num_samplers++;
	return sampler;
}

struct Timer {
	using Clock = std::chrono::high_resolution_clock;
	using Second = std::chrono::duration<double, std::ratio<1>>;

	Clock::time_point start = Clock::now();
	double elapsed_seconds() const {
		const auto duration = Clock::now() - start;
		double seconds = std::chrono::duration_cast<Second>(duration).count();
		return seconds;
	}
};

VkPipeline vk_find_pipeline(const Vk_Pipeline_Def& def) {
	for (int i = 0; i < vk_world.num_pipelines; i++) {
		const auto& cur_def = vk_world.pipeline_defs[i];

		if (cur_def.shader_type == def.shader_type &&
			cur_def.state_bits == def.state_bits &&
			cur_def.face_culling == def.face_culling &&
			cur_def.polygon_offset == def.polygon_offset &&
			cur_def.clipping_plane == def.clipping_plane &&
			cur_def.mirror == def.mirror &&
			cur_def.line_primitives == def.line_primitives &&
			cur_def.shadow_phase == def.shadow_phase)
		{
			return vk_world.pipelines[i];
		}
	}

	if (vk_world.num_pipelines >= MAX_VK_PIPELINES) {
		ri.Error(ERR_DROP, "vk_find_pipeline: MAX_VK_PIPELINES hit\n");
	}

	Timer t;
	VkPipeline pipeline = create_pipeline(def);
	vk_world.pipeline_create_time += t.elapsed_seconds();

	vk_world.pipeline_defs[vk_world.num_pipelines] = def;
	vk_world.pipelines[vk_world.num_pipelines] = pipeline;
	vk_world.num_pipelines++;
	return pipeline;
}

static VkRect2D get_viewport_rect() {
	VkRect2D r;
	if (backEnd.projection2D) {
		r.offset.x = 0.0f;
		r.offset.y = 0.0f;
		r.extent.width = glConfig.vidWidth;
		r.extent.height = glConfig.vidHeight;
	} else {
		r.offset.x = backEnd.viewParms.viewportX;
		r.offset.y = glConfig.vidHeight - (backEnd.viewParms.viewportY + backEnd.viewParms.viewportHeight);
		r.extent.width = backEnd.viewParms.viewportWidth;
		r.extent.height = backEnd.viewParms.viewportHeight;
	}
	return r;
}

static VkViewport get_viewport(Vk_Depth_Range depth_range) {
	VkRect2D r = get_viewport_rect();

	VkViewport viewport;
	viewport.x = (float)r.offset.x;
	viewport.y = (float)r.offset.y;
	viewport.width = (float)r.extent.width;
	viewport.height = (float)r.extent.height;

	if (depth_range == Vk_Depth_Range::force_zero) {
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 0.0f;
	} else if (depth_range == Vk_Depth_Range::force_one) {
		viewport.minDepth = 1.0f;
		viewport.maxDepth = 1.0f;
	} else if (depth_range == Vk_Depth_Range::weapon) {
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 0.3f;
	} else {
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
	}
	return viewport;
}

static VkRect2D get_scissor_rect() {
	VkRect2D r = get_viewport_rect();

	if (r.offset.x < 0)
		r.offset.x = 0;
	if (r.offset.y < 0)
		r.offset.y = 0;

	if (r.offset.x + r.extent.width > glConfig.vidWidth)
		r.extent.width = glConfig.vidWidth - r.offset.x;
	if (r.offset.y + r.extent.height > glConfig.vidHeight)
		r.extent.height = glConfig.vidHeight - r.offset.y;

	return r;
}

static void get_mvp_transform(float* mvp) {
	if (backEnd.projection2D) {
        //2D stuff sent by UI is in a 640x480 virtual screen.
		float mvp0 = 2.0f / 640.0f; //2.0f / glConfig.vidWidth; 
		float mvp5 = 2.0f / 480.0f; //2.0f / glConfig.vidHeight;

		mvp[0]  =  mvp0; mvp[1]  =  0.0f; mvp[2]  = 0.0f; mvp[3]  = 0.0f;
		mvp[4]  =  0.0f; mvp[5]  =  mvp5; mvp[6]  = 0.0f; mvp[7]  = 0.0f;
		mvp[8]  =  0.0f; mvp[9]  =  0.0f; mvp[10] = 1.0f; mvp[11] = 0.0f;
		mvp[12] = -1.0f; mvp[13] = -1.0f; mvp[14] = 0.0f; mvp[15] = 1.0f;

	} else {
		const float* p = backEnd.viewParms.projectionMatrix;

		// update q3's proj matrix (opengl) to vulkan conventions: z - [0, 1] instead of [-1, 1] and invert y direction
		float zNear	= r_znear->value;
		float zFar = backEnd.viewParms.zFar;
		float P10 = -zFar / (zFar - zNear);
		float P14 = -zFar*zNear / (zFar - zNear);
		float P5 = -p[5];

		float proj[16] = {
			p[0],  p[1],  p[2], p[3],
			p[4],  P5,    p[6], p[7],
			p[8],  p[9],  P10,  p[11],
			p[12], p[13], P14,  p[15]
		};

		myGlMultMatrix(vk_world.modelview_transform, proj, mvp);
	}
}

void vk_clear_attachments(bool clear_depth_stencil, bool clear_color, vec4_t color) {
	if (!vk.active)
		return;

	if (!clear_depth_stencil && !clear_color)
		return;

	VkClearAttachment attachments[2];
	uint32_t attachment_count = 0;

	if (clear_depth_stencil) {
		attachments[0].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		attachments[0].clearValue.depthStencil.depth = 1.0f;

		if (r_shadows->integer == 2) {
			attachments[0].aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			attachments[0].clearValue.depthStencil.stencil = 0;
		}
		attachment_count = 1;
	}

	if (clear_color) {
		attachments[attachment_count].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		attachments[attachment_count].colorAttachment = 0;
		attachments[attachment_count].clearValue.color = { color[0], color[1], color[2], color[3] };
		attachment_count++;
	}

	VkClearRect clear_rect[2];
	clear_rect[0].rect = get_scissor_rect();
	clear_rect[0].baseArrayLayer = 0;
	clear_rect[0].layerCount = 1;
	int rect_count = 1;

	// Split viewport rectangle into two non-overlapping rectangles.
	// It's a HACK to prevent Vulkan validation layer's performance warning:
	//		"vkCmdClearAttachments() issued on command buffer object XXX prior to any Draw Cmds.
	//		 It is recommended you use RenderPass LOAD_OP_CLEAR on Attachments prior to any Draw."
	// 
	// NOTE: we don't use LOAD_OP_CLEAR for color attachment when we begin renderpass
	// since at that point we don't know whether we need collor buffer clear (usually we don't).
	if (clear_color) {
		uint32_t h = clear_rect[0].rect.extent.height / 2;
		clear_rect[0].rect.extent.height = h;
		clear_rect[1] = clear_rect[0];
		clear_rect[1].rect.offset.y = h;
		rect_count = 2;
	}

	vkCmdClearAttachments(vk.command_buffer, attachment_count, attachments, rect_count, clear_rect);
}

void vk_bind_geometry() {
	// xyz stream
	{
		if ((vk.xyz_elements + tess.numVertexes) * sizeof(vec4_t) > XYZ_SIZE)
			ri.Error(ERR_DROP, "vk_bind_geometry: vertex buffer overflow (xyz)\n");

		byte* dst = vk.vertex_buffer_ptr + XYZ_OFFSET + vk.xyz_elements * sizeof(vec4_t);
		Com_Memcpy(dst, tess.xyz, tess.numVertexes * sizeof(vec4_t));

		VkDeviceSize xyz_offset = XYZ_OFFSET + vk.xyz_elements * sizeof(vec4_t);
		vkCmdBindVertexBuffers(vk.command_buffer, 0, 1, &vk.vertex_buffer, &xyz_offset);
		vk.xyz_elements += tess.numVertexes;
	}

	// indexes stream
	{
		std::size_t indexes_size = tess.numIndexes * sizeof(uint32_t);        

		if (vk.index_buffer_offset + indexes_size > INDEX_BUFFER_SIZE)
			ri.Error(ERR_DROP, "vk_bind_geometry: index buffer overflow\n");

		byte* dst = vk.index_buffer_ptr + vk.index_buffer_offset;
		Com_Memcpy(dst, tess.indexes, indexes_size);

		vkCmdBindIndexBuffer(vk.command_buffer, vk.index_buffer, vk.index_buffer_offset, VK_INDEX_TYPE_UINT32);
		vk.index_buffer_offset += indexes_size;
	}

	//
	// Specify push constants.
	//
	float push_constants[VK_PUSH_CONSTANT_SIZE]; // mvp transform + eye transform + clipping plane in eye space

	get_mvp_transform(push_constants);

	push_constants[16] = *((float*)(&(vk_world.current_image_index[0])));
	push_constants[17] = *((float*)(&(vk_world.current_image_index[1])));

	int push_constants_size = 18 * sizeof(float);

	if (backEnd.viewParms.isPortal) {
		// Eye space transform.
		// NOTE: backEnd.or.modelMatrix incorporates s_flipMatrix, so it should be taken into account 
		// when computing clipping plane too.
		float* eye_xform = push_constants + 18;
		for (int i = 0; i < 12; i++) {
			eye_xform[i] = backEnd.ori.modelMatrix[(i%4)*4 + i/4 ];
		}

		// Clipping plane in eye coordinates.
		float world_plane[4];
		world_plane[0] = backEnd.viewParms.portalPlane.normal[0];
		world_plane[1] = backEnd.viewParms.portalPlane.normal[1];
		world_plane[2] = backEnd.viewParms.portalPlane.normal[2];
		world_plane[3] = backEnd.viewParms.portalPlane.dist;

		float eye_plane[4];
		eye_plane[0] = DotProduct (backEnd.viewParms.ori.axis[0], world_plane);
		eye_plane[1] = DotProduct (backEnd.viewParms.ori.axis[1], world_plane);
		eye_plane[2] = DotProduct (backEnd.viewParms.ori.axis[2], world_plane);
		eye_plane[3] = DotProduct (world_plane, backEnd.viewParms.ori.origin) - world_plane[3];

		// Apply s_flipMatrix to be in the same coordinate system as eye_xfrom.
		push_constants[30] = -eye_plane[1];
		push_constants[31] =  eye_plane[2];
		push_constants[32] = -eye_plane[0];
		push_constants[33] =  eye_plane[3];

		push_constants_size += 64;
	}
	vkCmdPushConstants(vk.command_buffer, vk.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, push_constants_size, push_constants);
}

void vk_shade_geometry(VkPipeline pipeline, bool multitexture, Vk_Depth_Range depth_range, bool indexed) {
	// color
	{
		if ((vk.color_st_elements + tess.numVertexes) * sizeof(color4ub_t) > COLOR_SIZE)
			ri.Error(ERR_DROP, "vulkan: vertex buffer overflow (color)\n");

		byte* dst = vk.vertex_buffer_ptr + COLOR_OFFSET + vk.color_st_elements * sizeof(color4ub_t);
		Com_Memcpy(dst, tess.svars.colors, tess.numVertexes * sizeof(color4ub_t));
	}
	// st0
	{
		if ((vk.color_st_elements + tess.numVertexes) * sizeof(vec2_t) > ST0_SIZE)
			ri.Error(ERR_DROP, "vulkan: vertex buffer overflow (st0)\n");

		byte* dst = vk.vertex_buffer_ptr + ST0_OFFSET + vk.color_st_elements * sizeof(vec2_t);
		Com_Memcpy(dst, tess.svars.texcoords[0], tess.numVertexes * sizeof(vec2_t));
	}
	// st1
	if (multitexture) {
		if ((vk.color_st_elements + tess.numVertexes) * sizeof(vec2_t) > ST1_SIZE)
			ri.Error(ERR_DROP, "vulkan: vertex buffer overflow (st1)\n");

		byte* dst = vk.vertex_buffer_ptr + ST1_OFFSET + vk.color_st_elements * sizeof(vec2_t);
		Com_Memcpy(dst, tess.svars.texcoords[1], tess.numVertexes * sizeof(vec2_t));
	}

	// configure vertex data stream
	VkBuffer bufs[3] = { vk.vertex_buffer, vk.vertex_buffer, vk.vertex_buffer };
	VkDeviceSize offs[3] = {
		COLOR_OFFSET + vk.color_st_elements * sizeof(color4ub_t),
		ST0_OFFSET   + vk.color_st_elements * sizeof(vec2_t),
		ST1_OFFSET   + vk.color_st_elements * sizeof(vec2_t)
	};
	vkCmdBindVertexBuffers(vk.command_buffer, 1, multitexture ? 3 : 2, bufs, offs);
	vk.color_st_elements += tess.numVertexes;

	// bind descriptor sets
	vkCmdBindDescriptorSets(vk.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk.pipeline_layout, 0, 1, &vk.bindless_descriptor_set, 0, nullptr);

	// bind pipeline
	vkCmdBindPipeline(vk.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	// configure pipeline's dynamic state
	VkRect2D scissor_rect = get_scissor_rect();
	vkCmdSetScissor(vk.command_buffer, 0, 1, &scissor_rect);

	VkViewport viewport = get_viewport(depth_range);
	vkCmdSetViewport(vk.command_buffer, 0, 1, &viewport);

	if (tess.shader->polygonOffset) {
		vkCmdSetDepthBias(vk.command_buffer, r_offsetUnits->value, 0.0f, r_offsetFactor->value);
	}

	// issue draw call
	if (indexed)
		vkCmdDrawIndexed(vk.command_buffer, tess.numIndexes, 1, 0, 0, 0);
	else
		vkCmdDraw(vk.command_buffer, tess.numVertexes, 1, 0, 0);

	vk_world.dirty_depth_attachment = true;
}

void vk_begin_frame() {

	VK_CHECK(vkAcquireNextImageKHR(vk.device, vk.swapchain, UINT64_MAX, vk.image_acquired, VK_NULL_HANDLE, &vk.swapchain_image_index));

	VK_CHECK(vkWaitForFences(vk.device, 1, &vk.rendering_finished_fence, VK_FALSE, 1e9));
	VK_CHECK(vkResetFences(vk.device, 1, &vk.rendering_finished_fence));

	VkCommandBufferBeginInfo begin_info;
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.pNext = nullptr;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	begin_info.pInheritanceInfo = nullptr;

	VK_CHECK(vkBeginCommandBuffer(vk.command_buffer, &begin_info));

	// Ensure visibility of geometry buffers writes.
	record_buffer_memory_barrier(vk.command_buffer, vk.vertex_buffer,
		VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
		VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT);

	record_buffer_memory_barrier(vk.command_buffer, vk.index_buffer,
		VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
		VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_INDEX_READ_BIT);

	// Begin render pass.
	VkClearValue clear_values[2];
	/// ignore clear_values[0] which corresponds to color attachment
	clear_values[1].depthStencil.depth = 1.0;
	clear_values[1].depthStencil.stencil = 0;

	VkRenderPassBeginInfo render_pass_begin_info;
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.pNext = nullptr;
	render_pass_begin_info.renderPass = vk.render_pass;
	render_pass_begin_info.framebuffer = vk.framebuffers[vk.swapchain_image_index];
	render_pass_begin_info.renderArea.offset = { 0, 0 };
	render_pass_begin_info.renderArea.extent = { (uint32_t)glConfig.vidWidth, (uint32_t)glConfig.vidHeight };
	render_pass_begin_info.clearValueCount = 2;
	render_pass_begin_info.pClearValues = clear_values;

	vkCmdBeginRenderPass(vk.command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	vk_world.dirty_depth_attachment = false;
	vk.xyz_elements = 0;
	vk.color_st_elements = 0;
	vk.index_buffer_offset = 0;
}

void vk_end_frame() {

	vkCmdEndRenderPass(vk.command_buffer);
	VK_CHECK(vkEndCommandBuffer(vk.command_buffer));

	VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submit_info;
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext = nullptr;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &vk.image_acquired;
	submit_info.pWaitDstStageMask = &wait_dst_stage_mask;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &vk.command_buffer;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &vk.rendering_finished;

	VK_CHECK(vkQueueSubmit(vk.queue, 1, &submit_info, vk.rendering_finished_fence));

	VkPresentInfoKHR present_info;
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.pNext = nullptr;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &vk.rendering_finished;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &vk.swapchain;
	present_info.pImageIndices = &vk.swapchain_image_index;
	present_info.pResults = nullptr;
	VK_CHECK(vkQueuePresentKHR(vk.queue, &present_info));

	if (!images_to_update_bindless.empty()) {

		VkWriteDescriptorSet bindless_descriptor_writes[ images_to_update_bindless.size() ];
		VkDescriptorImageInfo bindless_image_info[ images_to_update_bindless.size() ];

		int current_write_index = 0;
		for (auto & item : images_to_update_bindless) {
			Vk_Image& image = vk_world.images[item];
			VkWriteDescriptorSet& descriptor_write = bindless_descriptor_writes[ current_write_index ];
			descriptor_write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
			descriptor_write.descriptorCount = 1;
			descriptor_write.dstArrayElement = item;
			descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptor_write.dstSet = vk.bindless_descriptor_set;
			descriptor_write.dstBinding = 0;

			Vk_Sampler_Def sampler_def;
			sampler_def.repeat_texture = image.repeat_texture;
			if (image.mipmap) {
				sampler_def.gl_mag_filter = gl_filter_max;
				sampler_def.gl_min_filter = gl_filter_min;
			} else {
				sampler_def.gl_mag_filter = VK_FILTER_LINEAR;
				sampler_def.gl_min_filter = VK_FILTER_LINEAR;
			}

			VkDescriptorImageInfo& image_info = bindless_image_info[ current_write_index ];
			image_info.sampler = vk_find_sampler(sampler_def);
			image_info.imageView = image.view;
			image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			descriptor_write.pImageInfo = &image_info;
			current_write_index ++;
		}

		vkUpdateDescriptorSets( vk.device, current_write_index, bindless_descriptor_writes, 0, nullptr );

		images_to_update_bindless.clear();
	}
}

void vk_read_pixels(byte* buffer) {
	vkDeviceWaitIdle(vk.device);

	// Create image in host visible memory to serve as a destination for framebuffer pixels.
	VkImageCreateInfo desc;
	desc.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	desc.pNext = nullptr;
	desc.flags = 0;
	desc.imageType = VK_IMAGE_TYPE_2D;
	desc.format = VK_FORMAT_R8G8B8A8_UNORM;
	desc.extent.width = glConfig.vidWidth;
	desc.extent.height = glConfig.vidHeight;
	desc.extent.depth = 1;
	desc.mipLevels = 1;
	desc.arrayLayers = 1;
	desc.samples = VK_SAMPLE_COUNT_1_BIT;
	desc.tiling = VK_IMAGE_TILING_LINEAR;
	desc.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	desc.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	desc.queueFamilyIndexCount = 0;
	desc.pQueueFamilyIndices = nullptr;
	desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImage image;
	VK_CHECK(vkCreateImage(vk.device, &desc, nullptr, &image));

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(vk.device, image, &memory_requirements);
	VkMemoryAllocateInfo alloc_info;
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.pNext = nullptr;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = find_memory_type(vk.physical_device, memory_requirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VkDeviceMemory memory;
	VK_CHECK(vkAllocateMemory(vk.device, &alloc_info, nullptr, &memory));
	VK_CHECK(vkBindImageMemory(vk.device, image, memory, 0));

	record_and_run_commands(vk.command_pool, vk.queue, [&image](VkCommandBuffer command_buffer) {
		record_image_layout_transition(command_buffer, vk.swapchain_images[vk.swapchain_image_index], VK_IMAGE_ASPECT_COLOR_BIT,
			VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 
			VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

		record_image_layout_transition(command_buffer, image, VK_IMAGE_ASPECT_COLOR_BIT,
			0, VK_IMAGE_LAYOUT_UNDEFINED,
			VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL);
	});

	// Check if we can use vkCmdBlitImage for the given source and destination image formats.
	bool blit_enabled = true;
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(vk.physical_device, vk.surface_format.format, &formatProps);
		if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) == 0)
			blit_enabled = false;

		vkGetPhysicalDeviceFormatProperties(vk.physical_device, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
		if ((formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) == 0)
			blit_enabled = false;
	}

	if (blit_enabled) {
		record_and_run_commands(vk.command_pool, vk.queue, [&image](VkCommandBuffer command_buffer) {
			VkImageBlit region;
			region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.srcSubresource.mipLevel = 0;
			region.srcSubresource.baseArrayLayer = 0;
			region.srcSubresource.layerCount = 1;
			region.srcOffsets[0] = {0, 0, 0};
			region.srcOffsets[1] = {glConfig.vidWidth, glConfig.vidHeight, 1};
			region.dstSubresource = region.srcSubresource;
			region.dstOffsets[0] = region.srcOffsets[0];
			region.dstOffsets[1] = region.srcOffsets[1];

			vkCmdBlitImage(command_buffer, vk.swapchain_images[vk.swapchain_image_index], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_GENERAL, 1, &region, VK_FILTER_NEAREST);
		});
	} else {
		record_and_run_commands(vk.command_pool, vk.queue, [&image](VkCommandBuffer command_buffer) {
			VkImageCopy region;
			region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.srcSubresource.mipLevel = 0;
			region.srcSubresource.baseArrayLayer = 0;
			region.srcSubresource.layerCount = 1;
			region.srcOffset = {0, 0, 0};
			region.dstSubresource = region.srcSubresource;
			region.dstOffset = region.srcOffset;
			region.extent = {(uint32_t)glConfig.vidWidth, (uint32_t)glConfig.vidHeight, 1};

			vkCmdCopyImage(command_buffer, vk.swapchain_images[vk.swapchain_image_index], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_GENERAL, 1, &region);
		});
	}

	// Copy data from destination image to memory buffer.
	VkImageSubresource subresource;
	subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource.mipLevel = 0;
	subresource.arrayLayer = 0;
	VkSubresourceLayout layout;
	vkGetImageSubresourceLayout(vk.device, image, &subresource, &layout);

	byte* data;
	VK_CHECK(vkMapMemory(vk.device, memory, 0, VK_WHOLE_SIZE, 0, (void**)&data));
	data += layout.offset;

	byte* buffer_ptr = buffer + glConfig.vidWidth * (glConfig.vidHeight - 1) * 4;
	for (int i = 0; i < glConfig.vidHeight; i++) {
		Com_Memcpy(buffer_ptr, data, glConfig.vidWidth * 4);
		buffer_ptr -= glConfig.vidWidth * 4;
		data += layout.rowPitch;
	}

	if (!blit_enabled) {
		auto fmt = vk.surface_format.format;
		bool swizzle_components = (fmt == VK_FORMAT_B8G8R8A8_SRGB || fmt == VK_FORMAT_B8G8R8A8_UNORM || fmt == VK_FORMAT_B8G8R8A8_SNORM);
		if (swizzle_components) {
			buffer_ptr = buffer;
			for (int i = 0; i < glConfig.vidWidth * glConfig.vidHeight; i++) {
				byte tmp = buffer_ptr[0];
				buffer_ptr[0] = buffer_ptr[2];
				buffer_ptr[2] = tmp;
				buffer_ptr += 4;
			}
		}
	}

	vkDestroyImage(vk.device, image, nullptr);
	vkFreeMemory(vk.device, memory, nullptr);
}

