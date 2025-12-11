#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include "GraphWidget.h"
#include "LegendWidget.h"
#include "MMLFileParser.h"
#include <iostream>
#include <memory>

class MainWindow {
private:
    Fl_Window* window_;
    GraphWidget* graphWidget_;
    LegendWidget* legendWidget_;
    Fl_Button* loadButton_;
    Fl_Button* clearButton_;
    
    int fileIndex_ = 0;
    
    void LoadFile(const char* filename);
    void UpdateLegend();
    
    static void LoadButtonCallback(Fl_Widget* widget, void* data);
    static void ClearButtonCallback(Fl_Widget* widget, void* data);
    
public:
    MainWindow(int argc, char** argv);
    ~MainWindow();
    
    void Show();
    int Run();
};

MainWindow::MainWindow(int argc, char** argv) : fileIndex_(0) {
    // Create main window
    window_ = new Fl_Window(1200, 700, "MML Parametric Curve 2D Visualizer (FLTK)");
    
    // Create graph widget
    graphWidget_ = new GraphWidget(10, 10, 900, 600);
    
    // Create legend widget
    legendWidget_ = new LegendWidget(920, 10, 270, 600, "Legend");
    
    // Create buttons
    loadButton_ = new Fl_Button(10, 620, 100, 30, "Load File");
    loadButton_->callback(LoadButtonCallback, this);
    
    clearButton_ = new Fl_Button(120, 620, 100, 30, "Clear");
    clearButton_->callback(ClearButtonCallback, this);
    
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
        auto curve = MMLFileParser::ParseFile(filename, fileIndex_);
        
        if (curve) {
            graphWidget_->AddCurve(std::move(curve));
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
    
    const auto& curves = graphWidget_->GetCurves();
    int colorIndex = 0;
    
    for (const auto& curve : curves) {
        legendWidget_->AddItem(curve->GetTitle(), GraphWidget::GetColor(colorIndex));
        colorIndex++;
    }
    
    legendWidget_->redraw();
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
    mainWin->graphWidget_->ClearCurves();
    mainWin->legendWidget_->Clear();
    mainWin->fileIndex_ = 0;
    mainWin->graphWidget_->redraw();
    mainWin->legendWidget_->redraw();
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
