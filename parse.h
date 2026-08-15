#ifndef PARSE_H_
#define PARSE_H_

#include <vector>
#include <fstream>
#include <sstream>

struct cpu_ticks
{
    size_t idle_time;
    size_t total_time;
};

/// @brief A class to hold parsed data
class parse_data
{
    public:
        parse_data();
        ~parse_data();

        const std::vector<double>& get_data() const;

        int cpu_load_parse(status_t& status, std::array<cpu_ticks, 4>& curr_cpu_ticks);

        int ram_parse(status_t& status);

        int temp_parse(status_t& status);

        int freq_parse(status_t& status); 

};

#endif  // !PARSE_H_