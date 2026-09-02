#include "status.h"

size_t status_t::get_ram_used_mb() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return ram_used_mb;
}

size_t status_t::get_ram_total_mb() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return ram_total_mb;
}

double status_t::get_cpu_temp() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return cpu_temp;
}

size_t status_t::get_cpu_freq_mhz() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return cpu_freq_mhz;
}

const std::array<double, 4>& status_t::get_cpu_load() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return cpu_load;
}