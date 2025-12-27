#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>

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
    
    // === Vector Scaling Panel ===
    QGroupBox* scalingGroup = new QGroupBox("Vector Scaling", this);
    QVBoxLayout* scalingLayout = new QVBoxLayout(scalingGroup);
    
    // Magnitude scale slider
    QHBoxLayout* magScaleLayout = new QHBoxLayout();
    magScaleLayout->addWidget(new QLabel("Magnitude Scale:", this));
    magnitudeScaleLabel_ = new QLabel("1.00", this);
    magnitudeScaleLabel_->setMinimumWidth(40);
    magScaleLayout->addWidget(magnitudeScaleLabel_);
    scalingLayout->addLayout(magScaleLayout);
    
    magnitudeScaleSlider_ = new QSlider(Qt::Horizontal, this);
    magnitudeScaleSlider_->setMinimum(10);
    magnitudeScaleSlider_->setMaximum(500);
    magnitudeScaleSlider_->setValue(100);
    connect(magnitudeScaleSlider_, &QSlider::valueChanged, this, &MainWindow::OnMagnitudeScaleChanged);
    scalingLayout->addWidget(magnitudeScaleSlider_);
    
    // Arrow size slider
    QHBoxLayout* arrowSizeLayout = new QHBoxLayout();
    arrowSizeLayout->addWidget(new QLabel("Arrow Size:", this));
    arrowSizeLabel_ = new QLabel("8", this);
    arrowSizeLabel_->setMinimumWidth(40);
    arrowSizeLayout->addWidget(arrowSizeLabel_);
    scalingLayout->addLayout(arrowSizeLayout);
    
    arrowSizeSlider_ = new QSlider(Qt::Horizontal, this);
    arrowSizeSlider_->setMinimum(2);
    arrowSizeSlider_->setMaximum(20);
    arrowSizeSlider_->setValue(8);
    connect(arrowSizeSlider_, &QSlider::valueChanged, this, &MainWindow::OnArrowSizeChanged);
    scalingLayout->addWidget(arrowSizeSlider_);
    
    rightLayout->addWidget(scalingGroup);
    
    // === Display Options Panel ===
    QGroupBox* displayGroup = new QGroupBox("Display Options", this);
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);
    
    // Color by magnitude checkbox
    colorByMagnitudeCheckbox_ = new QCheckBox("Color by Magnitude", this);
    colorByMagnitudeCheckbox_->setChecked(true);
    connect(colorByMagnitudeCheckbox_, &QCheckBox::stateChanged, this, &MainWindow::OnColorByMagnitudeChanged);
    displayLayout->addWidget(colorByMagnitudeCheckbox_);
    
    // Normalize vectors checkbox
    normalizeCheckbox_ = new QCheckBox("Normalize Vectors", this);
    connect(normalizeCheckbox_, &QCheckBox::stateChanged, this, &MainWindow::OnNormalizeChanged);
    displayLayout->addWidget(normalizeCheckbox_);
    
    // Preserve aspect ratio checkbox
    preserveAspectRatioCheckbox_ = new QCheckBox("Preserve Aspect Ratio", this);
    connect(preserveAspectRatioCheckbox_, &QCheckBox::stateChanged, this, &MainWindow::OnPreserveAspectRatioChanged);
    displayLayout->addWidget(preserveAspectRatioCheckbox_);
    
    // Arrow color dropdown
    QHBoxLayout* colorLayout = new QHBoxLayout();
    colorLayout->addWidget(new QLabel("Arrow Color:", this));
    arrowColorCombo_ = new QComboBox(this);
    arrowColorCombo_->addItem("Black");
    arrowColorCombo_->addItem("Blue");
    arrowColorCombo_->addItem("Red");
    arrowColorCombo_->addItem("Green");
    arrowColorCombo_->addItem("Orange");
    arrowColorCombo_->addItem("Purple");
    connect(arrowColorCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::OnArrowColorChanged);
    colorLayout->addWidget(arrowColorCombo_);
    displayLayout->addLayout(colorLayout);
    
    rightLayout->addWidget(displayGroup);
    
    // === Statistics Panel ===
    QGroupBox* statsGroup = new QGroupBox("Field Statistics", this);
    QVBoxLayout* statsLayout = new QVBoxLayout(statsGroup);
    
    vectorCountLabel_ = new QLabel("Vectors: -", this);
    minMagLabel_ = new QLabel("Min Magnitude: -", this);
    maxMagLabel_ = new QLabel("Max Magnitude: -", this);
    avgMagLabel_ = new QLabel("Avg Magnitude: -", this);
    
    statsLayout->addWidget(vectorCountLabel_);
    statsLayout->addWidget(minMagLabel_);
    statsLayout->addWidget(maxMagLabel_);
    statsLayout->addWidget(avgMagLabel_);
    
    rightLayout->addWidget(statsGroup);
    
    // === Info Panel ===
    QGroupBox* infoGroup = new QGroupBox("Information", this);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup);
    infoText_ = new QTextEdit(this);
    infoText_->setReadOnly(true);
    infoText_->setMaximumHeight(120);
    infoLayout->addWidget(infoText_);
    rightLayout->addWidget(infoGroup);
    
    // === File Controls ===
    QGroupBox* fileGroup = new QGroupBox("File", this);
    QVBoxLayout* fileLayout = new QVBoxLayout(fileGroup);
    QPushButton* loadBtn = new QPushButton("Load Data File...", this);
    QPushButton* resetBtn = new QPushButton("Reset View", this);
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::OnLoadFile);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::OnReset);
    fileLayout->addWidget(loadBtn);
    fileLayout->addWidget(resetBtn);
    rightLayout->addWidget(fileGroup);
    
    rightLayout->addStretch();
    rightPanel->setMaximumWidth(280);
    
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
        UpdateStatistics();
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
    UpdateStatistics();
}

