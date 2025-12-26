/**
 * MML Real Function Visualizer - FLTK Version
 * 
 * A cross-platform visualizer for mathematical functions, matching the
 * functionality of the WPF version as closely as possible.
 * 
 * Features:
 * - Loads REAL_FUNCTION and MULTI_REAL_FUNCTION data files
 * - Nice numbers algorithm for axis tick calculation
 * - Legend with visibility toggles (checkboxes)
 * - Sidebar panel with controls
 * - Preserve aspect ratio option
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
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_ask.H>
#include "GraphWidget.h"
#include "LegendWidget.h"
#include "MMLFileParser.h"
#include <iostream>
#include <memory>
#include <string>

// Window dimensions matching WPF spec
static const int WINDOW_WIDTH = 1200;
static const int WINDOW_HEIGHT = 850;
static const int SIDEBAR_WIDTH = 230;
static const int TITLE_BAR_HEIGHT = 40;
static const int MARGIN = 5;
static const int BUTTON_HEIGHT = 30;

class MainWindow {
private:
    // Main window
    Fl_Double_Window* window_;
    
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
    
    // Settings section
    Fl_Box* settingsLabel_;
    Fl_Check_Button* gridCheckbox_;
    Fl_Check_Button* aspectRatioCheckbox_;
    Fl_Check_Button* labelsCheckbox_;
    
    // Buttons section
    Fl_Button* loadButton_;
    Fl_Button* clearButton_;
    
    // State
    int fileIndex_ = 0;
    std::string graphTitle_;
    
    // Methods
    void LoadFile(const char* filename);
    void UpdateLegend();
    void OnVisibilityChanged(int functionIndex, bool visible);
    void OnTitleChanged();
    
    // Callbacks
    static void LoadButtonCallback(Fl_Widget* widget, void* data);
    static void ClearButtonCallback(Fl_Widget* widget, void* data);
    static void TitleInputCallback(Fl_Widget* widget, void* data);
    static void GridCheckboxCallback(Fl_Widget* widget, void* data);
    static void AspectRatioCheckboxCallback(Fl_Widget* widget, void* data);
    static void LabelsCheckboxCallback(Fl_Widget* widget, void* data);
    
public:
    MainWindow(int argc, char** argv);
    ~MainWindow();
    
    void Show();
    int Run();
};

MainWindow::MainWindow(int argc, char** argv) : fileIndex_(0), graphTitle_("Real Function Visualizer") {
    // Create main window
    window_ = new Fl_Double_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "MML Real Function Visualizer (FLTK)");
    window_->color(fl_rgb_color(245, 245, 245));
    
    // Calculate layout dimensions
    int graphWidth = WINDOW_WIDTH - SIDEBAR_WIDTH - 3 * MARGIN;
    int graphHeight = WINDOW_HEIGHT - TITLE_BAR_HEIGHT - 2 * MARGIN;
    int graphX = MARGIN;
    int graphY = TITLE_BAR_HEIGHT + MARGIN;
    
    int sidebarX = graphX + graphWidth + MARGIN;
    
    // ==================== GRAPH AREA ====================
    graphWidget_ = new GraphWidget(graphX, graphY, graphWidth, graphHeight);
    graphWidget_->GetStyle().showGrid = true;
    graphWidget_->GetStyle().showAxisLabels = true;
    
    // ==================== SIDEBAR ====================
    sidebarGroup_ = new Fl_Group(sidebarX, 0, SIDEBAR_WIDTH, WINDOW_HEIGHT);
    sidebarGroup_->color(fl_rgb_color(240, 240, 240));
    sidebarGroup_->box(FL_FLAT_BOX);
    
    int yPos = MARGIN;
    int widgetWidth = SIDEBAR_WIDTH - 2 * MARGIN;
    int sectionSpacing = 15;
    
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
    
    // Create scrollable area for legend (can hold many functions)
    int legendHeight = 300;
    legendScroll_ = new Fl_Scroll(sidebarX + MARGIN, yPos, widgetWidth, legendHeight);
    legendScroll_->box(FL_DOWN_FRAME);
    legendScroll_->color(FL_WHITE);
    
    legendWidget_ = new LegendWidget(sidebarX + MARGIN, yPos, widgetWidth - 20, legendHeight);
    legendWidget_->SetVisibilityCallback([this](int idx, bool vis) {
        OnVisibilityChanged(idx, vis);
    });
    
    legendScroll_->end();
    yPos += legendHeight + sectionSpacing;
    
    // ---------- SETTINGS SECTION ----------
    settingsLabel_ = new Fl_Box(sidebarX + MARGIN, yPos, widgetWidth, 20, "Graph Settings:");
    settingsLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    settingsLabel_->labelfont(FL_HELVETICA_BOLD);
    settingsLabel_->labelsize(12);
    yPos += 25;
    
    gridCheckbox_ = new Fl_Check_Button(sidebarX + MARGIN, yPos, widgetWidth, 25, "Show Grid");
    gridCheckbox_->value(1);
    gridCheckbox_->callback(GridCheckboxCallback, this);
    yPos += 28;
    
    labelsCheckbox_ = new Fl_Check_Button(sidebarX + MARGIN, yPos, widgetWidth, 25, "Show Axis Labels");
    labelsCheckbox_->value(1);
    labelsCheckbox_->callback(LabelsCheckboxCallback, this);
    yPos += 28;
    
    aspectRatioCheckbox_ = new Fl_Check_Button(sidebarX + MARGIN, yPos, widgetWidth, 25, "Preserve Aspect Ratio");
    aspectRatioCheckbox_->value(0);
    aspectRatioCheckbox_->callback(AspectRatioCheckboxCallback, this);
    yPos += 35 + sectionSpacing;
    
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
    }
}

MainWindow::~MainWindow() {
    delete window_;
}

void MainWindow::LoadFile(const char* filename) {
    try {
        auto func = MMLFileParser::ParseFile(filename, fileIndex_);
        
        if (func) {
            // Get title from first loaded function if graph title is default
            if (graphTitle_ == "Real Function Visualizer" && !func->GetTitle().empty()) {
                graphTitle_ = func->GetTitle();
                titleInput_->value(graphTitle_.c_str());
            }
            
            graphWidget_->AddFunction(std::move(func));
            fileIndex_++;
            
            UpdateLegend();
            graphWidget_->redraw();
        }
    } catch (const std::exception& e) {
        throw;
    }
}

void MainWindow::UpdateLegend() {
    legendWidget_->Clear();
    
    const auto& functions = graphWidget_->GetFunctions();
    int colorIndex = 0;
    
    for (const auto& func : functions) {
        if (func->GetDimension() == 1) {
            // Single function - convert Color to LegendColor
            Color c = GraphWidget::GetColor(colorIndex);
            legendWidget_->AddItem(
                func->GetTitle(), 
                LegendColor(c.r, c.g, c.b),
                colorIndex,
                func->IsVisible());
            colorIndex++;
        } else {
            // Multi-function - add each sub-function
            auto* multiFunc = dynamic_cast<MultiLoadedFunction*>(func.get());
            if (multiFunc) {
                for (int i = 0; i < func->GetDimension(); ++i) {
                    Color c = GraphWidget::GetColor(i);
                    legendWidget_->AddItem(
                        multiFunc->GetFunctionTitle(i), 
                        LegendColor(c.r, c.g, c.b),
                        i,
                        multiFunc->IsFunctionVisible(i));
                }
            }
            colorIndex += func->GetDimension();
        }
    }
    
    // Resize legend widget to fit all items
    int neededHeight = static_cast<int>(legendWidget_->GetItemCount()) * 28 + 10;
    legendWidget_->size(legendWidget_->w(), std::max(neededHeight, legendScroll_->h()));
    
    legendWidget_->redraw();
    legendScroll_->redraw();
}

void MainWindow::OnVisibilityChanged(int functionIndex, bool visible) {
    // Find the function and update its visibility
    auto& functions = graphWidget_->GetFunctions();
    
    // For now, assuming single function per load or multi-function
    // The functionIndex corresponds to the legend item index
    int currentIndex = 0;
    
    for (auto& func : functions) {
        if (func->GetDimension() == 1) {
            // Single function
            if (currentIndex == functionIndex) {
                func->SetVisible(visible);
                break;
            }
            currentIndex++;
        } else {
            // Multi-function
            auto* multiFunc = dynamic_cast<MultiLoadedFunction*>(func.get());
            if (multiFunc) {
                int numFuncs = func->GetDimension();
                if (functionIndex >= currentIndex && functionIndex < currentIndex + numFuncs) {
                    multiFunc->SetFunctionVisible(functionIndex - currentIndex, visible);
                    break;
                }
                currentIndex += numFuncs;
            }
        }
    }
    
    graphWidget_->redraw();
}

void MainWindow::OnTitleChanged() {
    graphTitle_ = titleInput_->value();
    // Could update window title or a title box on the graph
    std::string windowTitle = "MML Real Function Visualizer - " + graphTitle_;
    window_->label(windowTitle.c_str());
}

void MainWindow::LoadButtonCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    
    // Get the data directory path (WPF data folder)
    std::string defaultPath = "../../WPF/MML_RealFunctionVisualizer/data";
    
    Fl_File_Chooser chooser(defaultPath.c_str(), "MML Files (*.txt)", 
                            Fl_File_Chooser::SINGLE, "Select MML Data File");
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
    mainWin->graphWidget_->ClearFunctions();
    mainWin->legendWidget_->Clear();
    mainWin->fileIndex_ = 0;
    mainWin->graphTitle_ = "Real Function Visualizer";
    mainWin->titleInput_->value(mainWin->graphTitle_.c_str());
    mainWin->window_->label("MML Real Function Visualizer (FLTK)");
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
    mainWin->graphWidget_->GetStyle().showGrid = (checkbox->value() != 0);
    mainWin->graphWidget_->redraw();
}

void MainWindow::AspectRatioCheckboxCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Check_Button* checkbox = static_cast<Fl_Check_Button*>(widget);
    mainWin->graphWidget_->SetPreserveAspectRatio(checkbox->value() != 0);
    mainWin->graphWidget_->redraw();
}

void MainWindow::LabelsCheckboxCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Check_Button* checkbox = static_cast<Fl_Check_Button*>(widget);
    mainWin->graphWidget_->GetStyle().showAxisLabels = (checkbox->value() != 0);
    mainWin->graphWidget_->redraw();
}

void MainWindow::Show() {
    window_->show();
}

int MainWindow::Run() {
    return Fl::run();
}

int main(int argc, char** argv) {
    // Set FLTK scheme for better appearance
    Fl::scheme("gtk+");
    
    MainWindow mainWindow(argc, argv);
    mainWindow.Show();
    return mainWindow.Run();
}
