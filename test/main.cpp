#include "crater.h"
#include <stdio.h>

int32_t crater_device_features(VkPhysicalDevice physical_device)
{
//void vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures);
//void vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties);
//VkResult vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties);
//void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties);
//void vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties);
//void vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties);

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);
    int32_t priority = 0;
    switch(properties.deviceType){
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        priority = 200;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU :
        priority = 400;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU :
        priority = 100;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU :
        priority = 100;
        break;
    //case VK_PHYSICAL_DEVICE_TYPE_OTHER:
    default:
        break;
    }

    uint32_t queue_count = 16;
    VkQueueFamilyProperties queue[16];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_count, queue);
    return priority;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    printf("%s\n", pCallbackData->pMessage);
    return VK_FALSE;
}

int main(void)
{
    initialize_crater("vulkan-1.dll");

    const char* layers[] = {
        "VK_LAYER_NV_optimus",
        "VK_LAYER_RENDERDOC_Capture",
        "VK_LAYER_KHRONOS_validation",
    };
    const char* available_layers[3] = {};
    uint32_t available_layer_count = 3;
    vk_enumerate_available_layers(&available_layer_count, available_layers, layers);

    const char* extensions[] = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    };
    const char* available_extensions[1] = {};
    uint32_t available_extensions_count = 1;
    vk_enumerate_available_instance_extensions(&available_extensions_count, available_extensions, extensions);

    VkApplicationInfo application_info = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "test",
        0,
        "test_engine",
        0,
        0,
    };
    VkInstanceCreateInfo create_info = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &application_info,
        available_layer_count, available_layers,
        available_extensions_count, available_extensions,
    };
    VkResult result = vk_create_instance(&create_info, nullptr);

    vk_create_debug_utils_message(
        0,
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        debug_callback,
        nullptr,
        nullptr);

    uint32_t physical_device_count = 1;
    VkPhysicalDevice physical_devices[1];
    int32_t priorities[1];
    vk_choose_physical_devices(&physical_device_count, physical_devices, priorities, crater_device_features);

    vk_destroy_debug_utils_message(nullptr);
    vk_destroy_instance(nullptr);
    terminate_crater();
    return 0;
}
