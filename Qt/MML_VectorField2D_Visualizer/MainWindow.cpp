#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    SetupUI();
    setWindowTitle("MML Vector Field 2D Visualizer - Qt");
    resize(1200, 800);
}

MainWindow::~MainWindow() {
}

void MainWindow::SetupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Left panel - OpenGL widget
    glWidget_ = new GLWidget(this);
    glWidget_->setMinimumSize(800, 600);
    
    // Right panel - controls
    QWidget* rightPanel = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    
    // Arrow rendering controls
    QGroupBox* renderGroup = new QGroupBox("Arrow Rendering", this);
    QVBoxLayout* renderLayout = new QVBoxLayout(renderGroup);
    
    // Arrow scale slider
    QHBoxLayout* scaleLayout = new QHBoxLayout();
    scaleLayout->addWidget(new QLabel("Arrow Scale:", this));
    arrowScaleSlider_ = new QSlider(Qt::Horizontal, this);
    arrowScaleSlider_->setMinimum(10);
    arrowScaleSlider_->setMaximum(200);
    arrowScaleSlider_->setValue(50);
    connect(arrowScaleSlider_, &QSlider::valueChanged, this, &MainWindow::OnArrowScaleChanged);
    scaleLayout->addWidget(arrowScaleSlider_);
    renderLayout->addLayout(scaleLayout);
    
    // Normalize vectors checkbox
    normalizeCheckbox_ = new QCheckBox("Normalize Vectors", this);
    connect(normalizeCheckbox_, &QCheckBox::stateChanged, this, &MainWindow::OnNormalizeChanged);
    renderLayout->addWidget(normalizeCheckbox_);
    
    // Color by magnitude checkbox
    colorMagnitudeCheckbox_ = new QCheckBox("Color by Magnitude", this);
    colorMagnitudeCheckbox_->setChecked(true);
    connect(colorMagnitudeCheckbox_, &QCheckBox::stateChanged, this, &MainWindow::OnColorMagnitudeChanged);
    renderLayout->addWidget(colorMagnitudeCheckbox_);
    
    rightLayout->addWidget(renderGroup);
    
    // Info panel
    QGroupBox* infoGroup = new QGroupBox("Information", this);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup);
    infoText_ = new QTextEdit(this);
    infoText_->setReadOnly(true);
    infoText_->setMaximumHeight(200);
    infoLayout->addWidget(infoText_);
    rightLayout->addWidget(infoGroup);
    
    // File controls
    QGroupBox* fileGroup = new QGroupBox("File", this);
    QVBoxLayout* fileLayout = new QVBoxLayout(fileGroup);
    QPushButton* loadBtn = new QPushButton("Load Data File...", this);
    QPushButton* resetBtn = new QPushButton("Reset", this);
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::OnLoadFile);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::OnReset);
    fileLayout->addWidget(loadBtn);
    fileLayout->addWidget(resetBtn);
    rightLayout->addWidget(fileGroup);
    
    rightLayout->addStretch();
    rightPanel->setMaximumWidth(300);
    
    // Add to main layout
    mainLayout->addWidget(glWidget_, 1);
    mainLayout->addWidget(rightPanel);
}

void MainWindow::LoadDataFile(const QString& filename) {
    try {
        auto vectorField = MMLFileParser::ParseFile(filename.toStdString());
        currentFilename_ = filename;
        glWidget_->LoadVectorField(std::move(vectorField));
        UpdateInfo();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Failed to load file:\n%1").arg(e.what()));
    }
}

void MainWindow::OnLoadFile() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Select Vector Field Data File",
        QString(),
        "Text Files (*.txt);;All Files (*.*)"
    );
    
    if (!filename.isEmpty()) {
        LoadDataFile(filename);
    }
}

void MainWindow::OnReset() {
    glWidget_->ClearVectorField();
    infoText_->clear();
    currentFilename_.clear();
}

void MainWindow::OnArrowScaleChanged(int value) {
    glWidget_->SetArrowScale(value / 100.0);
}

void MainWindow::OnNormalizeChanged(int state) {
    glWidget_->SetNormalizeVectors(state == Qt::Checked);
}

void MainWindow::OnColorMagnitudeChanged(int state) {
    glWidget_->SetShowMagnitudeColor(state == Qt::Checked);
}

void MainWindow::UpdateInfo() {
    QString info;
    info += QString("File: %1\n").arg(QFileInfo(currentFilename_).fileName());
    info += "\n";
    info += "Rendering Options:\n";
    info += QString("- Arrow Scale: %1\n").arg(glWidget_->GetArrowScale(), 0, 'f', 2);
    info += QString("- Normalize: %1\n").arg(glWidget_->GetNormalizeVectors() ? "Yes" : "No");
    info += QString("- Color by Magnitude: %1\n").arg(glWidget_->GetShowMagnitudeColor() ? "Yes" : "No");
    info += "\n";
    info += "Controls:\n";
    info += "- Mouse drag: Pan view\n";
    info += "- Mouse wheel: Zoom\n";
    info += "- Arrow scale slider: Adjust arrow length\n";
    info += "- Normalize: Show direction only\n";
    info += "- Color: Blue (low) -> Red (high)\n";
    
    infoText_->setPlainText(info);
}
