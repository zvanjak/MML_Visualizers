#ifndef LEGEND_WIDGET_H
#define LEGEND_WIDGET_H

#include <FL/Fl_Group.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <vector>
#include <string>
#include <functional>

// Color struct (duplicated from GraphWidget.h to avoid circular dependencies)
struct LegendColor {
    unsigned char r, g, b;
    LegendColor(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0)
        : r(red), g(green), b(blue) {}
};

// Forward declaration
class LegendWidget;

// Custom color box widget to show function color
class ColorBox : public Fl_Widget {
private:
    LegendColor color_;
    
public:
    ColorBox(int X, int Y, int W, int H, const LegendColor& color);
    void draw() override;
    void SetColor(const LegendColor& color);
};

// Legend item containing checkbox, color box, and title
struct LegendEntry {
    Fl_Check_Button* checkbox;
    ColorBox* colorBox;
    int functionIndex;
    
    LegendEntry() : checkbox(nullptr), colorBox(nullptr), functionIndex(0) {}
};

// Callback type for visibility changes
using VisibilityChangedCallback = std::function<void(int functionIndex, bool visible)>;

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
    void AddItem(const std::string& title, const LegendColor& color, int functionIndex, bool visible = true);
    
    // Clear all legend items
    void Clear();
    
    // Set callback for visibility changes
    void SetVisibilityCallback(VisibilityChangedCallback callback);
    
    // Get/Set visibility for a function
    bool IsVisible(int functionIndex) const;
    void SetVisible(int functionIndex, bool visible);
    
    // Get title for a function
    std::string GetTitle(int functionIndex) const;
    
    // Set title for a function
    void SetTitle(int functionIndex, const std::string& title);
    
    // Get number of items
    size_t GetItemCount() const { return entries_.size(); }
    
    void draw() override;
};

#endif // LEGEND_WIDGET_H
