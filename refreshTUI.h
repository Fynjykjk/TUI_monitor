#ifndef REFRESH_TUI_H
#define REFRESH_TUI_H

#include <ftxui/component/screen_interactive.hpp>

class TuiRenderer {
public:
    explicit TuiRenderer(const status_t& store)
        : store_(store),
          screen_(ftxui::ScreenInteractive::TerminalOutput()) {}

    void run() {
        // Создаем компонент-рендерер FTXUI
        auto renderer = ftxui::Renderer([this] {
            auto snap = store_.get_snapshot();
            update_temp_history(snap.cpu_temp);
            return build_ui(snap);
        });

        // Кастомная обработка клавиши 'q' для выхода
        auto component = ftxui::CatchEvent(renderer, [this](ftxui::Event event) {
            if (event == ftxui::Event::Character('q') || event == ftxui::Event::Escape) {
                screen_.ExitLoopClosure()();
                return true;
            }
            return false;
        });

        screen_.Loop(component);
    }

    // Метод для сигнализации из фонового потока
    void request_refresh() {
        screen_.PostEvent(ftxui::Event::Custom);
    }

private:
    const status_t& store_;
    ftxui::ScreenInteractive screen_;
    
    std::deque<double> temp_history_;
    static constexpr size_t MAX_HISTORY_POINTS = 100;

    void update_temp_history(double temp) {
        temp_history_.push_back(temp);
        if (temp_history_.size() > MAX_HISTORY_POINTS) {
            temp_history_.pop_front();
        }
    }

    // Рендеринг загрузки 4 ядер CPU
    ftxui::Element render_cpu_cores(const std::array<double, 4>& loads) {
        ftxui::Elements core_widgets;
        
        for (size_t i = 0; i < loads.size(); ++i) {
            double load = std::clamp(loads[i], 0.0, 100.0);
            
            // Выбор цвета в зависимости от нагрузки
            auto bar_color = ftxui::Color::Green;
            if (load > 85.0) bar_color = ftxui::Color::Red;
            else if (load > 60.0) bar_color = ftxui::Color::Yellow;

            std::string label = "Core " + std::to_string(i) + ": ";
            std::string pct_text = std::to_string(static_cast<int>(load)) + "%";

            core_widgets.push_back(
                ftxui::hbox({
                    ftxui::text(label) | ftxui::bold,
                    ftxui::gauge(load / 100.0) | ftxui::color(bar_color) | ftxui::flex,
                    ftxui::text(" " + pct_text) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 5)
                })
            );
        }

        return ftxui::window(ftxui::text(" CPU Cores Load ") | ftxui::bold, 
                             ftxui::vbox(std::move(core_widgets)));
    }

    // Рендеринг графика температур
    ftxui::Element render_temp_chart(double current_temp) {
        using namespace ftxui;

        // Генератор точек для виджета graph()
        auto graph_data = [this](int width, int height) {
            std::vector<int> output(width, 0);
            if (temp_history_.empty()) return output;

            size_t count = std::min(static_cast<size_t>(width), temp_history_.size());
            
            // Диапазон шкалы температуры: от 30°C до 85°C
            const double min_temp = 30.0;
            const double max_temp = 85.0;

            for (size_t i = 0; i < count; ++i) {
                size_t hist_idx = temp_history_.size() - count + i;
                double t = temp_history_[hist_idx];
                
                // Нормализация значения в сетку высоты терминала [0, height - 1]
                double normalized = (t - min_temp) / (max_temp - min_temp);
                int y = static_cast<int>(normalized * (height - 1));
                
                output[i] = std::clamp(y, 0, height - 1);
            }
            return output;
        };

        std::string temp_str = std::to_string(static_cast<int>(current_temp)) + " °C";

        return window(
            text(" Thermal Sensor (" + temp_str + ") ") | bold,
            vbox({
                graph(std::move(graph_data)) | color(Color::RedLight) | flex,
                hbox({
                    text("30°C") | dim,
                    filler(),
                    text("85°C") | dim
                })
            })
        );
    }

    // Компоновка всего UI
    ftxui::Element build_ui(const StatusSnapshot& snap) {
        using namespace ftxui;

        auto header = hbox({
            text(" RASPBERRY PI 5 MONITOR ") | bold | color(Color::Cyan),
            filler(),
            text(" Press 'q' to exit ") | dim
        }) | border;

        auto left_panel = render_cpu_cores(snap.cpu_load);
        auto right_panel = render_temp_chart(snap.cpu_temp);

        auto main_content = hbox({
            left_panel | flex,
            right_panel | flex
        });

        return vbox({
            header,
            main_content | flex
        });
    }
};
#endif // !REFRESH_TUI_H