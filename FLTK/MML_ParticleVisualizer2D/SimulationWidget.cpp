#define NOMINMAX
#include "SimulationWidget.h"
#include <algorithm>

SimulationWidget::SimulationWidget(int X, int Y, int W, int H, const char* L)
    : Fl_Widget(X, Y, W, H, L), currentStep_(0) {
}

void SimulationWidget::SetSimulationData(std::unique_ptr<ParticleSimulationData> data) {
    simData_ = std::move(data);
    currentStep_ = 0;
    InitializeCoordParams();
}

void SimulationWidget::SetCurrentStep(int step) {
    if (simData_ && step >= 0 && step < simData_->GetNumSteps()) {
        currentStep_ = step;
        redraw();
    }
}

void SimulationWidget::InitializeCoordParams() {
    if (!simData_) {
        coordParams_.xMin = 0;
        coordParams_.xMax = 100;
        coordParams_.yMin = 0;
        coordParams_.yMax = 100;
    } else {
        simData_->GetBounds(coordParams_.xMin, coordParams_.xMax, 
                            coordParams_.yMin, coordParams_.yMax);
        
        // Add 5% padding
        double xPadding = (coordParams_.xMax - coordParams_.xMin) * 0.05;
        double yPadding = (coordParams_.yMax - coordParams_.yMin) * 0.05;
        coordParams_.xMin -= xPadding;
        coordParams_.xMax += xPadding;
        coordParams_.yMin -= yPadding;
        coordParams_.yMax += yPadding;
    }
    
    // Calculate scaling to preserve aspect ratio
    coordParams_.windowWidth = w();
    coordParams_.windowHeight = h();
    
    double xRange = coordParams_.xMax - coordParams_.xMin;
    double yRange = coordParams_.yMax - coordParams_.yMin;
    
    if (xRange <= 0) xRange = 1;
    if (yRange <= 0) yRange = 1;
    
    // Use the same scale for both axes to preserve aspect ratio
    double scaleX = (coordParams_.windowWidth * 0.95) / xRange;
    double scaleY = (coordParams_.windowHeight * 0.95) / yRange;
    coordParams_.scaleX = coordParams_.scaleY = std::min(scaleX, scaleY);
    
    double midPointX = (coordParams_.xMin + coordParams_.xMax) / 2;
    double midPointY = (coordParams_.yMin + coordParams_.yMax) / 2;
    
    coordParams_.centerX = coordParams_.windowWidth / 2 - midPointX * coordParams_.scaleX;
    coordParams_.centerY = coordParams_.windowHeight / 2 + midPointY * coordParams_.scaleY;
}

void SimulationWidget::WorldToScreen(double worldX, double worldY, int& screenX, int& screenY) const {
    screenX = static_cast<int>(coordParams_.centerX + worldX * coordParams_.scaleX);
    screenY = static_cast<int>(coordParams_.centerY - worldY * coordParams_.scaleY);
    
    // Add widget position offset
    screenX += x();
    screenY += y();
}

void SimulationWidget::DrawBorder() {
    // Draw simulation boundary rectangle
    int x1, y1, x2, y2;
    WorldToScreen(coordParams_.xMin, coordParams_.yMin, x1, y2);
    WorldToScreen(coordParams_.xMax, coordParams_.yMax, x2, y1);
    
    fl_color(FL_BLACK);
    fl_line_style(FL_SOLID, 2);
    fl_rect(x1, y1, x2 - x1, y2 - y1);
    fl_line_style(0);
}

void SimulationWidget::draw() {
    // Draw background
    fl_color(FL_WHITE);
    fl_rectf(x(), y(), w(), h());
    
    // Draw border
    fl_color(FL_BLACK);
    fl_rect(x(), y(), w(), h());
    
    if (!simData_ || currentStep_ >= simData_->GetNumSteps()) {
        return;
    }
    
    DrawBorder();
    
    // Draw all balls at current step
    for (int i = 0; i < simData_->GetNumBalls(); ++i) {
        const Ball& ball = simData_->GetBall(i);
        
        try {
            Vector2D pos = ball.GetPosition(currentStep_);
            int screenX, screenY;
            WorldToScreen(pos.x, pos.y, screenX, screenY);
            
            // Convert radius from world to screen coordinates
            int screenRadius = static_cast<int>(ball.GetRadius() * coordParams_.scaleX);
            
            // Draw ball
            Color color = ball.GetColor();
            fl_color(color.r, color.g, color.b);
            fl_pie(screenX - screenRadius, screenY - screenRadius, 
                   screenRadius * 2, screenRadius * 2, 0, 360);
            
            // Draw ball outline
            fl_color(FL_BLACK);
            fl_arc(screenX - screenRadius, screenY - screenRadius, 
                   screenRadius * 2, screenRadius * 2, 0, 360);
        } catch (const std::exception& e) {
            // Skip this ball if position data is invalid
        }
    }
}
