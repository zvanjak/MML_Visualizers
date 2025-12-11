#include "LegendWidget.h"

LegendWidget::LegendWidget(int X, int Y, int W, int H, const char* L)
    : Fl_Widget(X, Y, W, H, L) {
}

void LegendWidget::AddItem(const std::string& title, const Color& color) {
    items_.emplace_back(title, color);
}

void LegendWidget::Clear() {
    items_.clear();
}

void LegendWidget::draw() {
    // Draw background
    fl_color(250, 250, 250); // Light gray background
    fl_rectf(x(), y(), w(), h());
    
    // Draw border
    fl_color(0, 0, 0);
    fl_rect(x(), y(), w(), h());
    
    // Draw legend items
    fl_font(FL_HELVETICA, 12);
    int itemHeight = 25;
    int yPos = y() + 10;
    
    for (const auto& item : items_) {
        // Draw color box
        fl_color(item.color.r, item.color.g, item.color.b);
        fl_rectf(x() + 10, yPos, 20, 15);
        
        // Draw border around color box
        fl_color(0, 0, 0);
        fl_rect(x() + 10, yPos, 20, 15);
        
        // Draw text
        fl_draw(item.title.c_str(), x() + 35, yPos + 12);
        
        yPos += itemHeight;
    }
}
