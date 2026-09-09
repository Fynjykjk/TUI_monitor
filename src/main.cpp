#include <iostream>

#include "refreshTUI.h"
#include "status.h"
#include "parse.h"

std::array<cpu_ticks, 4> curr_cpu_ticks{};
auto screen = ftxui::ScreenInteractive::TerminalOutput();

using namespace std::chrono_literals;

void worker(parse_data& parser, status_t& status)
{
    while (status.get_is_running())
    {
        std::this_thread::sleep_for(5s);
        parser.cpu_load_parse(status, curr_cpu_ticks);  
        parser.ram_parse(status);
        parser.temp_parse(status);
        parser.freq_parse(status);

        screen.PostEvent(ftxui::Event::Custom);
    }
}

int main()
{
    using namespace ftxui;

    parse_data parser;
    status_t status;
    std::thread t(worker, std::ref(parser), std::ref(status));

    //Поток рендера
    auto ui = Renderer([&] {
        return vbox({
            text(" Real-time Hardware Monitor ") | bold | center,
            separator(),
            hbox({
                text("CPU load: "),
                gauge((status.get_cpu_load())[0]) | color(Color::Yellow),
                gauge((status.get_cpu_load())[1]) | color(Color::Yellow),
                gauge((status.get_cpu_load())[2]) | color(Color::Yellow),
                gauge((status.get_cpu_load())[3]) | color(Color::Yellow),
            }),
            hbox({
                text("RAM usage: "),
                gauge(status.get_ram_load()) | color(Color::Green),
                text("Total: " + std::to_string(status.get_ram_total_mb()) + " MB") | color(Color::Cyan)
            }),
            hbox({
                text("CPU temp: "),
                gauge(static_cast<double>(status.get_cpu_temp() / 110.)) | color(Color::Blue),
                text("Now: " + std::to_string(status.get_cpu_temp()) + "C") | color(Color::Cyan)
            }),
            hbox({
                text("CPU freq: "),
                text(std::to_string(status.get_cpu_freq_mhz()) + "MHz") | color(Color::Red)
            })
        }) | border;
    });

    //Обработка нажатий
    Component EventProcessing = CatchEvent(ui, [&](Event event) 
    {
        if (event == Event::Character('q') || event == Event::Escape) 
        {
            status.is_running_switch();
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(EventProcessing);
    t.join();
    
    return 0;
}