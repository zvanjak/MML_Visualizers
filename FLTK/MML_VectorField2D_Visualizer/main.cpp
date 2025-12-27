#define NOMINMAX
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Group.H>
#include <FL/fl_ask.H>
#include "VectorFieldWidget.h"
#include "MMLFileParser.h"
#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>

class MainWindow {
private:
    Fl_Window* window_;
    VectorFieldWidget* vectorFieldWidget_;
    
    // Buttons
    Fl_Button* loadButton_;
    Fl_Button* clearButton_;
    
    // Labels
    Fl_Box* titleLabel_;
    Fl_Box* infoLabel_;
    
    // Sidebar controls
    Fl_Group* sidebarGroup_;
    Fl_Box* displayLabel_;
    Fl_Value_Slider* magnitudeScaleSlider_;
    Fl_Value_Slider* arrowSizeSlider_;
    Fl_Check_Button* colorByMagnitudeCheck_;
    Fl_Check_Button* normalizeVectorsCheck_;
    Fl_Check_Button* preserveAspectRatioCheck_;
    Fl_Box* arrowColorLabel_;
    Fl_Choice* arrowColorChoice_;
    
    // Statistics display
    Fl_Box* statsLabel_;
    Fl_Box* vectorCountLabel_;
    Fl_Box* minMagLabel_;
    Fl_Box* maxMagLabel_;
    Fl_Box* avgMagLabel_;
    
    void LoadFile(const char* filename);
    void UpdateInfo();
    void UpdateStatistics();
    
    static void LoadButtonCallback(Fl_Widget* widget, void* data);
    static void ClearButtonCallback(Fl_Widget* widget, void* data);
    static void MagnitudeScaleCallback(Fl_Widget* widget, void* data);
    static void ArrowSizeCallback(Fl_Widget* widget, void* data);
    static void ColorByMagnitudeCallback(Fl_Widget* widget, void* data);
    static void NormalizeVectorsCallback(Fl_Widget* widget, void* data);
    static void PreserveAspectRatioCallback(Fl_Widget* widget, void* data);
    static void ArrowColorCallback(Fl_Widget* widget, void* data);
    
public:
    MainWindow(int argc, char** argv);
    ~MainWindow();
    
    void Show();
    int Run();
};

