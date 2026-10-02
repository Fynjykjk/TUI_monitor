#ifndef STATUS_H_
#define STATUS_H_

#include <array>
#include <mutex>
#include <atomic>

class status_t
{
    public:

        // getters
        size_t get_ram_used_mb() const;
        
        size_t get_ram_total_mb() const;

        double get_cpu_temp() const;

        size_t get_cpu_freq_mhz() const;

        const std::array<double, 4>& get_cpu_load() const;

        bool get_is_running() const;

        double get_ram_load() const;

        //setters
        void is_running_switch();ds

    private:
        size_t ram_used_mb{};
        size_t ram_total_mb{};
        double ram_load{};
        double cpu_temp{};
        size_t cpu_freq_mhz{};
        std::array<double, 4> cpu_load = {};

        mutable std::mutex mtx;

        std::atomic<bool> is_running = true;
        
        friend class parse_data;
};

#endif  // !STATUS_H_
