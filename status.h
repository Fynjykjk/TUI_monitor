#ifndef STATUS_H_
#define STATUS_H_
#include <array>
#include <mutex>

class status_t
{
    public:
        status_t();
        ~status_t();

        // getters
        size_t get_ram_used_mb() const;
        
        size_t get_ram_total_mb() const;

        double get_cpu_temp() const;

        size_t get_cpu_freq_mhz() const;

        const std::array<double, 4>& get_cpu_load() const;

    private:
        size_t ram_used_mb{};
        size_t ram_total_mb{};
        double cpu_temp{};
        size_t cpu_freq_mhz{};
        std::array<double, 4> cpu_load = {};
        mutable std::mutex mtx;

        friend class parse_data;
};

#endif  // !STATUS_H_