MainWindow::MainWindow(int argc, char** argv) {
    // Create main window (wider to accommodate sidebar)
    window_ = new Fl_Window(1200, 750, "MML Vector Field 2D Visualizer (FLTK)");
    
    // Create title label
    titleLabel_ = new Fl_Box(10, 10, 980, 30, "Vector Field 2D");
    titleLabel_->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    titleLabel_->box(FL_FLAT_BOX);
    titleLabel_->labelsize(16);
    titleLabel_->labelfont(FL_BOLD);
    
    // Create vector field widget (leave space on right for sidebar)
    vectorFieldWidget_ = new VectorFieldWidget(10, 50, 980, 600);
    
    // Create info label
    infoLabel_ = new Fl_Box(10, 660, 980, 30, "No data loaded");
    infoLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    
    // Create buttons
    loadButton_ = new Fl_Button(10, 700, 100, 30, "Load File");
    loadButton_->callback(LoadButtonCallback, this);
    
    clearButton_ = new Fl_Button(120, 700, 100, 30, "Clear");
    clearButton_->callback(ClearButtonCallback, this);
    
    // === Sidebar Group ===
    int sidebarX = 1000;
    int sidebarWidth = 190;
    int controlY = 50;
    int controlHeight = 25;
    int spacing = 30;
    
    sidebarGroup_ = new Fl_Group(sidebarX, 10, sidebarWidth, 680);
    
    // Display Settings label
    displayLabel_ = new Fl_Box(sidebarX, controlY, sidebarWidth, controlHeight, "Display Settings");
    displayLabel_->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    displayLabel_->labelfont(FL_BOLD);
    displayLabel_->labelsize(12);
    controlY += spacing + 5;
    
    // Magnitude Scale slider
    magnitudeScaleSlider_ = new Fl_Value_Slider(sidebarX, controlY, sidebarWidth, controlHeight, "Magnitude Scale");
    magnitudeScaleSlider_->type(FL_HOR_NICE_SLIDER);
    magnitudeScaleSlider_->bounds(0.1, 5.0);
    magnitudeScaleSlider_->value(1.0);
    magnitudeScaleSlider_->step(0.1);
    magnitudeScaleSlider_->align(FL_ALIGN_TOP);
    magnitudeScaleSlider_->callback(MagnitudeScaleCallback, this);
    controlY += spacing + 15;
    
    // Arrow Size slider
    arrowSizeSlider_ = new Fl_Value_Slider(sidebarX, controlY, sidebarWidth, controlHeight, "Arrow Size");
    arrowSizeSlider_->type(FL_HOR_NICE_SLIDER);
    arrowSizeSlider_->bounds(2, 20);
    arrowSizeSlider_->value(8);
    arrowSizeSlider_->step(1);
    arrowSizeSlider_->align(FL_ALIGN_TOP);
    arrowSizeSlider_->callback(ArrowSizeCallback, this);
    controlY += spacing + 20;
    
    // Checkboxes
    colorByMagnitudeCheck_ = new Fl_Check_Button(sidebarX, controlY, sidebarWidth, controlHeight, "Color by Magnitude");
    colorByMagnitudeCheck_->value(1);  // Checked by default
    colorByMagnitudeCheck_->callback(ColorByMagnitudeCallback, this);
    controlY += spacing;
    
    normalizeVectorsCheck_ = new Fl_Check_Button(sidebarX, controlY, sidebarWidth, controlHeight, "Normalize Vectors");
    normalizeVectorsCheck_->value(0);  // Unchecked by default
    normalizeVectorsCheck_->callback(NormalizeVectorsCallback, this);
    controlY += spacing;
    
    preserveAspectRatioCheck_ = new Fl_Check_Button(sidebarX, controlY, sidebarWidth, controlHeight, "Preserve Aspect Ratio");
    preserveAspectRatioCheck_->value(0);  // Unchecked by default
    preserveAspectRatioCheck_->callback(PreserveAspectRatioCallback, this);
    controlY += spacing + 10;
    
    // Arrow Color dropdown
    arrowColorLabel_ = new Fl_Box(sidebarX, controlY, sidebarWidth, controlHeight, "Arrow Color (when solid):");
    arrowColorLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    controlY += controlHeight;
    
    arrowColorChoice_ = new Fl_Choice(sidebarX, controlY, sidebarWidth, controlHeight);
    arrowColorChoice_->add("Black");
    arrowColorChoice_->add("Blue");
    arrowColorChoice_->add("Red");
    arrowColorChoice_->add("Green");
    arrowColorChoice_->add("Orange");
    arrowColorChoice_->add("Purple");
    arrowColorChoice_->value(0);  // Black by default
    arrowColorChoice_->callback(ArrowColorCallback, this);
    controlY += spacing + 20;
    
    // Statistics section
    statsLabel_ = new Fl_Box(sidebarX, controlY, sidebarWidth, controlHeight, "Statistics");
    statsLabel_->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    statsLabel_->labelfont(FL_BOLD);
    statsLabel_->labelsize(12);
    controlY += spacing;
    
    vectorCountLabel_ = new Fl_Box(sidebarX, controlY, sidebarWidth, controlHeight, "Vectors: -");
    vectorCountLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    controlY += controlHeight;
    
    minMagLabel_ = new Fl_Box(sidebarX, controlY, sidebarWidth, controlHeight, "Min Mag: -");
    minMagLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    controlY += controlHeight;
    
    maxMagLabel_ = new Fl_Box(sidebarX, controlY, sidebarWidth, controlHeight, "Max Mag: -");
    maxMagLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    controlY += controlHeight;
    
    avgMagLabel_ = new Fl_Box(sidebarX, controlY, sidebarWidth, controlHeight, "Avg Mag: -");
    avgMagLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    
    sidebarGroup_->end();
    
    window_->end();
    window_->resizable(vectorFieldWidget_);
    
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
        auto vectorField = MMLFileParser::ParseFile(filename);
        
        if (vectorField) {
            titleLabel_->copy_label(vectorField->GetTitle().c_str());
            vectorFieldWidget_->SetVectorField(std::move(vectorField));
            UpdateInfo();
            UpdateStatistics();
            vectorFieldWidget_->redraw();
        }
    } catch (const std::exception& e) {
        throw;
    }
}

