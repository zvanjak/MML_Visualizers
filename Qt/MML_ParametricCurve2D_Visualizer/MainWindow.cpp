#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QScrollArea>
#include <QIntValidator>
#include <sstream>
#include <iomanip>

MainWindow::MainWindow(const std::vector<std::string>& filenames, QWidget *parent)
    : QMainWindow(parent)
    , curveCounter_(0)
{
    setWindowTitle("MML Parametric Curve 2D Visualizer (Qt + OpenGL)");
    resize(1200, 700);

    // Create central widget and main layout
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // Create GL widget
    glWidget_ = new GLWidget(this);
    mainLayout->addWidget(glWidget_, 1);  // Stretch factor 1

    // Create sidebar
    CreateSidebar();
    mainLayout->addWidget(sidebarWidget_);

    // Create status bar
    statusBar_ = new QStatusBar(this);
    setStatusBar(statusBar_);
    statusBar_->showMessage("Ready - Load a parametric curve file or pass files as arguments");

    // Set animation callback to update UI
    glWidget_->SetAnimationFrameCallback([this]() {
        OnAnimationFrame();
    });

    // Load initial files from command line arguments
    for (size_t i = 0; i < filenames.size(); i++) {
        LoadCurveFile(QString::fromStdString(filenames[i]), i);
    }
}

MainWindow::~MainWindow() {
}

void MainWindow::CreateSidebar() {
    sidebarWidget_ = new QWidget(this);
    sidebarWidget_->setFixedWidth(280);
    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebarWidget_);
    sidebarLayout->setContentsMargins(5, 5, 5, 5);
    sidebarLayout->setSpacing(10);

    // === File Group ===
    fileGroup_ = new QGroupBox("File", sidebarWidget_);
    QVBoxLayout* fileLayout = new QVBoxLayout(fileGroup_);
    
    loadButton_ = new QPushButton("Load Curve...", fileGroup_);
    clearButton_ = new QPushButton("Clear All", fileGroup_);
    resetViewButton_ = new QPushButton("Reset View", fileGroup_);
    
    fileLayout->addWidget(loadButton_);
    fileLayout->addWidget(clearButton_);
    fileLayout->addWidget(resetViewButton_);
    
    connect(loadButton_, &QPushButton::clicked, this, &MainWindow::LoadFile);
    connect(clearButton_, &QPushButton::clicked, this, &MainWindow::ClearAll);
    connect(resetViewButton_, &QPushButton::clicked, this, &MainWindow::ResetView);
    
    sidebarLayout->addWidget(fileGroup_);

    // === Display Settings Group ===
    displayGroup_ = new QGroupBox("Display", sidebarWidget_);
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup_);
    
    showGridCheckbox_ = new QCheckBox("Show Grid", displayGroup_);
    showGridCheckbox_->setChecked(true);
    showLabelsCheckbox_ = new QCheckBox("Show Labels", displayGroup_);
    showLabelsCheckbox_->setChecked(true);
    aspectRatioCheckbox_ = new QCheckBox("Preserve Aspect Ratio", displayGroup_);
    aspectRatioCheckbox_->setChecked(true);
    
    displayLayout->addWidget(showGridCheckbox_);
    displayLayout->addWidget(showLabelsCheckbox_);
    displayLayout->addWidget(aspectRatioCheckbox_);
    
    connect(showGridCheckbox_, &QCheckBox::toggled, this, &MainWindow::OnGridToggled);
    connect(showLabelsCheckbox_, &QCheckBox::toggled, this, &MainWindow::OnLabelsToggled);
    connect(aspectRatioCheckbox_, &QCheckBox::toggled, this, &MainWindow::OnAspectRatioToggled);
    
    sidebarLayout->addWidget(displayGroup_);

    // === Animation Group ===
    animationGroup_ = new QGroupBox("Animation", sidebarWidget_);
    QVBoxLayout* animLayout = new QVBoxLayout(animationGroup_);
    
    // Animation buttons row
    QHBoxLayout* animButtonLayout = new QHBoxLayout();
    startButton_ = new QPushButton("Start", animationGroup_);
    pauseButton_ = new QPushButton("Pause", animationGroup_);
    resetAnimButton_ = new QPushButton("Reset", animationGroup_);
    pauseButton_->setEnabled(false);
    
    animButtonLayout->addWidget(startButton_);
    animButtonLayout->addWidget(pauseButton_);
    animButtonLayout->addWidget(resetAnimButton_);
    animLayout->addLayout(animButtonLayout);
    
    // Speed input
    QHBoxLayout* speedLayout = new QHBoxLayout();
    speedLayout->addWidget(new QLabel("Speed (pts/sec):", animationGroup_));
    speedInput_ = new QLineEdit("10", animationGroup_);
    speedInput_->setFixedWidth(60);
    speedInput_->setValidator(new QIntValidator(1, 1000, this));
    speedLayout->addWidget(speedInput_);
    speedLayout->addStretch();
    animLayout->addLayout(speedLayout);
    
    // Frame and T value display
    frameLabel_ = new QLabel("Frame: 0 / 0", animationGroup_);
    tValueLabel_ = new QLabel("t = 0.0000", animationGroup_);
    animLayout->addWidget(frameLabel_);
    animLayout->addWidget(tValueLabel_);
    
    connect(startButton_, &QPushButton::clicked, this, &MainWindow::OnStartAnimation);
    connect(pauseButton_, &QPushButton::clicked, this, &MainWindow::OnPauseAnimation);
    connect(resetAnimButton_, &QPushButton::clicked, this, &MainWindow::OnResetAnimation);
    connect(speedInput_, &QLineEdit::editingFinished, this, &MainWindow::OnAnimationSpeedChanged);
    
    sidebarLayout->addWidget(animationGroup_);

    // === Legend Group ===
    legendGroup_ = new QGroupBox("Curves", sidebarWidget_);
    legendLayout_ = new QVBoxLayout(legendGroup_);
    legendLayout_->setContentsMargins(5, 5, 5, 5);
    legendLayout_->setSpacing(3);
    
    // Add stretch to push entries to top
    legendLayout_->addStretch();
    
    sidebarLayout->addWidget(legendGroup_);

    // === Info Group ===
    infoGroup_ = new QGroupBox("Curve Info", sidebarWidget_);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup_);
    
    infoDisplay_ = new QTextEdit(infoGroup_);
    infoDisplay_->setReadOnly(true);
    infoDisplay_->setMaximumHeight(150);
    infoLayout->addWidget(infoDisplay_);
    
    sidebarLayout->addWidget(infoGroup_);

    // Add stretch at bottom
    sidebarLayout->addStretch();
}

