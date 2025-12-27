#ifndef VECTOR_FIELD_WIDGET_H
#define VECTOR_FIELD_WIDGET_H

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include "MMLData.h"
#include <memory>
#include <cmath>
#include <functional>

class VectorFieldWidget : public Fl_Widget {
private:
    std::unique_ptr<VectorField2D> vectorField_;
    CoordSystemParams coordParams_;
    
    // Display settings
    double magnitudeScale_;      // User slider (0.1 - 5.0), multiplies optimal scale
    double optimalScale_;        // Calculated automatically
    int arrowSize_;              // Arrow head size in pixels (2-20)
    bool colorByMagnitude_;      // Use magnitude gradient coloring
    bool normalizeVectors_;      // Show direction only (unit length)
    bool preserveAspectRatio_;   // Maintain 1:1 X/Y scaling
    int arrowColorIndex_;        // 0=Black, 1=Blue, 2=Red, 3=Green, 4=Orange, 5=Purple
    
    void InitializeCoordParams();
    void DrawCoordinateSystem();
    void DrawAxes();
    void DrawGrid();
    void DrawAxisLabels();
    void DrawVector(const VectorRepr& vec);
    void DrawArrowHead(int x1, int y1, int x2, int y2, Fl_Color color);
    std::string FormatNumber(double value) const;
    
    // Get color based on magnitude (Blue→Cyan→Green→Yellow→Red)
    Fl_Color GetMagnitudeColor(double magnitude) const;
    
    // Get solid arrow color by index
    Fl_Color GetArrowColor() const;
    
public:
    VectorFieldWidget(int X, int Y, int W, int H, const char* L = nullptr);
    
    void draw() override;
    void resize(int X, int Y, int W, int H) override;
    
    void SetVectorField(std::unique_ptr<VectorField2D> field);
    void ClearVectorField();
    
    const VectorField2D* GetVectorField() const { return vectorField_.get(); }
    
    // Display settings setters
    void SetMagnitudeScale(double scale) { 
        magnitudeScale_ = std::max(0.1, std::min(5.0, scale)); 
        redraw(); 
    }
    void SetArrowSize(int size) { 
        arrowSize_ = std::max(2, std::min(20, size)); 
        redraw(); 
    }
    void SetColorByMagnitude(bool enabled) { 
        colorByMagnitude_ = enabled; 
        redraw(); 
    }
    void SetNormalizeVectors(bool enabled) { 
        normalizeVectors_ = enabled; 
        redraw(); 
    }
    void SetPreserveAspectRatio(bool enabled) { 
        preserveAspectRatio_ = enabled; 
        InitializeCoordParams();
        redraw(); 
    }
    void SetArrowColorIndex(int index) { 
        arrowColorIndex_ = std::max(0, std::min(5, index)); 
        redraw(); 
    }
    
    // Getters for UI synchronization
    double GetMagnitudeScale() const { return magnitudeScale_; }
    int GetArrowSize() const { return arrowSize_; }
    bool GetColorByMagnitude() const { return colorByMagnitude_; }
    bool GetNormalizeVectors() const { return normalizeVectors_; }
    bool GetPreserveAspectRatio() const { return preserveAspectRatio_; }
    int GetArrowColorIndex() const { return arrowColorIndex_; }
    
    // Helper to convert world coordinates to screen coordinates
    void WorldToScreen(double x, double y, int& screenX, int& screenY) const;
};

#endif // VECTOR_FIELD_WIDGET_H
