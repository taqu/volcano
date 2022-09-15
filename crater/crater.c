/**
 */
#include "crater.h"
#include <assert.h>
#include <string.h>

#ifdef _WIN32
#    include <Windows.h>
#endif

#define CRATER_VK_EXPORTED_FUNCTION(NAME) PFN_##NAME NAME;
#define CRATER_VK_EXT_INSTANCE_FUNCTION(NAME) PFN_##NAME NAME;
#include "crater_vk_funcs.inc"

#ifdef __cplusplus
namespace
{
#    define CRATER_NULL nullptr
using HANDLE = HMODULE;
HANDLE instance_ = CRATER_NULL;
} // namespace
#else
#    define CRATER_NULL NULL
typedef void* HANDLE;
static HANDLE instance_ = NULL;
#endif

bool CRATER_API initialize_crater(const char* vulkan_dynamic)
{
    assert(CRATER_NULL != vulkan_dynamic);
    if(CRATER_NULL != instance_) {
        return true;
    }

#ifdef _WIN32
    instance_ = LoadLibraryA(vulkan_dynamic);
    if(CRATER_NULL == instance_) {
        return false;
    }
#    define CRATER_VK_EXPORTED_FUNCTION(NAME) NAME = (PFN_##NAME)GetProcAddress(instance_, #    NAME);
#else
    instance_ = dlopen(vulkan_dynamic, RTLD_NOW);
    if(CRATER_NULL == instance_) {
        return false;
    }
#    define CRATER_VK_EXPORTED_FUNCTION(NAME) NAME = (PFN_##NAME)dlsym(instance_, #    NAME);
#endif
#    include "crater_vk_funcs.inc"
    return true;
}

void CRATER_API terminate_crater()
{
    if(CRATER_NULL == instance_) {
        return;
    }
#ifdef _WIN32
    FreeLibrary(instance_);
#else
    dlclose(instance_);
#endif
    instance_ = CRATER_NULL;
}

//--- VkInstance
VkInstance vk_instance = CRATER_NULL;

#ifdef _DEBUG
//--- VkDebugUtilsMessengerEXT
static VkDebugUtilsMessengerEXT vk_debug_utils_messenger = CRATER_NULL;
#endif

static bool vk_find_string(uint32_t* index, uint32_t size, char const* const* strings, char const* const string)
{
    for(uint32_t i = 0; i < size; ++i) {
        if(0 == strcmp(strings[i], string)) {
            *index = i;
            return true;
        }
    }
    return false;
}

void CRATER_API vk_enumerate_available_layers(uint32_t* layer_count, char const** const dst_layers, char const* const* const src_layers)
{
    assert(CRATER_NULL != layer_count);
    assert(CRATER_NULL != dst_layers);
    assert(CRATER_NULL != src_layers);
    uint32_t count = 0;
    uint32_t property_count = 64;
    VkLayerProperties properties[64];
    VkResult result = vkEnumerateInstanceLayerProperties(&property_count, properties);
    if(VK_SUCCESS != result && VK_INCOMPLETE != result || property_count <= 0) {
        *layer_count = 0;
        return;
    }
    for(uint32_t i = 0; i < property_count; ++i) {
        uint32_t index = -1;
        if(vk_find_string(&index, *layer_count, src_layers, properties[i].layerName)) {
            dst_layers[count] = src_layers[index];
            ++count;
        }
    }
    *layer_count = count;
}

void CRATER_API vk_enumerate_available_instance_extensions(uint32_t* extension_count, char const** const dst_extensions, char const* const* const src_extensions)
{
    assert(CRATER_NULL != extension_count);
    assert(CRATER_NULL != dst_extensions);
    assert(CRATER_NULL != src_extensions);
    uint32_t count = 0;
    uint32_t property_count = 64;
    VkExtensionProperties properties[64];
    VkResult result = vkEnumerateInstanceExtensionProperties(CRATER_NULL, &property_count, properties);
    if(VK_SUCCESS != result && VK_INCOMPLETE != result || property_count <= 0) {
        *extension_count = 0;
        return;
    }
    for(uint32_t i = 0; i < property_count; ++i) {
        uint32_t index = -1;
        if(vk_find_string(&index, *extension_count, src_extensions, properties[i].extensionName)) {
            dst_extensions[count] = src_extensions[index];
            ++count;
        }
    }
    *extension_count = count;
}

VkResult CRATER_API vk_create_instance(const VkInstanceCreateInfo* create_info, const VkAllocationCallbacks* allocator)
{
    assert(CRATER_NULL != create_info);
    if(CRATER_NULL != vk_instance) {
        return VK_SUCCESS;
    }
    VkResult result = vkCreateInstance(create_info, allocator, &vk_instance);
    if(VK_SUCCESS != result){
        return result;
    }
#define CRATER_VK_EXT_INSTANCE_FUNCTION(NAME) NAME = (PFN_##NAME)vkGetInstanceProcAddr(vk_instance, #NAME);
#include "crater_vk_funcs.inc"
    return VK_SUCCESS;
}

void CRATER_API vk_destroy_instance(const VkAllocationCallbacks* allocator)
{
    if(CRATER_NULL == vk_instance) {
        return;
    }
#define CRATER_VK_EXT_INSTANCE_FUNCTION(NAME) NAME = CRATER_NULL;
#include "crater_vk_funcs.inc"

    vkDestroyInstance(vk_instance, allocator);
    vk_instance = CRATER_NULL;
}

