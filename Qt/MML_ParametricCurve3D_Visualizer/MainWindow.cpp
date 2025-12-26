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
#include <QScrollArea>
#include <QIntValidator>
#include <sstream>
#include <iomanip>

MainWindow::MainWindow(const std::vector<std::string>& filenames, QWidget *parent)
    : QMainWindow(parent)
    , curveCounter_(0) {
    
    setWindowTitle("MML Parametric Curve 3D Visualizer (Qt + OpenGL)");
    resize(1200, 800);
    
    // Create central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);
    
    // Left side: OpenGL widget
    glWidget_ = new GLWidget(this);
    glWidget_->setMinimumSize(640, 480);
    mainLayout->addWidget(glWidget_, 1);
    
    // Right side: Sidebar
    CreateSidebar();
    mainLayout->addWidget(sidebarWidget_);
    
    // Status bar
    statusLabel_ = new QLabel("Ready - Load a 3D parametric curve file", this);
    statusBar()->addWidget(statusLabel_);
    
    // Set animation callback to update UI
    glWidget_->SetAnimationFrameCallback([this]() {
        OnAnimationFrame();
    });
    
    // Load initial files if provided
    for (const auto& filename : filenames) {
        LoadCurveFile(QString::fromStdString(filename));
    }
    
    UpdateInfoDisplay();
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
    
    // Line width controls
    QHBoxLayout* lineWidthLayout = new QHBoxLayout();
    lineWidthLayout->addWidget(new QLabel("Line width:", displayGroup_));
    lineWidthDecButton_ = new QPushButton("−", displayGroup_);
    lineWidthDecButton_->setFixedWidth(30);
    lineWidthIncButton_ = new QPushButton("+", displayGroup_);
    lineWidthIncButton_->setFixedWidth(30);
    lineWidthLabel_ = new QLabel("2.0", displayGroup_);
    lineWidthLabel_->setFixedWidth(40);
    lineWidthLabel_->setAlignment(Qt::AlignCenter);
    
    lineWidthLayout->addWidget(lineWidthDecButton_);
    lineWidthLayout->addWidget(lineWidthLabel_);
    lineWidthLayout->addWidget(lineWidthIncButton_);
    lineWidthLayout->addStretch();
    displayLayout->addLayout(lineWidthLayout);
    
    // Instructions
    QLabel* instructions = new QLabel(
        "<small><b>Mouse:</b> Left=Rotate, Right=Pan, Wheel=Zoom<br>"
        "<b>Axes:</b> <font color='red'>X</font>, <font color='green'>Y</font>, <font color='blue'>Z</font></small>",
        displayGroup_
    );
    instructions->setWordWrap(true);
    displayLayout->addWidget(instructions);
    
    connect(lineWidthIncButton_, &QPushButton::clicked, this, &MainWindow::OnIncreaseLineWidth);
    connect(lineWidthDecButton_, &QPushButton::clicked, this, &MainWindow::OnDecreaseLineWidth);
    
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
    infoDisplay_->setMaximumHeight(120);
    infoLayout->addWidget(infoDisplay_);
    
    sidebarLayout->addWidget(infoGroup_);
    
    // Add stretch at bottom
    sidebarLayout->addStretch();
}

LegendEntry MainWindow::CreateLegendEntry(const QString& name, const Color& color, int index) {
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
    QColor qColor(static_cast<int>(color.r * 255), 
                  static_cast<int>(color.g * 255), 
                  static_cast<int>(color.b * 255));
    entry.colorSwatch->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(qColor.name()));
    layout->addWidget(entry.colorSwatch);
    
    // Name label
    entry.nameLabel = new QLabel(name, container);
    layout->addWidget(entry.nameLabel, 1);  // Stretch
    
    // Insert before the stretch at the end
    legendLayout_->insertWidget(legendLayout_->count() - 1, container);
    
    return entry;
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
        
        QString curveName = QString::fromStdString(curve->GetName());
        
        glWidget_->AddCurve(std::move(curve));
        loadedFilenames_.push_back(filename.toStdString());
        
        // Get the color that was assigned
        Color color = glWidget_->GetCurves().back()->GetColor();
        
        // Create legend entry
        legendEntries_.push_back(CreateLegendEntry(curveName, color, curveCounter_));
        
        curveCounter_++;
        
        UpdateInfoDisplay();
        UpdateAnimationUI();
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

