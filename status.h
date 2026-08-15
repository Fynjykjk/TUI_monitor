#ifndef PREPARE_H_
#define PREPARE_H_
#include <array>
#include <mutex>

class status_t
{
    public:
        status_t();
        ~status_t();


        const std::vector<double>& get_data() const;

    private:
        size_t ram_used_mb{};
        size_t ram_total_mb{};
        double cpu_temp{};
        size_t cpu_freq_mhz{};
        std::array<double, 4> cpu_load = {};
        std::mutex mtx;

        friend class parse_data;
};

#endif  // !PREPARE_H_