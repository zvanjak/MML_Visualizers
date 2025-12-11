#define NOMINMAX
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Box.H>
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
    Fl_Button* loadButton_;
    Fl_Button* clearButton_;
    Fl_Box* titleLabel_;
    Fl_Box* infoLabel_;
    
    void LoadFile(const char* filename);
    void UpdateInfo();
    
    static void LoadButtonCallback(Fl_Widget* widget, void* data);
    static void ClearButtonCallback(Fl_Widget* widget, void* data);
    
public:
    MainWindow(int argc, char** argv);
    ~MainWindow();
    
    void Show();
    int Run();
};

MainWindow::MainWindow(int argc, char** argv) {
    // Create main window
    window_ = new Fl_Window(1000, 750, "MML Vector Field 2D Visualizer (FLTK)");
    
    // Create title label
    titleLabel_ = new Fl_Box(10, 10, 980, 30, "Vector Field 2D");
    titleLabel_->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    titleLabel_->box(FL_FLAT_BOX);
    titleLabel_->labelsize(16);
    titleLabel_->labelfont(FL_BOLD);
    
    // Create vector field widget
    vectorFieldWidget_ = new VectorFieldWidget(10, 50, 980, 600);
    
    // Create info label
    infoLabel_ = new Fl_Box(10, 660, 980, 30, "No data loaded");
    infoLabel_->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    
    // Create buttons
    loadButton_ = new Fl_Button(10, 700, 100, 30, "Load File");
    loadButton_->callback(LoadButtonCallback, this);
    
    clearButton_ = new Fl_Button(120, 700, 100, 30, "Clear");
    clearButton_->callback(ClearButtonCallback, this);
    
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
