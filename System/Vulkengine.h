#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.hpp>
#include <glm\mat4x4.hpp>

class Vulkengine
{
public:
	vk::Result CreateInstance();
	vk::Result GetPhysicalDeviceList();
	vk::Result GetQueueFamilies();
	vk::Result CreateDevice();
	vk::Result CreateCommandPool();
	vk::Result CreateSurface(HINSTANCE hinstance, HWND hwnd);
	vk::Result CreateSwapChain();
	vk::Result CreateDepthBuffer();
	void	   CreateUniformBuffer();
	uint32_t   GetMemoryTypeIndex(uint32_t typeBits, vk::MemoryPropertyFlags properties);
private:

private:
	vk::Instance							m_Instance;			// Vulkan Instance
	vk::PhysicalDevice						m_PhysicalDevice;	// Physical Device = GPU
	std::vector<vk::QueueFamilyProperties>	m_QueueFamilies;	// Queue Families represent a Queue to send data to the GPU
	std::vector<uint32_t>					m_QueueIndicies;	// Queue Indicies specify the location of the Queue Family
	uint32_t								m_QueueGraphicIndex;// The Queue Family Index for the graphic queue
	uint32_t								m_QueuePresentIndex;// The Queue Family Index for the present queue
	vk::Device								m_Device;			// Device is a virtualization of a Physical Device ( A Driverish representation? )
	vk::CommandPool							m_CommandPool;		// Command pool is used to send commands to the GPU ( LineWidth, PointSize, Etc )
	vk::CommandBuffer						m_CommandBuffer;	// Command buffer stores commands that are then passed into the CommandPools
	vk::SwapchainKHR						m_SwapChain;		// Swapchain is like a double buffer, it stores images that you can swap
	vk::SurfaceKHR							m_Surface;			// Surface is our window to draw into
	vk::Queue								m_PresentQueue;		// Transfer Queue
	vk::Queue								m_GraphicQueue;		// Graphic Queue
	std::vector<vk::Image>					m_Images;			// Our swapchain images
	vk::Extent2D							m_Extent2D;			// Window dimensions - 2D
	vk::Extent3D							m_Extent3D;			// Window dimensions - 3D
	vk::ImageView							m_DepthView;		// This gets attached to a resource telling it how it is used
	vk::Image								m_DepthImage;		// This is our depth buffer stored in an image
	vk::DeviceMemory						m_DepthMemory;		// Memory allocated for depth buffer
	glm::mat4								m_Model;			// Model ( position, rotation, scale, etc )
	glm::mat4								m_View;				// View ( camera position, rotation, scale )
	glm::mat4								m_Projection;		// Projection ( 3D extent, clipping region, etc )
	glm::mat4								m_Clip;				// Specify the clip space
	glm::mat4								m_MVP;				// Combination of clip, proj, view, model
	vk::Buffer								m_UniformBuffer;	// Uniform buffer to send to the pipeline
	vk::DeviceMemory						m_UniformMemory;	// Uniform Memory allocated for the uniform
};