#ifndef VECTOR_FIELD_WIDGET_H
#define VECTOR_FIELD_WIDGET_H

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include "MMLData.h"
#include <memory>
#include <cmath>

class VectorFieldWidget : public Fl_Widget {
private:
    std::unique_ptr<VectorField2D> vectorField_;
    CoordSystemParams coordParams_;
    double vectorScaleFactor_;  // Scale factor for drawing vectors
    
    void InitializeCoordParams();
    void DrawCoordinateSystem();
    void DrawAxes();
    void DrawGrid();
    void DrawAxisLabels();
    void DrawVector(const VectorRepr& vec);
    void DrawArrowHead(int x1, int y1, int x2, int y2);
    std::string FormatNumber(double value) const;
    
public:
    VectorFieldWidget(int X, int Y, int W, int H, const char* L = nullptr);
    
    void draw() override;
    
    void SetVectorField(std::unique_ptr<VectorField2D> field);
    void ClearVectorField();
    
    const VectorField2D* GetVectorField() const { return vectorField_.get(); }
    
    // Helper to convert world coordinates to screen coordinates
    void WorldToScreen(double x, double y, int& screenX, int& screenY) const;
};

#endif // VECTOR_FIELD_WIDGET_H
