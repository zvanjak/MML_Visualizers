/**
 * MML Parametric Curve 2D Visualizer - FLTK Version
 * 
 * A cross-platform visualizer for 2D parametric curves, matching the
 * functionality of the WPF version.
 * 
 * Features:
 * - Loads PARAMETRIC_CURVE_CARTESIAN_2D data files
 * - Nice numbers algorithm for axis tick calculation
 * - Legend with visibility toggles (checkboxes)
 * - Sidebar panel with controls
 * - Preserve aspect ratio option (critical for parametric curves)
 * - Animation system with markers moving along curves
 * - Proper coordinate system rendering
 */

#define NOMINMAX
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_ask.H>
#include "GraphWidget.h"
#include "LegendWidget.h"
#include "MMLFileParser.h"
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>

// Window dimensions matching WPF spec
static const int WINDOW_WIDTH = 1250;
static const int WINDOW_HEIGHT = 850;
static const int SIDEBAR_WIDTH = 230;
static const int MARGIN = 5;
static const int BUTTON_HEIGHT = 30;

class MainWindow {
private:
    // Main window
    Fl_Double_Window* window_;
    
    // Title display at top
    Fl_Box* titleDisplay_;
    
    // Main graph area
    GraphWidget* graphWidget_;
    
    // Sidebar components
    Fl_Group* sidebarGroup_;
    
    // Title section
    Fl_Box* titleLabel_;
    Fl_Input* titleInput_;
    
    // Legend section
    Fl_Box* legendLabel_;
    Fl_Scroll* legendScroll_;
    LegendWidget* legendWidget_;
    
    // Animation section
    Fl_Box* animationLabel_;
    Fl_Button* startButton_;
    Fl_Button* pauseButton_;
    Fl_Button* resetButton_;
    Fl_Box* speedLabel_;
    Fl_Value_Input* speedInput_;
    Fl_Box* frameLabel_;
    
    // Settings section
    Fl_Box* settingsLabel_;
    Fl_Check_Button* gridCheckbox_;
    Fl_Check_Button* aspectRatioCheckbox_;
    Fl_Check_Button* labelsCheckbox_;
    
    // Bounds display
    Fl_Box* boundsLabel_;
    Fl_Box* xBoundsDisplay_;
    Fl_Box* yBoundsDisplay_;
    Fl_Box* tBoundsDisplay_;
    Fl_Box* pointsDisplay_;
    
    // Buttons section
    Fl_Button* loadButton_;
    Fl_Button* clearButton_;
    
    // State
    int fileIndex_ = 0;
    std::string graphTitle_;
    bool isPaused_ = false;
    
    // Methods
    void LoadFile(const char* filename);
    void UpdateLegend();
    void UpdateBoundsDisplay();
    void UpdateFrameDisplay();
    void OnVisibilityChanged(int curveIndex, bool visible);
    void OnTitleChanged();
    
    // Callbacks
    static void LoadButtonCallback(Fl_Widget* widget, void* data);
    static void ClearButtonCallback(Fl_Widget* widget, void* data);
    static void TitleInputCallback(Fl_Widget* widget, void* data);
    static void GridCheckboxCallback(Fl_Widget* widget, void* data);
    static void AspectRatioCheckboxCallback(Fl_Widget* widget, void* data);
    static void LabelsCheckboxCallback(Fl_Widget* widget, void* data);
    static void StartButtonCallback(Fl_Widget* widget, void* data);
    static void PauseButtonCallback(Fl_Widget* widget, void* data);
    static void ResetButtonCallback(Fl_Widget* widget, void* data);
    static void SpeedInputCallback(Fl_Widget* widget, void* data);
    
public:
    MainWindow(int argc, char** argv);
    ~MainWindow();
    
    void Show();
    int Run();
};