void MainWindow::OnMagnitudeScaleChanged(int value) {
    double scale = value / 100.0;
    magnitudeScaleLabel_->setText(QString::number(scale, 'f', 2));
    glWidget_->SetMagnitudeScale(scale);
}

void MainWindow::OnArrowSizeChanged(int value) {
    arrowSizeLabel_->setText(QString::number(value));
    glWidget_->SetArrowSize(value);
}

void MainWindow::OnNormalizeChanged(int state) {
    glWidget_->SetNormalizeVectors(state == Qt::Checked);
}

void MainWindow::OnColorByMagnitudeChanged(int state) {
    glWidget_->SetColorByMagnitude(state == Qt::Checked);
}

void MainWindow::OnPreserveAspectRatioChanged(int state) {
    glWidget_->SetPreserveAspectRatio(state == Qt::Checked);
}

void MainWindow::OnArrowColorChanged(int index) {
    glWidget_->SetArrowColorIndex(index);
}

void MainWindow::UpdateInfo() {
    QString info;
    info += QString("File: %1\n\n").arg(QFileInfo(currentFilename_).fileName());
    info += "Controls:\n";
    info += "- Mouse drag: Pan view\n";
    info += "- Mouse wheel: Zoom\n";
    info += "- Sliders: Adjust vector display\n";
    
    infoText_->setPlainText(info);
}

void MainWindow::UpdateStatistics() {
    const VectorField2D* field = glWidget_->GetVectorField();
    
    if (!field) {
        vectorCountLabel_->setText("Vectors: -");
        minMagLabel_->setText("Min Magnitude: -");
        maxMagLabel_->setText("Max Magnitude: -");
        avgMagLabel_->setText("Avg Magnitude: -");
        return;
    }
    
    vectorCountLabel_->setText(QString("Vectors: %1").arg(field->GetNumVectors()));
    minMagLabel_->setText(QString("Min Magnitude: %1").arg(field->GetMinMagnitude(), 0, 'f', 4));
    maxMagLabel_->setText(QString("Max Magnitude: %1").arg(field->GetMaxMagnitude(), 0, 'f', 4));
    avgMagLabel_->setText(QString("Avg Magnitude: %1").arg(field->GetAvgMagnitude(), 0, 'f', 4));
}
