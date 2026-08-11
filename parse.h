#ifndef PARSE_H_
#define PARSE_H_
#include <vector>

/// @brief A class to hold parsed data
class parse_data
{
    public:
        parse_data();
        ~parse_data();

        const std::vector<double>& get_data() const;

        int cpu_load();
    private:
        std::vector<double> data;
};

#endif  // !PARSE_H_