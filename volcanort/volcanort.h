#ifndef INC_VOLCANORT_H_
#define INC_VOLCANORT_H_
#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

struct volcanort_device;

struct volcanort_aabb
{
    float bmin_[3];
    float bmax_[3];
};

#ifdef __cplusplus
extern "C"
{
#endif
volcanort_device* volcanort_create_device(void* (*allocate)(size_t), void (*deallocate)(void*));
#ifdef __cplusplus
}
#endif
#endif //INC_VOLCANORT_H_
