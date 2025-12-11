#define NOMINMAX
#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QGroupBox>
#include <QStatusBar>
#include <sstream>

MainWindow::MainWindow(const std::vector<std::string>& filenames, QWidget *parent)
    : QMainWindow(parent)
    , curveCounter_(0) {
    
    setWindowTitle("MML Parametric Curve 3D Visualizer - Qt Edition");
    resize(1200, 800);
    
    // Create central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Create splitter
    QSplitter* splitter = new QSplitter(Qt::Horizontal, centralWidget);
    
    // Left side: OpenGL widget
    glWidget_ = new GLWidget(splitter);
    glWidget_->setMinimumSize(640, 480);
    
    // Right side: Control panel
    QWidget* controlPanel = new QWidget(splitter);
    QVBoxLayout* controlLayout = new QVBoxLayout(controlPanel);
    
    // Buttons
    loadButton_ = new QPushButton("Load Curve File", controlPanel);
    connect(loadButton_, &QPushButton::clicked, this, &MainWindow::LoadFile);
    controlLayout->addWidget(loadButton_);
    
    resetButton_ = new QPushButton("Reset View", controlPanel);
    connect(resetButton_, &QPushButton::clicked, this, &MainWindow::ResetView);
    controlLayout->addWidget(resetButton_);
    
    // Info display
    QGroupBox* infoGroup = new QGroupBox("Curve Information", controlPanel);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup);
    
    infoDisplay_ = new QTextEdit(infoGroup);
    infoDisplay_->setReadOnly(true);
    infoDisplay_->setMaximumHeight(300);
    infoLayout->addWidget(infoDisplay_);
    
    controlLayout->addWidget(infoGroup);
    
    // Instructions
    QGroupBox* instructionsGroup = new QGroupBox("Mouse Controls", controlPanel);
    QVBoxLayout* instructionsLayout = new QVBoxLayout(instructionsGroup);
    
    QLabel* instructions = new QLabel(
        "<b>Left Mouse:</b> Rotate camera<br>"
        "<b>Right/Middle Mouse:</b> Pan camera<br>"
        "<b>Mouse Wheel:</b> Zoom in/out<br>"
        "<br>"
        "<b>Axes:</b><br>"
        "<font color='red'>■</font> X-axis (Red)<br>"
        "<font color='green'>■</font> Y-axis (Green)<br>"
        "<font color='blue'>■</font> Z-axis (Blue)",
        instructionsGroup
    );
    instructions->setWordWrap(true);
    instructionsLayout->addWidget(instructions);
    
    controlLayout->addWidget(instructionsGroup);
    
    controlLayout->addStretch();
    
    // Status bar
    statusLabel_ = new QLabel("Ready", this);
    statusBar()->addWidget(statusLabel_);
    
    // Set splitter sizes
    splitter->addWidget(glWidget_);
    splitter->addWidget(controlPanel);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(splitter);
    
    // Load initial files if provided
    for (const auto& filename : filenames) {
        LoadCurveFile(QString::fromStdString(filename));
    }
    
    UpdateInfoDisplay();
}

MainWindow::~MainWindow() {
}

void MainWindow::LoadFile() {
    QStringList filenames = QFileDialog::getOpenFileNames(
        this,
        "Open Parametric Curve 3D Files",
        "",
        "MML Files (*.txt);;All Files (*.*)"
    );
    
    for (const QString& filename : filenames) {
        LoadCurveFile(filename);
    }
}

void MainWindow::LoadCurveFile(const QString& filename) {
    try {
        auto curve = ParseParametricCurve3D(filename.toStdString());
        Color color = GetColorForIndex(curveCounter_++);
        
        glWidget_->AddCurve(std::move(curve), color);
        loadedFilenames_.push_back(filename.toStdString());
        
        UpdateInfoDisplay();
        statusLabel_->setText("Loaded: " + filename);
        
    } catch (const std::exception& e) {
        QMessageBox::warning(
            this,
            "Error Loading File",
            QString("Failed to load file:\n%1\n\nError: %2")
                .arg(filename)
                .arg(e.what())
        );
    }
}

void MainWindow::ResetView() {
    glWidget_->ResetCamera();
    statusLabel_->setText("View reset");
}

void MainWindow::UpdateInfoDisplay() {
    if (loadedFilenames_.empty()) {
        infoDisplay_->setHtml(
            "<p><i>No curves loaded yet.</i></p>"
            "<p>Click 'Load Curve File' to open a data file.</p>"
        );
        return;
    }
    
    std::ostringstream oss;
    oss << "<b>Loaded Curves: " << loadedFilenames_.size() << "</b><br><br>";
    
    for (size_t i = 0; i < loadedFilenames_.size(); ++i) {
        Color color = GetColorForIndex(i);
        
        // Extract filename from path
        std::string filename = loadedFilenames_[i];
        size_t lastSlash = filename.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            filename = filename.substr(lastSlash + 1);
        }
        
        oss << "<font color='rgb(" << color.r << "," << color.g << "," << color.b << ")'>";
        oss << "■</font> <b>Curve " << (i + 1) << ":</b> " << filename << "<br>";
    }
    
    infoDisplay_->setHtml(QString::fromStdString(oss.str()));
}

Color MainWindow::GetColorForIndex(int index) {
    static const std::vector<Color> palette = {
        Color(0, 0, 0),       // Black
        Color(0, 0, 255),     // Blue
        Color(255, 0, 0),     // Red
        Color(0, 128, 0),     // Green
        Color(255, 0, 255),   // Magenta
        Color(0, 128, 128),   // Teal
        Color(255, 128, 0),   // Orange
        Color(128, 0, 128),   // Purple
        Color(128, 128, 0),   // Olive
        Color(0, 255, 255),   // Cyan
    };
    
    return palette[index % palette.size()];
}
