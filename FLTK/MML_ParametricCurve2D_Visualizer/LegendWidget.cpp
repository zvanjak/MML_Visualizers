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
    fl_color(FL_WHITE);
    fl_rectf(x(), y(), w(), h());
    
    // Draw border
    fl_color(FL_BLACK);
    fl_rect(x(), y(), w(), h());
    
    // Draw title
    if (label()) {
        fl_color(FL_BLACK);
        fl_font(FL_HELVETICA_BOLD, 14);
        fl_draw(label(), x() + 10, y() + 20);
    }
    
    // Draw legend items
    int yPos = y() + 40;
    fl_font(FL_HELVETICA, 12);
    
    for (const auto& item : items_) {
        // Draw color box
        fl_color(item.color.r, item.color.g, item.color.b);
        fl_rectf(x() + 10, yPos - 10, 20, 10);
        
        // Draw border around color box
        fl_color(FL_BLACK);
        fl_rect(x() + 10, yPos - 10, 20, 10);
        
        // Draw title
        fl_draw(item.title.c_str(), x() + 35, yPos);
        
        yPos += 20;
        
        if (yPos > y() + h() - 10) break; // Prevent overflow
    }
}