void MainWindow::UpdateInfo() {
    const VectorField2D* field = vectorFieldWidget_->GetVectorField();
    if (!field) {
        infoLabel_->label("No data loaded");
        return;
    }
    
    std::ostringstream oss;
    oss << "Vectors: " << field->GetNumVectors();
    oss << " | Max magnitude: " << std::fixed << std::setprecision(2) 
        << field->GetMaxVectorMagnitude();
    
    infoLabel_->copy_label(oss.str().c_str());
    infoLabel_->redraw();
}

void MainWindow::UpdateStatistics() {
    const VectorField2D* field = vectorFieldWidget_->GetVectorField();
    if (!field) {
        vectorCountLabel_->label("Vectors: -");
        minMagLabel_->label("Min Mag: -");
        maxMagLabel_->label("Max Mag: -");
        avgMagLabel_->label("Avg Mag: -");
        return;
    }
    
    std::ostringstream oss;
    
    oss.str(""); oss << "Vectors: " << field->GetNumVectors();
    vectorCountLabel_->copy_label(oss.str().c_str());
    
    oss.str(""); oss << std::fixed << std::setprecision(3);
    oss << "Min Mag: " << field->GetMinVectorMagnitude();
    minMagLabel_->copy_label(oss.str().c_str());
    
    oss.str(""); oss << "Max Mag: " << field->GetMaxVectorMagnitude();
    maxMagLabel_->copy_label(oss.str().c_str());
    
    oss.str(""); oss << "Avg Mag: " << field->GetAvgVectorMagnitude();
    avgMagLabel_->copy_label(oss.str().c_str());
    
    vectorCountLabel_->redraw();
    minMagLabel_->redraw();
    maxMagLabel_->redraw();
    avgMagLabel_->redraw();
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

void MainWindow::ClearButtonCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    mainWin->vectorFieldWidget_->ClearVectorField();
    mainWin->titleLabel_->label("Vector Field 2D");
    mainWin->infoLabel_->label("No data loaded");
    mainWin->UpdateStatistics();
    mainWin->vectorFieldWidget_->redraw();
}

void MainWindow::MagnitudeScaleCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Value_Slider* slider = static_cast<Fl_Value_Slider*>(widget);
    mainWin->vectorFieldWidget_->SetMagnitudeScale(slider->value());
    mainWin->vectorFieldWidget_->redraw();
}

void MainWindow::ArrowSizeCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Value_Slider* slider = static_cast<Fl_Value_Slider*>(widget);
    mainWin->vectorFieldWidget_->SetArrowSize(static_cast<int>(slider->value()));
    mainWin->vectorFieldWidget_->redraw();
}

void MainWindow::ColorByMagnitudeCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Check_Button* check = static_cast<Fl_Check_Button*>(widget);
    mainWin->vectorFieldWidget_->SetColorByMagnitude(check->value() != 0);
    mainWin->vectorFieldWidget_->redraw();
}

void MainWindow::NormalizeVectorsCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Check_Button* check = static_cast<Fl_Check_Button*>(widget);
    mainWin->vectorFieldWidget_->SetNormalizeVectors(check->value() != 0);
    mainWin->vectorFieldWidget_->redraw();
}

void MainWindow::PreserveAspectRatioCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Check_Button* check = static_cast<Fl_Check_Button*>(widget);
    mainWin->vectorFieldWidget_->SetPreserveAspectRatio(check->value() != 0);
    mainWin->vectorFieldWidget_->redraw();
}

void MainWindow::ArrowColorCallback(Fl_Widget* widget, void* data) {
    MainWindow* mainWin = static_cast<MainWindow*>(data);
    Fl_Choice* choice = static_cast<Fl_Choice*>(widget);
    mainWin->vectorFieldWidget_->SetArrowColorIndex(choice->value());
    mainWin->vectorFieldWidget_->redraw();
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
