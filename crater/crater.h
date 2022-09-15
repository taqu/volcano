#ifndef INC_CRATER_H_
#define INC_CRATER_H_
/**
 */
#define VK_NO_PROTOTYPES (1)
#include <vulkan/vulkan.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#define CRATER_VK_EXPORTED_FUNCTION(NAME) extern PFN_##NAME NAME;
#define CRATER_VK_EXT_INSTANCE_FUNCTION(NAME) extern PFN_##NAME NAME;
#include "crater_vk_funcs.inc"

#ifdef __cplusplus
struct crater_device
#ifdef _WIN32
#define CRATER_API __cdecl
#else
#define CRATER_API
#endif
#else
typedef struct crater_device_t
#ifdef _WIN32
#define CRATER_API __cdecl
#else
#define CRATER_API
#endif
#endif
{
#ifdef __cplusplus
    operator VkDevice()
    {
        return device_;
    }
#endif
    VkDevice device_;

#define CRATER_VK_EXT_DEVICE_FUNCTION(NAME) PFN_##NAME NAME;
#include "crater_vk_funcs.inc"
#ifdef __cplusplus
};
#else
} crater_device;
#endif

typedef int32_t (VKAPI_PTR *PFN_crater_device_features)(VkPhysicalDevice physical_device);

/**
 @brief

 "vulkan-1.dll", ""
 */
bool CRATER_API initialize_crater(const char* vulkan_dynamic);
void CRATER_API terminate_crater();

extern VkInstance vk_instance;

void CRATER_API vk_enumerate_available_layers(uint32_t* layer_count, char const ** const dst_layers, char const * const * const src_layers);
void CRATER_API vk_enumerate_available_instance_extensions(uint32_t* extension_count, char const ** const dst_extensions, char const * const * const src_extensions);

VkResult CRATER_API vk_create_instance(const VkInstanceCreateInfo* create_info, const VkAllocationCallbacks* allocator);
void CRATER_API vk_destroy_instance(const VkAllocationCallbacks* allocator);

void CRATER_API vk_create_debug_utils_message(VkDebugUtilsMessengerCreateFlagsEXT flags, VkDebugUtilsMessageSeverityFlagsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, PFN_vkDebugUtilsMessengerCallbackEXT callback, void* user_data, const VkAllocationCallbacks* allocator);
void CRATER_API vk_destroy_debug_utils_message(const VkAllocationCallbacks* allocator);

VkResult CRATER_API vk_create_device(VkPhysicalDevice physical_device, const VkDeviceCreateInfo* create_info, const VkAllocationCallbacks* allocator, crater_device* device);
void CRATER_API vk_destroy_device(crater_device* device, const VkAllocationCallbacks* allocator);

void CRATER_API vk_choose_physical_devices(
    uint32_t* physical_device_count,
    VkPhysicalDevice* physical_devices,
    int32_t* priorities,
    PFN_crater_device_features device_features);
#endif //INC_CRATER_H_

