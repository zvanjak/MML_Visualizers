#include "LegendWidget.h"
#include <FL/Fl.H>

// ColorBox implementation
ColorBox::ColorBox(int X, int Y, int W, int H, const LegendColor& color)
    : Fl_Widget(X, Y, W, H), color_(color) {
}

void ColorBox::draw() {
    // Draw filled color box
    fl_color(color_.r, color_.g, color_.b);
    fl_rectf(x(), y(), w(), h());
    
    // Draw border
    fl_color(0, 0, 0);
    fl_rect(x(), y(), w(), h());
}

void ColorBox::SetColor(const LegendColor& color) {
    color_ = color;
    redraw();
}

// LegendWidget implementation
LegendWidget::LegendWidget(int X, int Y, int W, int H, const char* L)
    : Fl_Group(X, Y, W, H, L), nextY_(Y + 5), itemHeight_(28) {
    end(); // End the group initially
}

LegendWidget::~LegendWidget() {
    Clear();
}

void LegendWidget::CheckboxCallback(Fl_Widget* widget, void* data) {
    LegendWidget* legend = static_cast<LegendWidget*>(data);
    Fl_Check_Button* checkbox = static_cast<Fl_Check_Button*>(widget);
    
    // Find which entry this checkbox belongs to
    for (auto& entry : legend->entries_) {
        if (entry.checkbox == checkbox) {
            bool visible = checkbox->value() != 0;
            if (legend->visibilityCallback_) {
                legend->visibilityCallback_(entry.functionIndex, visible);
            }
            break;
        }
    }
}

void LegendWidget::AddItem(const std::string& title, const LegendColor& color, int functionIndex, bool visible) {
    begin(); // Begin adding children to this group
    
    int leftMargin = 10;
    int checkboxWidth = 24;
    int colorBoxWidth = 25;
    int colorBoxHeight = 16;
    int spacing = 5;
    
    // Create checkbox
    Fl_Check_Button* checkbox = new Fl_Check_Button(
        x() + leftMargin, nextY_, checkboxWidth, itemHeight_, "");
    checkbox->value(visible ? 1 : 0);
    checkbox->callback(CheckboxCallback, this);
    
    // Create color box
    ColorBox* colorBox = new ColorBox(
        x() + leftMargin + checkboxWidth + spacing,
        nextY_ + (itemHeight_ - colorBoxHeight) / 2,
        colorBoxWidth,
        colorBoxHeight,
        color);
    
    // Store entry
    LegendEntry entry;
    entry.checkbox = checkbox;
    entry.colorBox = colorBox;
    entry.functionIndex = functionIndex;
    entries_.push_back(entry);
    
    // Store title
    titles_.push_back(title);
    
    nextY_ += itemHeight_;
    
    end(); // End adding children
    redraw();
}

void LegendWidget::Clear() {
    // Remove all children
    for (auto& entry : entries_) {
        if (entry.checkbox) {
            remove(entry.checkbox);
            delete entry.checkbox;
        }
        if (entry.colorBox) {
            remove(entry.colorBox);
            delete entry.colorBox;
        }
    }
    entries_.clear();
    titles_.clear();
    nextY_ = y() + 5;
    redraw();
}

void LegendWidget::SetVisibilityCallback(VisibilityChangedCallback callback) {
    visibilityCallback_ = callback;
}

bool LegendWidget::IsVisible(int functionIndex) const {
    for (const auto& entry : entries_) {
        if (entry.functionIndex == functionIndex) {
            return entry.checkbox && entry.checkbox->value() != 0;
        }
    }
    return true;
}

void LegendWidget::SetVisible(int functionIndex, bool visible) {
    for (auto& entry : entries_) {
        if (entry.functionIndex == functionIndex && entry.checkbox) {
            entry.checkbox->value(visible ? 1 : 0);
            break;
        }
    }
}

std::string LegendWidget::GetTitle(int functionIndex) const {
    if (functionIndex >= 0 && functionIndex < static_cast<int>(titles_.size())) {
        return titles_[functionIndex];
    }
    return "";
}

void LegendWidget::SetTitle(int functionIndex, const std::string& title) {
    if (functionIndex >= 0 && functionIndex < static_cast<int>(titles_.size())) {
        titles_[functionIndex] = title;
        redraw();
    }
}

void LegendWidget::draw() {
    // Draw background
    fl_color(250, 250, 250);
    fl_rectf(x(), y(), w(), h());
    
    // Draw border
    fl_color(180, 180, 180);
    fl_rect(x(), y(), w(), h());
    
    // Draw children (checkboxes and color boxes)
    Fl_Group::draw();
    
    // Draw titles next to color boxes
    fl_font(FL_HELVETICA, 12);
    fl_color(0, 0, 0);
    
    int leftMargin = 10;
    int checkboxWidth = 24;
    int colorBoxWidth = 25;
    int spacing = 5;
    int textX = x() + leftMargin + checkboxWidth + spacing + colorBoxWidth + spacing;
    int currentY = y() + 5;
    
    for (size_t i = 0; i < titles_.size(); ++i) {
        int textY = currentY + itemHeight_ / 2 + 4;
        fl_draw(titles_[i].c_str(), textX, textY);
        currentY += itemHeight_;
    }
}