LegendEntry MainWindow::CreateLegendEntry(const QString& name, const QColor& color, int index) {
    LegendEntry entry;
    entry.curveIndex = index;
    
    // Create horizontal layout container
    QWidget* container = new QWidget(legendGroup_);
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    
    // Checkbox
    entry.checkbox = new QCheckBox(container);
    entry.checkbox->setChecked(true);
    entry.checkbox->setProperty("curveIndex", index);
    connect(entry.checkbox, &QCheckBox::toggled, this, &MainWindow::OnLegendCheckboxToggled);
    layout->addWidget(entry.checkbox);
    
    // Color swatch
    entry.colorSwatch = new QFrame(container);
    entry.colorSwatch->setFixedSize(16, 16);
    entry.colorSwatch->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(color.name()));
    layout->addWidget(entry.colorSwatch);
    
    // Name label
    entry.nameLabel = new QLabel(name, container);
    entry.nameLabel->setStyleSheet(QString("color: %1;").arg(color.name()));
    layout->addWidget(entry.nameLabel, 1);  // Stretch
    
    // Insert before the stretch at the end
    legendLayout_->insertWidget(legendLayout_->count() - 1, container);
    
    return entry;
}

void MainWindow::LoadFile() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Load Parametric Curve 2D Data",
        QString::fromStdString("../../data/ParametricCurve2D"),
        "Data Files (*.txt);;All Files (*.*)"
    );

    if (!filename.isEmpty()) {
        LoadCurveFile(filename, curveCounter_);
    }
}

void MainWindow::LoadCurveFile(const QString& filename, int index) {
    try {
        auto curve = MMLFileParser::ParseFile(filename.toStdString(), index);
        
        // Get color before adding
        QColor color = glWidget_->GetCurveColor(glWidget_->GetCurves().size());
        QString curveName = QString::fromStdString(curve->GetTitle());
        
        glWidget_->AddCurve(std::move(curve));
        loadedFilenames_.push_back(filename.toStdString());
        
        // Create legend entry
        legendEntries_.push_back(CreateLegendEntry(curveName, color, curveCounter_));
        
        curveCounter_++;
        
        UpdateInfoDisplay();
        UpdateAnimationUI();
        statusBar_->showMessage("Loaded: " + filename, 3000);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Failed to load file:\n%1").arg(e.what()));
        statusBar_->showMessage("Error loading file", 3000);
    }
}

void MainWindow::ResetView() {
    glWidget_->ResetView();
    statusBar_->showMessage("View reset", 2000);
}

