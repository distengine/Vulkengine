#include "Vulkengine.h"
#include <glm\gtc\matrix_transform.hpp>

vk::Result Vulkengine::CreateInstance()
{
	// Get the supported layers and extensions
	std::string test;
	auto extensions = vk::enumerateInstanceExtensionProperties();
	auto layers = vk::enumerateInstanceLayerProperties();
	std::vector<const char*> extensionNames;
	std::vector<const char*> layerNames;
	std::vector<std::string> neededLayers = { "VK_LAYER_LUNARG_standard_validation", "VK_LAYER_VALVE_steam_overlay" };
	for (const auto& ext : extensions)
	{
		extensionNames.push_back(ext.extensionName);
	}
	for (const auto& lay : layers)
	{
		for (const auto& needLay : neededLayers)
		{
			if (lay.layerName == needLay)
			{
				layerNames.push_back(lay.layerName);
			}
		}
	}
	// Create out variables to create the instance and store the results
	vk::Result Result;
	vk::InstanceCreateInfo iCreateInfo;
	vk::ApplicationInfo appInfo;

	// Fill our structures to support our needed layers and extensions
	appInfo.pApplicationName = "Game";
	appInfo.pEngineName = "Vulkengine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	iCreateInfo.pApplicationInfo = &appInfo;
	iCreateInfo.ppEnabledExtensionNames = extensionNames.data();
	iCreateInfo.enabledExtensionCount = extensionNames.size();
	iCreateInfo.ppEnabledLayerNames = layerNames.data();
	iCreateInfo.enabledLayerCount = layerNames.size();

	//Create our instance
	Result = vk::createInstance(&iCreateInfo, nullptr, &m_Instance);
	return Result;
}

vk::Result Vulkengine::GetPhysicalDeviceList()
{
	// Enumerate through out physical devices
	// TODO : Multi-GPU Solution
	auto physicalDevices = m_Instance.enumeratePhysicalDevices();
	if (physicalDevices.size() > 0)
	{
		m_PhysicalDevice = physicalDevices[0];
		return vk::Result::eSuccess;
	}
	return vk::Result::eIncomplete;
}

vk::Result Vulkengine::GetQueueFamilies()
{
	// Enumerate through our queue families and store them
	m_QueueFamilies = m_PhysicalDevice.getQueueFamilyProperties();
	if(m_QueueFamilies.size() > 0)
	{
		return vk::Result::eSuccess;
	}
	return vk::Result::eIncomplete;
}

vk::Result Vulkengine::CreateDevice()
{
	// Get our needed layers and extensions
	auto extensions = m_PhysicalDevice.enumerateDeviceExtensionProperties();
	std::vector<const char*> extensionNames;
	std::vector<std::string> neededExtensions = { "VK_KHR_swapchain" }; // Swapchain lets us render to the screen
	for (const auto& ext : extensions)
	{
		for (const auto& needExt : neededExtensions)
		{
			if (ext.extensionName == needExt)
			{
				extensionNames.push_back(ext.extensionName);
			}
		}
	}

	// Determine which queues have graphic and transfer bits
	for (uint32_t i = 0; i < m_QueueFamilies.size(); i++)
	{
		if (m_QueueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			m_QueueIndicies.push_back(i);
			m_QueueGraphicIndex = i;
		}
	}

	// Fill our device queue create info
	float queuePriority = 1.0f;
	vk::DeviceQueueCreateInfo dqCreateInfo;
	dqCreateInfo.queueFamilyIndex = *m_QueueIndicies.data();
	dqCreateInfo.queueCount = m_QueueIndicies.size();
	dqCreateInfo.pQueuePriorities = &queuePriority;
	// File our device create info
	vk::DeviceCreateInfo dCreateInfo;
	dCreateInfo.queueCreateInfoCount = m_QueueIndicies.size();
	dCreateInfo.pQueueCreateInfos = &dqCreateInfo;
	dCreateInfo.ppEnabledExtensionNames = extensionNames.data();
	dCreateInfo.enabledExtensionCount = extensionNames.size();
	// Create Device
	auto Result = m_PhysicalDevice.createDevice(&dCreateInfo, nullptr, &m_Device);
	return Result;
}

