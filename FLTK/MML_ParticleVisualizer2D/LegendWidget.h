#ifndef LEGEND_WIDGET_H
#define LEGEND_WIDGET_H

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <vector>
#include <string>
#include "MMLData.h"

struct LegendItem {
    std::string title;
    Color color;
    
    LegendItem(const std::string& t, const Color& c) : title(t), color(c) {}
};

class LegendWidget : public Fl_Widget {
private:
    std::vector<LegendItem> items_;
    
public:
    LegendWidget(int X, int Y, int W, int H, const char* L = nullptr);
    
    void draw() override;
    
    void AddItem(const std::string& title, const Color& color);
    void Clear();
};

#endif // LEGEND_WIDGET_H
