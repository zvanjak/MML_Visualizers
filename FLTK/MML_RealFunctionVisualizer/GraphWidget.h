#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include "MMLData.h"
#include <vector>
#include <memory>
#include <cmath>

// Color definitions for drawing
struct Color {
    unsigned char r, g, b;
    Color(unsigned char red, unsigned char green, unsigned char blue) 
        : r(red), g(green), b(blue) {}
};

class GraphWidget : public Fl_Widget {
private:
    std::vector<std::unique_ptr<LoadedFunction>> functions_;
    CoordSystemParams coordParams_;
    
    // Color palette for multiple functions
    static const std::vector<Color> colors_;
    
    void InitializeCoordParams();
    void DrawCoordinateSystem();
    void DrawAxes();
    void DrawGrid();
    void DrawAxisLabels();
    std::string FormatNumber(double value) const;
    
public:
    GraphWidget(int X, int Y, int W, int H, const char* L = nullptr);
    
    void draw() override;
    
    void AddFunction(std::unique_ptr<LoadedFunction> func);
    void ClearFunctions();
    
    const CoordSystemParams& GetCoordParams() const { return coordParams_; }
    const std::vector<std::unique_ptr<LoadedFunction>>& GetFunctions() const { return functions_; }
    
    // Public method to get color for a function
    static Color GetColor(int index) {
        return colors_[index % colors_.size()];
    }
    
    // Helper to convert world coordinates to screen coordinates
    void WorldToScreen(double x, double y, int& screenX, int& screenY) const;
};

#endif // GRAPH_WIDGET_H
