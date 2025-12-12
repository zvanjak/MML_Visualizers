#pragma once

#include <vector>
#include <string>

struct ScalarFunction2DData
{
    std::string title;
    
    // Domain parameters
    double xMin, xMax;
    int numPointsX;
    double yMin, yMax;
    int numPointsY;
    
    // Values matrix: values[i * numPointsY + j] = f(x_i, y_j)
    std::vector<double> values;
    
    // Bounds
    double minValue, maxValue;
    
    ScalarFunction2DData() 
        : xMin(0), xMax(0), numPointsX(0)
        , yMin(0), yMax(0), numPointsY(0)
        , minValue(1e10), maxValue(-1e10)
    {}
    
    double getValue(int i, int j) const {
        if (i < 0 || i >= numPointsX || j < 0 || j >= numPointsY) return 0;
        return values[i * numPointsY + j];
    }
    
    void calculateBounds() {
        if (values.empty()) return;
        
        minValue = maxValue = values[0];
        for (double val : values) {
            if (val < minValue) minValue = val;
            if (val > maxValue) maxValue = val;
        }
    }
    
    // Get interpolated X coordinate for grid index i
    double getX(int i) const {
        if (numPointsX <= 1) return xMin;
        return xMin + i * (xMax - xMin) / (numPointsX - 1);
    }
    
    // Get interpolated Y coordinate for grid index j
    double getY(int j) const {
        if (numPointsY <= 1) return yMin;
        return yMin + j * (yMax - yMin) / (numPointsY - 1);
    }
};