vk::Result Vulkengine::CreateCommandPool()
{
	vk::CommandPoolCreateInfo cmdPoolInfo;
	cmdPoolInfo.queueFamilyIndex = *m_QueueIndicies.data();
	auto Result = m_Device.createCommandPool(&cmdPoolInfo, nullptr, &m_CommandPool);
	if (Result == vk::Result::eSuccess)
	{
		vk::CommandBufferAllocateInfo cmd;
		cmd.commandPool = m_CommandPool;
		cmd.level = vk::CommandBufferLevel::ePrimary;
		cmd.commandBufferCount = m_QueueIndicies.size();

		Result = m_Device.allocateCommandBuffers(&cmd, &m_CommandBuffer);
		return Result;
	}
	return Result;
}

vk::Result Vulkengine::CreateSurface(HINSTANCE hinstance, HWND hwnd)
{
	// Creates our window surface to load vulkan into
	vk::Win32SurfaceCreateInfoKHR createInfo;
	createInfo.hinstance = hinstance;
	createInfo.hwnd = hwnd;
	auto Result = m_Instance.createWin32SurfaceKHR(&createInfo, nullptr, &m_Surface);
	return Result;
}

vk::Result Vulkengine::CreateSwapChain()
{
	std::vector<vk::Bool32> presentSupport;
	for (int i = 0; i < m_QueueFamilies.size(); i++)
		presentSupport.push_back(m_PhysicalDevice.getSurfaceSupportKHR(i, m_Surface));

	for (uint32_t i = 0; i < presentSupport.size(); i++)
	{
		if (i == m_QueueGraphicIndex && presentSupport[i] == VK_TRUE)
		{
			m_QueuePresentIndex = i;
			break;
		}
		else if (presentSupport[i] == VK_TRUE && presentSupport[i] > m_QueueGraphicIndex)
		{
			m_QueuePresentIndex = i;
			break;
		}
		else
		{
			m_QueuePresentIndex = 999;
			break;
		}
	}
	auto surfForm = m_PhysicalDevice.getSurfaceFormatsKHR(m_Surface);
	auto surfCapi = m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);
	m_Extent2D = surfCapi.currentExtent;
	vk::SwapchainCreateInfoKHR createInfo;
	createInfo.surface = m_Surface;
	createInfo.minImageCount = 2;
	createInfo.imageFormat = surfForm[0].format;
	createInfo.imageExtent = surfCapi.currentExtent;
	createInfo.preTransform = surfCapi.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.imageArrayLayers = surfCapi.maxImageArrayLayers;
	createInfo.presentMode = vk::PresentModeKHR::eFifo;
	createInfo.oldSwapchain = vk::SwapchainKHR();
	createInfo.clipped = true;
	createInfo.imageColorSpace = surfForm[0].colorSpace;
	createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	uint32_t queueFamilyIndices[] = {m_QueueGraphicIndex, m_QueuePresentIndex};
	if (queueFamilyIndices[0] == queueFamilyIndices[1])
	{
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	else
	{
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	auto Result = m_Device.createSwapchainKHR(&createInfo, nullptr, &m_SwapChain);
	m_Images = m_Device.getSwapchainImagesKHR(m_SwapChain);
	for (uint32_t i = 0; i < m_Images.size(); i++)
	{
		vk::ImageSubresourceRange isr;
		isr.aspectMask = vk::ImageAspectFlagBits::eColor;
		isr.baseMipLevel = 0;
		isr.levelCount = 1;
		isr.baseArrayLayer = 0;
		isr.layerCount = 1;

		vk::ImageViewCreateInfo ivCreateInfo;
		ivCreateInfo.image = m_Images[i];
		ivCreateInfo.viewType = vk::ImageViewType::e2D;
		ivCreateInfo.format = surfForm[0].format;
		ivCreateInfo.components.r = vk::ComponentSwizzle::eR;
		ivCreateInfo.components.g = vk::ComponentSwizzle::eG;
		ivCreateInfo.components.b = vk::ComponentSwizzle::eB;
		ivCreateInfo.components.a = vk::ComponentSwizzle::eA;
		ivCreateInfo.subresourceRange = isr;
	}
	return Result;
}

vk::Result Vulkengine::CreateDepthBuffer()
{
	// Create the depth image 
	vk::ImageCreateInfo imageInfo;
	const vk::Format depthFormat = vk::Format::eD16Unorm;
	vk::FormatProperties props;
	m_PhysicalDevice.getFormatProperties(depthFormat, &props);
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.format = depthFormat;
	imageInfo.extent.width = m_Extent2D.width;
	imageInfo.extent.height = m_Extent2D.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = vk::SampleCountFlagBits::e4;
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;

	m_DepthImage = m_Device.createImage(imageInfo);
	
	//Allocate the memory for the depth buffer
	auto memoryReqs = m_Device.getImageMemoryRequirements(m_DepthImage);
	vk::MemoryAllocateInfo memInfo;
	memInfo.allocationSize = memoryReqs.size;
	memInfo.memoryTypeIndex = GetMemoryTypeIndex(memoryReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	m_DepthMemory = m_Device.allocateMemory(memInfo);
	m_Device.bindImageMemory(m_DepthImage, m_DepthMemory, 0);

	// Create the image view

	vk::ImageSubresourceRange isr;
	isr.aspectMask = vk::ImageAspectFlagBits::eDepth;
	isr.baseMipLevel = 0;
	isr.levelCount = 1;
	isr.baseArrayLayer = 0;
	isr.layerCount = 1;

	vk::ImageViewCreateInfo iCreateInfo;
	iCreateInfo.image = m_DepthImage;
	iCreateInfo.format = vk::Format::eD16Unorm;
	iCreateInfo.components.r = vk::ComponentSwizzle::eR;
	iCreateInfo.components.g = vk::ComponentSwizzle::eG;
	iCreateInfo.components.b = vk::ComponentSwizzle::eB;
	iCreateInfo.components.a = vk::ComponentSwizzle::eA;
	iCreateInfo.subresourceRange = isr;
	iCreateInfo.viewType = vk::ImageViewType::e2D;
	
	auto Result = m_Device.createImageView(&iCreateInfo, nullptr, &m_DepthView);
	return Result;
}

void Vulkengine::CreateUniformBuffer()
{
	m_Model = glm::mat4(1.0f);
	m_View = glm::lookAt(glm::vec3(0, 3, 10), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
	m_Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	m_Clip = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f, 1.0f);
	m_MVP = m_Clip * m_Projection * m_View * m_Model;

	vk::BufferCreateInfo createInfo;
	createInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
	createInfo.size = sizeof(m_MVP);
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.sharingMode = vk::SharingMode::eExclusive;
	m_UniformBuffer = m_Device.createBuffer(createInfo, nullptr);
	auto memoryReq = m_Device.getBufferMemoryRequirements(m_UniformBuffer);
	vk::MemoryAllocateInfo memInfo;
	memInfo.memoryTypeIndex = 0;
	memInfo.allocationSize = memoryReq.size;
	memInfo.memoryTypeIndex = GetMemoryTypeIndex(memoryReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	m_UniformMemory = m_Device.allocateMemory(memInfo, nullptr);
	auto *pData = m_Device.mapMemory(m_UniformMemory, 0, memoryReq.size);
	memcpy(pData, &m_MVP, sizeof(m_MVP));
	m_Device.unmapMemory(m_UniformMemory);
	m_Device.bindBufferMemory(m_UniformBuffer, m_UniformMemory, 0);
}

// This function is used to request a device memory type that supports all the property flags we request (e.g. device local, host visibile)
// Upon success it will return the index of the memory type that fits our requestes memory properties
// This is necessary as implementations can offer an arbitrary number of memory types with different
// memory properties. 
// You can check http://vulkan.gpuinfo.org/ for details on different memory configurations
uint32_t Vulkengine::GetMemoryTypeIndex(uint32_t typeBits, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties deviceMemoryProperties = m_PhysicalDevice.getMemoryProperties();
	// Iterate over all memory types available for the device used in this example
	for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		typeBits >>= 1;
	}

	throw "Could not find a suitable memory type!";
}