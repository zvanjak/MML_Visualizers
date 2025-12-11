#define NOMINMAX
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Box.H>
#include <FL/fl_ask.H>
#include "SimulationWidget.h"
#include "LegendWidget.h"
#include "MMLFileParser.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <iomanip>

class MainWindow {
private:
    Fl_Window* window_;
    SimulationWidget* simWidget_;
    LegendWidget* legendWidget_;
    Fl_Button* loadButton_;
    Fl_Button* playButton_;
    Fl_Button* pauseButton_;
    Fl_Button* resetButton_;
    Fl_Value_Slider* stepSlider_;
    Fl_Box* stepLabel_;
    Fl_Box* timeLabel_;
    
    bool isPlaying_;
    double playbackSpeed_;  // Steps per second
    
    void LoadFile(const char* filename);
    void UpdateLegend();
    void UpdateLabels();
    void StartPlayback();
    void StopPlayback();
    void ResetSimulation();
    
    static void LoadButtonCallback(Fl_Widget* widget, void* data);
    static void PlayButtonCallback(Fl_Widget* widget, void* data);
    static void PauseButtonCallback(Fl_Widget* widget, void* data);
    static void ResetButtonCallback(Fl_Widget* widget, void* data);
    static void StepSliderCallback(Fl_Widget* widget, void* data);
    static void TimerCallback(void* data);
    
public:
    MainWindow(int argc, char** argv);
    ~MainWindow();
    
    void Show();
    int Run();
};

MainWindow::MainWindow(int argc, char** argv) : isPlaying_(false), playbackSpeed_(10.0) {
    // Create main window
    window_ = new Fl_Window(1200, 750, "MML Particle Visualizer 2D (FLTK)");
    
    // Create simulation widget
    simWidget_ = new SimulationWidget(10, 10, 900, 600);
    
    // Create legend widget
    legendWidget_ = new LegendWidget(920, 10, 270, 600, "Particles");
    
    // Create control buttons
    loadButton_ = new Fl_Button(10, 620, 100, 30, "Load File");
    loadButton_->callback(LoadButtonCallback, this);
    
    playButton_ = new Fl_Button(120, 620, 80, 30, "Play");
    playButton_->callback(PlayButtonCallback, this);
    
    pauseButton_ = new Fl_Button(210, 620, 80, 30, "Pause");
    pauseButton_->callback(PauseButtonCallback, this);
    
    resetButton_ = new Fl_Button(300, 620, 80, 30, "Reset");
    resetButton_->callback(ResetButtonCallback, this);
    
    // Create step slider
    stepSlider_ = new Fl_Value_Slider(10, 660, 900, 30, "Step:");
    stepSlider_->type(FL_HOR_NICE_SLIDER);
    stepSlider_->bounds(0, 100);
    stepSlider_->step(1);
    stepSlider_->value(0);
    stepSlider_->callback(StepSliderCallback, this);
    stepSlider_->align(FL_ALIGN_LEFT);
    
    // Create status labels
    stepLabel_ = new Fl_Box(10, 700, 200, 30, "Step: 0 / 0");
    stepLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    
    timeLabel_ = new Fl_Box(220, 700, 200, 30, "Time: 0.00");
    timeLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    
    window_->end();
    window_->resizable(simWidget_);
    
    // Load file from command line argument
    if (argc > 1) {
        try {
            LoadFile(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Error loading file " << argv[1] << ": " << e.what() << std::endl;
            fl_alert("Error loading file %s:\n%s", argv[1], e.what());
        }
    }
}

MainWindow::~MainWindow() {
    delete window_;
}

void MainWindow::LoadFile(const char* filename) {
    try {
        auto simData = MMLFileParser::ParseFile(filename);
        
        if (simData) {
            simWidget_->SetSimulationData(std::move(simData));
            
            // Update slider range
            int maxSteps = simWidget_->GetMaxSteps();
            stepSlider_->bounds(0, maxSteps > 0 ? maxSteps - 1 : 0);
            stepSlider_->value(0);
            
            UpdateLegend();
            UpdateLabels();
            simWidget_->redraw();
        }
    } catch (const std::exception& e) {
        throw;
    }
}

void MainWindow::UpdateLegend() {
    legendWidget_->Clear();
    
    const ParticleSimulationData* simData = simWidget_->GetSimData();
    if (!simData) return;
    
    int maxDisplay = std::min(simData->GetNumBalls(), 25);  // Limit display
    for (int i = 0; i < maxDisplay; ++i) {
        const Ball& ball = simData->GetBall(i);
        legendWidget_->AddItem(ball.GetName(), ball.GetColor());
    }
    
    legendWidget_->redraw();
}

void MainWindow::UpdateLabels() {
    const ParticleSimulationData* simData = simWidget_->GetSimData();
    if (!simData) {
        stepLabel_->label("Step: 0 / 0");
        timeLabel_->label("Time: 0.00");
        return;
    }
    
    int currentStep = simWidget_->GetCurrentStep();
    int maxSteps = simWidget_->GetMaxSteps();
    double time = simData->GetTimeStep(currentStep);
    
    std::ostringstream stepStream;
    stepStream << "Step: " << currentStep << " / " << maxSteps;
    stepLabel_->copy_label(stepStream.str().c_str());
    
    std::ostringstream timeStream;
    timeStream << "Time: " << std::fixed << std::setprecision(2) << time;
    timeLabel_->copy_label(timeStream.str().c_str());
    
    stepLabel_->redraw();
    timeLabel_->redraw();
}

void MainWindow::StartPlayback() {
    isPlaying_ = true;
    Fl::add_timeout(1.0 / playbackSpeed_, TimerCallback, this);
}

void MainWindow::StopPlayback() {
    isPlaying_ = false;
    Fl::remove_timeout(TimerCallback, this);
}

void MainWindow::ResetSimulation() {
    StopPlayback();
    simWidget_->SetCurrentStep(0);
    stepSlider_->value(0);
    UpdateLabels();
}

void MainWindow::LoadButtonCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    
    Fl_File_Chooser chooser(".", "MML Files (*.txt)", Fl_File_Chooser::SINGLE, "Select MML File");
    chooser.show();
    
    while (chooser.shown()) {
        Fl::wait();
    }
    
    if (chooser.value() != nullptr) {
        try {
            mainWin->LoadFile(chooser.value());
        } catch (const std::exception& e) {
            fl_alert("Error loading file:\n%s", e.what());
        }
    }
}

