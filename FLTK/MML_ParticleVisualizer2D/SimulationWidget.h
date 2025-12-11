#ifndef SIMULATION_WIDGET_H
#define SIMULATION_WIDGET_H

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include "MMLData.h"
#include <memory>
#include <cmath>

class SimulationWidget : public Fl_Widget {
private:
    std::unique_ptr<ParticleSimulationData> simData_;
    CoordSystemParams coordParams_;
    int currentStep_;
    
    void InitializeCoordParams();
    void DrawBorder();
    
public:
    SimulationWidget(int X, int Y, int W, int H, const char* L = nullptr);
    
    void draw() override;
    
    void SetSimulationData(std::unique_ptr<ParticleSimulationData> data);
    void SetCurrentStep(int step);
    int GetCurrentStep() const { return currentStep_; }
    int GetMaxSteps() const { return simData_ ? simData_->GetNumSteps() : 0; }
    
    const ParticleSimulationData* GetSimData() const { return simData_.get(); }
    
    // Helper to convert world coordinates to screen coordinates
    void WorldToScreen(double x, double y, int& screenX, int& screenY) const;
};

#endif // SIMULATION_WIDGET_H