MainWindow::MainWindow(int argc, char** argv) : fileIndex_(0), graphTitle_("Parametric Curve 2D Visualizer") {
    // Create main window
    window_ = new Fl_Double_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "MML Parametric Curve 2D Visualizer (FLTK)");
    window_->color(fl_rgb_color(245, 245, 245));
    
    // Calculate layout dimensions
    int titleBarHeight = 40;
    int graphWidth = WINDOW_WIDTH - SIDEBAR_WIDTH - 3 * MARGIN;
    int graphHeight = WINDOW_HEIGHT - titleBarHeight - 2 * MARGIN;
    int graphX = MARGIN;
    int graphY = titleBarHeight + MARGIN;
    
    int sidebarX = graphX + graphWidth + MARGIN;
    
    // ==================== TITLE BAR ====================
    titleDisplay_ = new Fl_Box(0, 0, WINDOW_WIDTH - SIDEBAR_WIDTH, titleBarHeight);
    titleDisplay_->label(graphTitle_.c_str());
    titleDisplay_->labelfont(FL_HELVETICA_BOLD);
    titleDisplay_->labelsize(16);
    titleDisplay_->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    
    // ==================== GRAPH AREA ====================
    graphWidget_ = new GraphWidget(graphX, graphY, graphWidth, graphHeight);
    graphWidget_->SetPreserveAspectRatio(true);  // Default true for parametric curves
    graphWidget_->SetShowGrid(true);
    graphWidget_->SetShowAxisLabels(true);
    
    // Set animation frame callback
    graphWidget_->SetAnimationFrameCallback([this]() {
        UpdateFrameDisplay();
    });
    
    // ==================== SIDEBAR ====================
    sidebarGroup_ = new Fl_Group(sidebarX, 0, SIDEBAR_WIDTH, WINDOW_HEIGHT);
    sidebarGroup_->color(fl_rgb_color(240, 240, 240));
    sidebarGroup_->box(FL_FLAT_BOX);
    
    int yPos = MARGIN;
    int widgetWidth = SIDEBAR_WIDTH - 2 * MARGIN;
    int sectionSpacing = 12;
    
    // ---------- TITLE SECTION ----------
    titleLabel_ = new Fl_Box(sidebarX + MARGIN, yPos, widgetWidth, 20, "Graph Title:");
    titleLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    titleLabel_->labelfont(FL_HELVETICA_BOLD);
    titleLabel_->labelsize(12);
    yPos += 22;
    
    titleInput_ = new Fl_Input(sidebarX + MARGIN, yPos, widgetWidth, 28);
    titleInput_->value(graphTitle_.c_str());
    titleInput_->callback(TitleInputCallback, this);
    titleInput_->when(FL_WHEN_ENTER_KEY | FL_WHEN_RELEASE);
    yPos += 35 + sectionSpacing;
    
    // ---------- LEGEND SECTION ----------
    legendLabel_ = new Fl_Box(sidebarX + MARGIN, yPos, widgetWidth, 20, "Legend:");
    legendLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    legendLabel_->labelfont(FL_HELVETICA_BOLD);
    legendLabel_->labelsize(12);
    yPos += 22;
    
    int legendHeight = 150;
    legendScroll_ = new Fl_Scroll(sidebarX + MARGIN, yPos, widgetWidth, legendHeight);
    legendScroll_->box(FL_DOWN_FRAME);
    legendScroll_->color(FL_WHITE);
    
    legendWidget_ = new LegendWidget(sidebarX + MARGIN, yPos, widgetWidth - 20, legendHeight);
    legendWidget_->SetVisibilityCallback([this](int idx, bool vis) {
        OnVisibilityChanged(idx, vis);
    });
    
    legendScroll_->end();
    yPos += legendHeight + sectionSpacing;
    
    // ---------- ANIMATION SECTION ----------
    animationLabel_ = new Fl_Box(sidebarX + MARGIN, yPos, widgetWidth, 20, "Animation:");
    animationLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    animationLabel_->labelfont(FL_HELVETICA_BOLD);
    animationLabel_->labelsize(12);
    yPos += 22;
    
    // Animation buttons
    int buttonWidth = (widgetWidth - 10) / 3;
    startButton_ = new Fl_Button(sidebarX + MARGIN, yPos, buttonWidth, BUTTON_HEIGHT, "Start");
    startButton_->callback(StartButtonCallback, this);
    startButton_->color(fl_rgb_color(70, 180, 70));
    startButton_->labelcolor(FL_WHITE);
    
    pauseButton_ = new Fl_Button(sidebarX + MARGIN + buttonWidth + 5, yPos, buttonWidth, BUTTON_HEIGHT, "Pause");
    pauseButton_->callback(PauseButtonCallback, this);
    pauseButton_->color(fl_rgb_color(180, 140, 70));
    pauseButton_->labelcolor(FL_WHITE);
    
    resetButton_ = new Fl_Button(sidebarX + MARGIN + 2*buttonWidth + 10, yPos, buttonWidth, BUTTON_HEIGHT, "Reset");
    resetButton_->callback(ResetButtonCallback, this);
    resetButton_->color(fl_rgb_color(180, 70, 70));
    resetButton_->labelcolor(FL_WHITE);
    yPos += BUTTON_HEIGHT + 5;
    
    // Speed control
    speedLabel_ = new Fl_Box(sidebarX + MARGIN, yPos, 100, 25, "Speed (pts/s):");
    speedLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    speedLabel_->labelsize(11);
    
    speedInput_ = new Fl_Value_Input(sidebarX + MARGIN + 100, yPos, 60, 25);
    speedInput_->value(10);
    speedInput_->minimum(0.1);
    speedInput_->maximum(1000);
    speedInput_->step(1);
    speedInput_->callback(SpeedInputCallback, this);
    yPos += 28;
    
    // Frame display
    frameLabel_ = new Fl_Box(sidebarX + MARGIN, yPos, widgetWidth, 20, "Frame: 0 / 0");
    frameLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    frameLabel_->labelsize(10);
    yPos += 25 + sectionSpacing;
    
    // ---------- SETTINGS SECTION ----------
    settingsLabel_ = new Fl_Box(sidebarX + MARGIN, yPos, widgetWidth, 20, "Graph Settings:");
    settingsLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    settingsLabel_->labelfont(FL_HELVETICA_BOLD);
    settingsLabel_->labelsize(12);
    yPos += 22;
    
    gridCheckbox_ = new Fl_Check_Button(sidebarX + MARGIN, yPos, widgetWidth, 22, "Show Grid");
    gridCheckbox_->value(1);
    gridCheckbox_->callback(GridCheckboxCallback, this);
    yPos += 24;
    
    labelsCheckbox_ = new Fl_Check_Button(sidebarX + MARGIN, yPos, widgetWidth, 22, "Show Axis Labels");
    labelsCheckbox_->value(1);
    labelsCheckbox_->callback(LabelsCheckboxCallback, this);
    yPos += 24;
    
    aspectRatioCheckbox_ = new Fl_Check_Button(sidebarX + MARGIN, yPos, widgetWidth, 22, "Preserve Aspect Ratio");
    aspectRatioCheckbox_->value(1);  // Default true for parametric curves
    aspectRatioCheckbox_->callback(AspectRatioCheckboxCallback, this);
    yPos += 30 + sectionSpacing;
    
    // ---------- BOUNDS DISPLAY SECTION ----------
    boundsLabel_ = new Fl_Box(sidebarX + MARGIN, yPos, widgetWidth, 20, "Curve Info:");
    boundsLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    boundsLabel_->labelfont(FL_HELVETICA_BOLD);
    boundsLabel_->labelsize(12);
    yPos += 22;
    
    xBoundsDisplay_ = new Fl_Box(sidebarX + MARGIN, yPos, widgetWidth, 18, "X: - to -");
    xBoundsDisplay_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    xBoundsDisplay_->labelsize(10);
    yPos += 18;
    
    yBoundsDisplay_ = new Fl_Box(sidebarX + MARGIN, yPos, widgetWidth, 18, "Y: - to -");
    yBoundsDisplay_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    yBoundsDisplay_->labelsize(10);
    yPos += 18;
    
    tBoundsDisplay_ = new Fl_Box(sidebarX + MARGIN, yPos, widgetWidth, 18, "T: - to -");
    tBoundsDisplay_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    tBoundsDisplay_->labelsize(10);
    yPos += 18;
    
    pointsDisplay_ = new Fl_Box(sidebarX + MARGIN, yPos, widgetWidth, 18, "Points: 0");
    pointsDisplay_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    pointsDisplay_->labelsize(10);
    yPos += 25 + sectionSpacing;
    
    // ---------- BUTTONS SECTION ----------
    loadButton_ = new Fl_Button(sidebarX + MARGIN, yPos, widgetWidth, BUTTON_HEIGHT, "Load File...");
    loadButton_->callback(LoadButtonCallback, this);
    loadButton_->color(fl_rgb_color(70, 130, 180));
    loadButton_->labelcolor(FL_WHITE);
    loadButton_->labelfont(FL_HELVETICA_BOLD);
    yPos += BUTTON_HEIGHT + 10;
    
    clearButton_ = new Fl_Button(sidebarX + MARGIN, yPos, widgetWidth, BUTTON_HEIGHT, "Clear All");
    clearButton_->callback(ClearButtonCallback, this);
    clearButton_->color(fl_rgb_color(180, 70, 70));
    clearButton_->labelcolor(FL_WHITE);
    
    sidebarGroup_->end();
    
    // ==================== WINDOW SETUP ====================
    window_->end();
    window_->resizable(graphWidget_);
    
    // Load files from command line arguments
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            try {
                LoadFile(argv[i]);
            } catch (const std::exception& e) {
                std::cerr << "Error loading file " << argv[i] << ": " << e.what() << std::endl;
                fl_alert("Error loading file %s:\n%s", argv[i], e.what());
            }
        }
        
        // Update title from first curve if loaded
        if (!graphWidget_->GetCurves().empty()) {
            graphTitle_ = graphWidget_->GetCurves()[0]->GetTitle();
            titleInput_->value(graphTitle_.c_str());
            titleDisplay_->copy_label(graphTitle_.c_str());
        }
    }
}