void MainWindow::ClearAll() {
    glWidget_->ClearCurves();
    loadedFilenames_.clear();
    curveCounter_ = 0;
    
    // Clear legend entries
    for (auto& entry : legendEntries_) {
        // Find and delete the parent widget
        if (entry.checkbox) {
            QWidget* container = entry.checkbox->parentWidget();
            if (container) {
                legendLayout_->removeWidget(container);
                delete container;
            }
        }
    }
    legendEntries_.clear();
    
    UpdateInfoDisplay();
    UpdateAnimationUI();
    statusBar_->showMessage("All curves cleared", 2000);
}

void MainWindow::UpdateLegend() {
    // This is now handled incrementally in LoadCurveFile
}

void MainWindow::UpdateInfoDisplay() {
    QString html = "<table border='0' cellpadding='2' style='font-size:9pt;'>";
    html += "<tr><th align='left'>Curve</th><th>Pts</th><th>T Range</th></tr>";

    const auto& curves = glWidget_->GetCurves();
    for (size_t i = 0; i < curves.size(); i++) {
        const auto& curve = curves[i];
        Color curveColor = curve->GetColor();
        QColor color(static_cast<int>(curveColor.r * 255),
                     static_cast<int>(curveColor.g * 255),
                     static_cast<int>(curveColor.b * 255));
        
        QString colorStr = QString("rgb(%1,%2,%3)")
            .arg(color.red())
            .arg(color.green())
            .arg(color.blue());

        std::ostringstream tRange;
        tRange << std::fixed << std::setprecision(2) 
               << "[" << curve->GetMinT() << ", " << curve->GetMaxT() << "]";

        html += QString("<tr>"
                        "<td><span style='color:%1'>●</span> %2</td>"
                        "<td align='center'>%3</td>"
                        "<td>%4</td>"
                        "</tr>")
                    .arg(colorStr)
                    .arg(QString::fromStdString(curve->GetTitle()).left(15))
                    .arg(curve->GetNumPoints())
                    .arg(QString::fromStdString(tRange.str()));
    }

    html += "</table>";
    infoDisplay_->setHtml(html);
}

void MainWindow::UpdateAnimationUI() {
    int currentFrame = glWidget_->GetCurrentAnimationFrame();
    int maxFrames = glWidget_->GetMaxAnimationFrames();
    
    frameLabel_->setText(QString("Frame: %1 / %2").arg(currentFrame).arg(maxFrames));
    
    // Calculate t value from current frame
    const auto& curves = glWidget_->GetCurves();
    if (!curves.empty() && maxFrames > 0) {
        double minT = curves[0]->GetMinT();
        double maxT = curves[0]->GetMaxT();
        double t = minT + (maxT - minT) * static_cast<double>(currentFrame) / static_cast<double>(maxFrames - 1);
        tValueLabel_->setText(QString("t = %1").arg(t, 0, 'f', 4));
    } else {
        tValueLabel_->setText("t = 0.0000");
    }
}

// Animation slots
void MainWindow::OnStartAnimation() {
    bool wasRunning = glWidget_->IsAnimationRunning();
    
    if (wasRunning) {
        glWidget_->ResumeAnimation();
    } else {
        glWidget_->StartAnimation();
    }
    
    startButton_->setEnabled(false);
    pauseButton_->setEnabled(true);
    statusBar_->showMessage("Animation started", 2000);
}

void MainWindow::OnPauseAnimation() {
    glWidget_->PauseAnimation();
    startButton_->setEnabled(true);
    pauseButton_->setEnabled(false);
    statusBar_->showMessage("Animation paused", 2000);
}

void MainWindow::OnResetAnimation() {
    glWidget_->ResetAnimation();
    startButton_->setEnabled(true);
    pauseButton_->setEnabled(false);
    UpdateAnimationUI();
    statusBar_->showMessage("Animation reset", 2000);
}

void MainWindow::OnAnimationSpeedChanged() {
    bool ok;
    int speed = speedInput_->text().toInt(&ok);
    if (ok && speed > 0) {
        glWidget_->SetAnimationSpeed(static_cast<double>(speed));
        statusBar_->showMessage(QString("Animation speed: %1 pts/sec").arg(speed), 2000);
    }
}

void MainWindow::OnAnimationFrame() {
    UpdateAnimationUI();
}

// Display settings slots
void MainWindow::OnGridToggled(bool checked) {
    glWidget_->SetGridVisible(checked);
}

void MainWindow::OnLabelsToggled(bool checked) {
    glWidget_->SetLabelsVisible(checked);
}

void MainWindow::OnAspectRatioToggled(bool checked) {
    glWidget_->SetPreserveAspectRatio(checked);
}

// Legend checkbox slot
void MainWindow::OnLegendCheckboxToggled(bool checked) {
    QCheckBox* checkbox = qobject_cast<QCheckBox*>(sender());
    if (checkbox) {
        int index = checkbox->property("curveIndex").toInt();
        glWidget_->SetCurveVisible(index, checked);
    }
}