void MainWindow::ClearAll() {
    glWidget_->ClearCurves();
    loadedFilenames_.clear();
    curveCounter_ = 0;
    
    // Clear legend entries
    for (auto& entry : legendEntries_) {
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
    statusLabel_->setText("All curves cleared");
}

void MainWindow::UpdateInfoDisplay() {
    if (loadedFilenames_.empty()) {
        infoDisplay_->setHtml(
            "<p><i>No curves loaded.</i></p>"
            "<p>Click 'Load Curve...' to open a file.</p>"
        );
        return;
    }
    
    const auto& curves = glWidget_->GetCurves();
    
    QString html = "<table border='0' cellpadding='2' style='font-size:9pt;'>";
    html += "<tr><th>Curve</th><th>Pts</th><th>T Range</th></tr>";
    
    for (size_t i = 0; i < curves.size(); ++i) {
        const auto& curve = curves[i];
        Color color = curve->GetColor();
        QColor qColor(static_cast<int>(color.r * 255),
                      static_cast<int>(color.g * 255),
                      static_cast<int>(color.b * 255));
        
        std::ostringstream tRange;
        tRange << std::fixed << std::setprecision(2) 
               << "[" << curve->GetT1() << ", " << curve->GetT2() << "]";
        
        html += QString("<tr>"
                        "<td><span style='color:%1'>●</span> %2</td>"
                        "<td align='center'>%3</td>"
                        "<td>%4</td>"
                        "</tr>")
                    .arg(qColor.name())
                    .arg(QString::fromStdString(curve->GetName()).left(12))
                    .arg(curve->GetNumPoints())
                    .arg(QString::fromStdString(tRange.str()));
    }
    
    html += "</table>";
    infoDisplay_->setHtml(html);
}

void MainWindow::UpdateAnimationUI() {
    size_t currentFrame = glWidget_->GetCurrentAnimationFrame();
    size_t maxFrames = glWidget_->GetMaxAnimationFrames();
    
    frameLabel_->setText(QString("Frame: %1 / %2").arg(currentFrame).arg(maxFrames));
    
    // Calculate t value from current frame
    const auto& curves = glWidget_->GetCurves();
    if (!curves.empty() && maxFrames > 0) {
        double minT = curves[0]->GetT1();
        double maxT = curves[0]->GetT2();
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
    statusLabel_->setText("Animation started");
}

void MainWindow::OnPauseAnimation() {
    glWidget_->PauseAnimation();
    startButton_->setEnabled(true);
    pauseButton_->setEnabled(false);
    statusLabel_->setText("Animation paused");
}

void MainWindow::OnResetAnimation() {
    glWidget_->ResetAnimation();
    startButton_->setEnabled(true);
    pauseButton_->setEnabled(false);
    UpdateAnimationUI();
    statusLabel_->setText("Animation reset");
}

void MainWindow::OnAnimationSpeedChanged() {
    bool ok;
    int speed = speedInput_->text().toInt(&ok);
    if (ok && speed > 0) {
        glWidget_->SetAnimationSpeed(static_cast<double>(speed));
        statusLabel_->setText(QString("Animation speed: %1 pts/sec").arg(speed));
    }
}

void MainWindow::OnAnimationFrame() {
    UpdateAnimationUI();
}

// Display settings slots
void MainWindow::OnIncreaseLineWidth() {
    glWidget_->IncreaseLineWidth();
    lineWidthLabel_->setText(QString::number(glWidget_->GetLineWidth(), 'f', 1));
}

void MainWindow::OnDecreaseLineWidth() {
    glWidget_->DecreaseLineWidth();
    lineWidthLabel_->setText(QString::number(glWidget_->GetLineWidth(), 'f', 1));
}

// Legend checkbox slot
void MainWindow::OnLegendCheckboxToggled(bool checked) {
    QCheckBox* checkbox = qobject_cast<QCheckBox*>(sender());
    if (checkbox) {
        int index = checkbox->property("curveIndex").toInt();
        glWidget_->SetCurveVisible(index, checked);
    }
}