void MainWindow::PlayButtonCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    if (!mainWin->isPlaying_) {
        mainWin->StartPlayback();
    }
}

void MainWindow::PauseButtonCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    mainWin->StopPlayback();
}

void MainWindow::ResetButtonCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    mainWin->ResetSimulation();
}

void MainWindow::StepSliderCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Value_Slider* slider = static_cast<Fl_Value_Slider*>(widget);
    
    int step = static_cast<int>(slider->value());
    mainWin->simWidget_->SetCurrentStep(step);
    mainWin->UpdateLabels();
}

void MainWindow::TimerCallback(void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    
    if (!mainWin->isPlaying_) return;
    
    int currentStep = mainWin->simWidget_->GetCurrentStep();
    int maxSteps = mainWin->simWidget_->GetMaxSteps();
    
    if (currentStep < maxSteps - 1) {
        currentStep++;
        mainWin->simWidget_->SetCurrentStep(currentStep);
        mainWin->stepSlider_->value(currentStep);
        mainWin->UpdateLabels();
        
        // Schedule next update
        Fl::repeat_timeout(1.0 / mainWin->playbackSpeed_, TimerCallback, data);
    } else {
        mainWin->StopPlayback();
    }
}

void MainWindow::Show() {
    window_->show();
}

int MainWindow::Run() {
    return Fl::run();
}

int main(int argc, char** argv) {
    MainWindow mainWindow(argc, argv);
    mainWindow.Show();
    return mainWindow.Run();
}
