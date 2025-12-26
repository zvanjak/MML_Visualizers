#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include "MMLData.h"
#include "AxisTickCalculator.h"
#include <vector>
#include <memory>
#include <cmath>
#include <functional>

// Color definitions for drawing
struct Color {
    unsigned char r, g, b;
    Color(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0) 
        : r(red), g(green), b(blue) {}
};

// Coordinate system styling
struct CoordSystemStyle {
    Color axisColor{0, 0, 0};         // Black for axes
    Color gridColor{220, 220, 220};   // Light gray for grid
    Color tickColor{0, 0, 0};         // Black for ticks
    Color labelColor{0, 0, 0};        // Black for labels
    
    double axisThickness = 1.5;
    double gridThickness = 0.5;
    double tickLength = 5;
    int labelFontSize = 11;
    
    bool showGrid = true;
    bool showMinorGrid = false;
    bool showAxisLabels = true;
    
    double labelOffsetX = 5;
    double labelOffsetY = 5;
};

class GraphWidget : public Fl_Widget {
public:
    // Callback type for redraw requests
    using RedrawCallback = std::function<void()>;

private:
    std::vector<std::unique_ptr<LoadedFunction>> functions_;
    CoordSystemParams coordParams_;
    CoordSystemStyle style_;
    
    // Tick information calculated from data
    AxisTickInfo xTickInfo_;
    AxisTickInfo yTickInfo_;
    
    // Data bounds (original, before nice rounding)
    double dataXMin_, dataXMax_, dataYMin_, dataYMax_;
    
    // Color palette for multiple functions
    static const std::vector<Color> colors_;
    
    // Callback for requesting main window redraw
    RedrawCallback redrawCallback_;
    
    void InitializeCoordParams();
    void CalculateDataBounds();
    void DrawCoordinateSystem();
    void DrawGrid();
    void DrawAxes();
    void DrawAxisTicks();
    void DrawAxisLabels();
    
public:
    GraphWidget(int X, int Y, int W, int H, const char* L = nullptr);
    
    void draw() override;
    
    void AddFunction(std::unique_ptr<LoadedFunction> func);
    void ClearFunctions();
    
    const CoordSystemParams& GetCoordParams() const { return coordParams_; }
    CoordSystemParams& GetCoordParams() { return coordParams_; }
    const std::vector<std::unique_ptr<LoadedFunction>>& GetFunctions() const { return functions_; }
    std::vector<std::unique_ptr<LoadedFunction>>& GetFunctions() { return functions_; }
    
    // Get tick info for external display
    const AxisTickInfo& GetXTickInfo() const { return xTickInfo_; }
    const AxisTickInfo& GetYTickInfo() const { return yTickInfo_; }
    
    // Data bounds getters
    double GetDataXMin() const { return dataXMin_; }
    double GetDataXMax() const { return dataXMax_; }
    double GetDataYMin() const { return dataYMin_; }
    double GetDataYMax() const { return dataYMax_; }
    int GetNumPoints() const { return coordParams_.numPoints; }
    
    // Preserve aspect ratio
    void SetPreserveAspectRatio(bool preserve) { 
        coordParams_.preserveAspectRatio = preserve; 
    }
    bool GetPreserveAspectRatio() const { 
        return coordParams_.preserveAspectRatio; 
    }
    
    // Style access
    CoordSystemStyle& GetStyle() { return style_; }
    const CoordSystemStyle& GetStyle() const { return style_; }
    
    // Set redraw callback
    void SetRedrawCallback(RedrawCallback cb) { redrawCallback_ = cb; }
    
    // Public method to get color for a function
    static Color GetColor(int index) {
        return colors_[index % colors_.size()];
    }
    
    // Helper to convert world coordinates to screen coordinates
    void WorldToScreen(double x, double y, int& screenX, int& screenY) const;
    
    // Helper to convert screen coordinates to world coordinates
    void ScreenToWorld(int screenX, int screenY, double& x, double& y) const;
    
    // Recalculate and redraw
    void RecalculateAndRedraw();
};

#endif // GRAPH_WIDGET_H