MainWindow::~MainWindow() {
    delete window_;
}

void MainWindow::LoadFile(const char* filename) {
    try {
        auto curve = MMLFileParser::ParseFile(filename, fileIndex_);
        
        if (curve) {
            graphWidget_->AddCurve(std::move(curve));
            fileIndex_++;
            
            UpdateLegend();
            UpdateBoundsDisplay();
            UpdateFrameDisplay();
            graphWidget_->redraw();
        }
    } catch (const std::exception& e) {
        throw;
    }
}

void MainWindow::UpdateLegend() {
    legendWidget_->Clear();
    
    const auto& curves = graphWidget_->GetCurves();
    
    for (size_t i = 0; i < curves.size(); ++i) {
        const auto& curve = curves[i];
        legendWidget_->AddItem(curve->GetTitle(), curve->GetColor(), 
                               static_cast<int>(i), curve->IsVisible());
    }
    
    legendWidget_->redraw();
}

void MainWindow::UpdateBoundsDisplay() {
    std::ostringstream oss;
    
    oss << std::fixed << std::setprecision(3);
    
    oss.str(""); oss.clear();
    oss << "X: " << graphWidget_->GetDataMinX() << " to " << graphWidget_->GetDataMaxX();
    xBoundsDisplay_->copy_label(oss.str().c_str());
    
    oss.str(""); oss.clear();
    oss << "Y: " << graphWidget_->GetDataMinY() << " to " << graphWidget_->GetDataMaxY();
    yBoundsDisplay_->copy_label(oss.str().c_str());
    
    oss.str(""); oss.clear();
    oss << "T: " << graphWidget_->GetDataMinT() << " to " << graphWidget_->GetDataMaxT();
    tBoundsDisplay_->copy_label(oss.str().c_str());
    
    oss.str(""); oss.clear();
    oss << "Points: " << graphWidget_->GetMaxAnimationFrames();
    pointsDisplay_->copy_label(oss.str().c_str());
}