void CRATER_API vk_create_debug_utils_message(
    VkDebugUtilsMessengerCreateFlagsEXT flags,
    VkDebugUtilsMessageSeverityFlagsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    PFN_vkDebugUtilsMessengerCallbackEXT callback,
    void* user_data,
    const VkAllocationCallbacks* allocator)
{
#ifdef _DEBUG
    if(CRATER_NULL != vk_debug_utils_messenger){
        vkDestroyDebugUtilsMessengerEXT(vk_instance, vk_debug_utils_messenger, allocator);
        vk_debug_utils_messenger = CRATER_NULL;
    }
    VkDebugUtilsMessengerCreateInfoEXT create_info = {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        CRATER_NULL,
        flags,
        severity,
        type,
        callback,
        user_data,
    };
    vkCreateDebugUtilsMessengerEXT(vk_instance, & create_info, allocator, &vk_debug_utils_messenger);
#else
    (void)flags;
    (void)severity;
    (void)type;
    (void)callback;
    (void)user_data;
    (void)allocator;
#endif
}

void CRATER_API vk_destroy_debug_utils_message(const VkAllocationCallbacks* allocator)
{
#ifdef _DEBUG
    if(CRATER_NULL == vk_debug_utils_messenger){
        return;
    }
    vkDestroyDebugUtilsMessengerEXT(vk_instance, vk_debug_utils_messenger, allocator);
    vk_debug_utils_messenger = CRATER_NULL;
#else
    (void)allocator;
#endif
}

VkResult CRATER_API vk_create_device(VkPhysicalDevice physical_device, const VkDeviceCreateInfo* create_info, const VkAllocationCallbacks* allocator, crater_device* device)
{
    assert(CRATER_NULL != create_info);
    if(CRATER_NULL == device) {
        return VK_ERROR_UNKNOWN;
    }
    VkResult result = vkCreateDevice(physical_device, create_info, allocator, &(device->device_));
    if(VK_SUCCESS != result) {
        return result;
    }
#define CRATER_VK_EXT_DEVICE_FUNCTION(NAME) device->NAME = (PFN_##NAME)vkGetDeviceProcAddr(device->device_, #NAME);
#include "crater_vk_funcs.inc"

    return result;
}

void CRATER_API vk_destroy_device(crater_device* device, const VkAllocationCallbacks* allocator)
{
    if(CRATER_NULL == device) {
        return;
    }
#define CRATER_VK_EXT_DEVICE_FUNCTION(NAME) device->NAME = CRATER_NULL;
#include "crater_vk_funcs.inc"

    vkDestroyDevice(device->device_, allocator);
}

static void vk_push_physical_device(
    uint32_t max_device_count,
    uint32_t* physical_device_count,
    VkPhysicalDevice* physical_devices,
    int32_t* priorities,
    int32_t priority,
    VkPhysicalDevice physical_device)
{
    assert(CRATER_NULL != physical_device_count);
    if(*physical_device_count <= 0) {
        physical_devices[0] = physical_device;
        priorities[0] = priority;
        *physical_device_count += 1;
        return;
    }
    uint32_t pos = max_device_count;
    for(uint32_t i = *physical_device_count; 0 < i; --i) {
        if(priorities[i - 1] < priority) {
            pos = i - 1;
            break;
        }
    }
    if(max_device_count <= pos) {
        return;
    }
    *physical_device_count = (*physical_device_count < max_device_count) ? *physical_device_count + 1 : max_device_count;
    for(uint32_t i = *physical_device_count - 1; pos < i; --i) {
        physical_devices[i] = physical_devices[i - 1];
        priorities[i] = priorities[i - 1];
    }
    physical_devices[pos] = physical_device;
    priorities[pos] = priority;
}

void CRATER_API vk_choose_physical_devices(
    uint32_t* physical_device_count,
    VkPhysicalDevice* physical_devices,
    int32_t* priorities,
    PFN_crater_device_features device_features)
{
    assert(CRATER_NULL != physical_device_count);
    assert(0 < *physical_device_count);
    assert(CRATER_NULL != physical_devices);
    assert(CRATER_NULL != priorities);
#ifdef __cplusplus
    static const uint32_t MaxTmpCount = 16;
#else
#    define MaxTmpCount (16)
#endif
    uint32_t tmp_device_count;
    VkPhysicalDevice tmp_devices[MaxTmpCount];
    uint32_t count = 0;
    tmp_device_count = MaxTmpCount;
    VkResult result = vkEnumeratePhysicalDevices(vk_instance, &tmp_device_count, tmp_devices);
    if(VK_SUCCESS != result && VK_INCOMPLETE != result || tmp_device_count <= 0) {
        *physical_device_count = 0;
        return;
    }
    for(uint32_t i = 0; i < tmp_device_count; ++i) {
        int32_t priority = (CRATER_NULL != device_features) ? device_features(tmp_devices[i]) : 0;
        if(priority < 0) {
            continue;
        }
        vk_push_physical_device(*physical_device_count, &count, physical_devices, priorities, priority, tmp_devices[i]);
    }
    *physical_device_count = count;
}
