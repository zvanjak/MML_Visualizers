#ifndef LEGEND_WIDGET_H
#define LEGEND_WIDGET_H

#include <FL/Fl_Group.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include "MMLData.h"
#include <vector>
#include <string>
#include <functional>

// Custom color box widget to show curve color
class ColorBox : public Fl_Widget {
private:
    CurveColor color_;
    
public:
    ColorBox(int X, int Y, int W, int H, const CurveColor& color)
        : Fl_Widget(X, Y, W, H), color_(color) {}
    
    void draw() override {
        fl_color(color_.r, color_.g, color_.b);
        fl_rectf(x(), y(), w(), h());
        fl_color(FL_BLACK);
        fl_rect(x(), y(), w(), h());
    }
    
    void SetColor(const CurveColor& color) {
        color_ = color;
        redraw();
    }
};

// Legend item containing checkbox, color box
struct LegendEntry {
    Fl_Check_Button* checkbox;
    ColorBox* colorBox;
    int curveIndex;
    
    LegendEntry() : checkbox(nullptr), colorBox(nullptr), curveIndex(0) {}
};

// Callback type for visibility changes
using VisibilityChangedCallback = std::function<void(int curveIndex, bool visible)>;

// Legend widget with visibility checkboxes
class LegendWidget : public Fl_Group {
private:
    std::vector<LegendEntry> entries_;
    std::vector<std::string> titles_;
    VisibilityChangedCallback visibilityCallback_;
    int nextY_;
    int itemHeight_;
    
    static void CheckboxCallback(Fl_Widget* widget, void* data);
    
public:
    LegendWidget(int X, int Y, int W, int H, const char* L = nullptr);
    ~LegendWidget();
    
    // Add a legend item with checkbox
    void AddItem(const std::string& title, const CurveColor& color, int curveIndex, bool visible = true);
    
    // Clear all legend items
    void Clear();
    
    // Set callback for visibility changes
    void SetVisibilityCallback(VisibilityChangedCallback callback);
    
    // Get/Set visibility for a curve
    bool IsVisible(int curveIndex) const;
    void SetVisible(int curveIndex, bool visible);
    
    // Get number of items
    size_t GetItemCount() const { return entries_.size(); }
    
    void draw() override;
};

#endif // LEGEND_WIDGET_H