void MainWindow::UpdateFrameDisplay() {
    std::ostringstream oss;
    oss << "Frame: " << graphWidget_->GetCurrentAnimationFrame() 
        << " / " << graphWidget_->GetMaxAnimationFrames();
    frameLabel_->copy_label(oss.str().c_str());
}

void MainWindow::OnVisibilityChanged(int curveIndex, bool visible) {
    graphWidget_->SetCurveVisible(curveIndex, visible);
    graphWidget_->redraw();
}

void MainWindow::OnTitleChanged() {
    graphTitle_ = titleInput_->value();
    titleDisplay_->copy_label(graphTitle_.c_str());
}

// Callbacks
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

void MainWindow::ClearButtonCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    mainWin->graphWidget_->ClearCurves();
    mainWin->legendWidget_->Clear();
    mainWin->fileIndex_ = 0;
    mainWin->UpdateBoundsDisplay();
    mainWin->UpdateFrameDisplay();
    mainWin->graphWidget_->redraw();
    mainWin->legendWidget_->redraw();
}

void MainWindow::TitleInputCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    mainWin->OnTitleChanged();
}

void MainWindow::GridCheckboxCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Check_Button* checkbox = static_cast<Fl_Check_Button*>(widget);
    mainWin->graphWidget_->SetShowGrid(checkbox->value() != 0);
}

