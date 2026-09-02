#include <iostream>
#include <thread>
#include "refreshTUI.h"
#include "status.h"
#include "parse.h"

std::array<cpu_ticks, 4> curr_cpu_ticks{};

void worker(parse_data& parser, status_t& status)
{
    while ()
    parser.cpu_load_parse(status, curr_cpu_ticks);  

    parser.ram_parse(status);
    
    parser.temp_parse(status);
    
    parser.freq_parse(status);
}

int main()
{
    parse_data parser;
    status_t status;
    std::thread t(worker, std::ref(parser), std::ref(status));

    t.join();
    return 0;
}