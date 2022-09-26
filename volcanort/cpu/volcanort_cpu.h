#ifndef INC_VOLCANORT_CPU_H_
#define INC_VOLCANORT_CPU_H_
#include <cstdint>

namespace volcanort
{
class volcanort_tlas_cpu
{
public:
    static constexpr uint64_t MinAlign = 32;
};

class volcanort_blas_cpu
{
public:
};

class volcanort_device_cpu: public volcanort_device
{
public:
};
} // namespace volcanort
#endif // INC_VOLCANORT_CPU_H_