void MainWindow::AspectRatioCheckboxCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Check_Button* checkbox = static_cast<Fl_Check_Button*>(widget);
    mainWin->graphWidget_->SetPreserveAspectRatio(checkbox->value() != 0);
}

void MainWindow::LabelsCheckboxCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Check_Button* checkbox = static_cast<Fl_Check_Button*>(widget);
    mainWin->graphWidget_->SetShowAxisLabels(checkbox->value() != 0);
}

void MainWindow::StartButtonCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    
    if (mainWin->graphWidget_->IsAnimationRunning()) {
        // Already running, ignore
        return;
    }
    
    if (mainWin->isPaused_) {
        mainWin->graphWidget_->ResumeAnimation();
        mainWin->isPaused_ = false;
    } else {
        mainWin->graphWidget_->StartAnimation();
    }
    
    mainWin->startButton_->label("Running");
    mainWin->pauseButton_->label("Pause");
}

void MainWindow::PauseButtonCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    
    if (mainWin->graphWidget_->IsAnimationRunning()) {
        mainWin->graphWidget_->PauseAnimation();
        mainWin->isPaused_ = true;
        mainWin->startButton_->label("Resume");
        mainWin->pauseButton_->label("Paused");
    } else if (mainWin->isPaused_) {
        mainWin->graphWidget_->ResumeAnimation();
        mainWin->isPaused_ = false;
        mainWin->startButton_->label("Running");
        mainWin->pauseButton_->label("Pause");
    }
}

void MainWindow::ResetButtonCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    mainWin->graphWidget_->StopAnimation();
    mainWin->graphWidget_->ResetAnimation();
    mainWin->isPaused_ = false;
    mainWin->startButton_->label("Start");
    mainWin->pauseButton_->label("Pause");
    mainWin->UpdateFrameDisplay();
}

void MainWindow::SpeedInputCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Value_Input* input = static_cast<Fl_Value_Input*>(widget);
    mainWin->graphWidget_->SetAnimationSpeed(input->value());
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
