#include "parse.h"

int parse_data::cpu_load_parse(status_t& status, std::array<cpu_ticks, 4>& curr_cpu_ticks)
{
    std::ifstream cpu_load_file("/proc/stat");
    if (!cpu_load_file.is_open())
    {
        status.is_running_switch();
        return -1; // Error opening file
    }

    std::string line;
    int i{0};
    auto prev_cpu_ticks = curr_cpu_ticks;

    while (std::getline(cpu_load_file, line))
    {
        if (line.compare(0, 3, "cpu") == 0 && line.size() > 3 && std::isdigit(line[3]))
        {
            std::istringstream ss(line);
            std::string cpu_label;
            size_t user, nice, system, idle, iowait, irq, softirq, steal;
            ss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
            size_t idle_time = idle + iowait;
            size_t total_time = user + nice + system + idle + iowait + irq + softirq + steal;
            curr_cpu_ticks[i] = {idle_time, total_time};

            size_t delta_idle = curr_cpu_ticks[i].idle_time - prev_cpu_ticks[i].idle_time;
            size_t delta_total = curr_cpu_ticks[i].total_time - prev_cpu_ticks[i].total_time;
            if (delta_total == 0)
            {
                status.cpu_load[i] = 0.0;
            }
            else
            {
                double cpu_usage = (1.0 - static_cast<double>(delta_idle) / delta_total) * 100.0;

                std::lock_guard<std::mutex> lock(status.mtx);
                status.cpu_load[i] = cpu_usage;
            }
            i++;
        }
    }

    cpu_load_file.close();
    return 0;
}

int parse_data::temp_parse(status_t& status)
{
    std::ifstream temp_file("/sys/class/thermal/thermal_zone0/temp");
    if (!temp_file.is_open())
    {
        status.is_running_switch();
        return -1; // Error opening file
    }

    int temp_millidegree;
    temp_file >> temp_millidegree;

    std::lock_guard<std::mutex> lock(status.mtx);
    status.cpu_temp = static_cast<double>(temp_millidegree) / 1000.0; // Convert to degrees Celsius

    temp_file.close();
    return 0;
}

int parse_data::freq_parse(status_t& status)
{
    std::ifstream freq_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
    if (!freq_file.is_open())
    {
        status.is_running_switch();
        return -1; // Error opening file
    }

    size_t freq_khz;
    freq_file >> freq_khz;
    
    std::lock_guard<std::mutex> lock(status.mtx);
    status.cpu_freq_mhz  = (freq_khz / 1000); // Convert to MHz

    freq_file.close();
    return 0;
}

int parse_data::ram_parse(status_t& status)
{
    std::ifstream meminfo_file("/proc/meminfo");
    if (!meminfo_file.is_open())
    {
        status.is_running_switch();
        return -1; // Error opening file
    }

    std::string line;
    size_t mem_total_kb = 0;
    size_t mem_free_kb = 0;
    size_t buffers_kb = 0;
    size_t cached_kb = 0;

    while (std::getline(meminfo_file, line))
    {
        std::istringstream ss(line);
        std::string key;
        size_t value;
        std::string unit;
        ss >> key >> value >> unit;

        if (key == "MemTotal:")
        {
            mem_total_kb = value;
        }
        else if (key == "MemFree:")
        {
            mem_free_kb = value;
        }
        else if (key == "Buffers:")
        {
            buffers_kb = value;
        }
        else if (key == "Cached:")
        {
            cached_kb = value;
        }
    }

    std::lock_guard<std::mutex> lock(status.mtx);
    status.ram_used_mb = (mem_total_kb - mem_free_kb - buffers_kb - cached_kb) / 1024; // Convert to MB
    status.ram_total_mb = mem_total_kb / 1024; // Convert to MB
    status.ram_load = (status.ram_total_mb > 0) 
    ? static_cast<double>(status.ram_used_mb) / static_cast<double>(status.ram_total_mb) 
    : 0.0f;

    meminfo_file.close();
    return 0;
}