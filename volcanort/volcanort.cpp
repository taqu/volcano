#include "volcanort.h"
#include "volcanort_conf.h"
#include <stdlib.h>

struct volcanort_device
{
    void* (*allocate_)(size_t);
    void (*deallocate_)(void*);
};

#ifdef VOLCANORT_BACKEND_CPU
#include "cpu/volcanort_cpu.h"
#include "cpu/volcanort_cpu.cpp"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

volcanort_device* volcanort_create_device(void* (*allocate)(size_t), void (*deallocate)(void*))
{
    if(nullptr == allocate){
        allocate = malloc;
    }
    if(nullptr == deallocate){
        deallocate = free;
    }
    volcanort_device* device = reinterpret_cast<volcanort_device*>(allocate(sizeof(volcanort_device)));
    if(nullptr == device){
        return nullptr;
    }
    device->allocate_ = allocate;
    device->deallocate_ = deallocate;
    return device;
}

#ifdef __cplusplus
}
#endif

