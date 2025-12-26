#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFrame>
#include <QScrollArea>
#include <QPalette>

// Color palette matching WPF version
const std::vector<Color> MainWindow::colorPalette_ = {
    Color(0.0f, 0.0f, 0.0f),       // Black
    Color(0.0f, 0.0f, 1.0f),       // Blue
    Color(1.0f, 0.0f, 0.0f),       // Red
    Color(0.0f, 0.5f, 0.0f),       // Green
    Color(1.0f, 0.65f, 0.0f),      // Orange
    Color(0.5f, 0.0f, 0.5f),       // Purple
    Color(0.65f, 0.16f, 0.16f),    // Brown
    Color(0.0f, 1.0f, 1.0f),       // Cyan
    Color(1.0f, 0.0f, 1.0f),       // Magenta
    Color(0.5f, 0.5f, 0.5f),       // Gray
    Color(1.0f, 1.0f, 0.0f)        // Yellow
};

MainWindow::MainWindow(const std::vector<std::string>& filenames, QWidget *parent)
    : QMainWindow(parent)
    , functionCounter_(0)
    , graphTitle_("Real Function Visualizer")
{
    setWindowTitle("MML Real Function Visualizer (Qt + OpenGL)");
    resize(1200, 850);

    // Create central widget and main layout
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // Create GL widget (takes most space)
    glWidget_ = new GLWidget(this);
    glWidget_->setMinimumSize(600, 400);
    mainLayout->addWidget(glWidget_, 1);  // Stretch factor 1
    
    // Connect signals
    connect(glWidget_, &GLWidget::boundsChanged, this, &MainWindow::OnBoundsChanged);

    // Create sidebar
    sidebarWidget_ = new QWidget(this);
    sidebarWidget_->setFixedWidth(230);
    CreateSidebar(sidebarWidget_);
    mainLayout->addWidget(sidebarWidget_, 0);  // No stretch

    // Create status bar
    statusBar_ = new QStatusBar(this);
    setStatusBar(statusBar_);
    statusBar_->showMessage("Ready");

    // Load initial files
    for (const auto& filename : filenames) {
        LoadFunctionFile(QString::fromStdString(filename));
    }
}

MainWindow::~MainWindow() {
}

void MainWindow::CreateSidebar(QWidget* parent) {
    QVBoxLayout* sidebarLayout = new QVBoxLayout(parent);
    sidebarLayout->setContentsMargins(5, 5, 5, 5);
    sidebarLayout->setSpacing(10);
    
    // ===== TITLE SECTION =====
    QLabel* titleLabel = new QLabel("Graph Title:", parent);
    titleLabel->setStyleSheet("font-weight: bold;");
    sidebarLayout->addWidget(titleLabel);
    
    titleEdit_ = new QLineEdit(graphTitle_, parent);
    connect(titleEdit_, &QLineEdit::editingFinished, this, &MainWindow::OnTitleChanged);
    sidebarLayout->addWidget(titleEdit_);
    
    // ===== LEGEND SECTION =====
    QLabel* legendLabel = new QLabel("Legend:", parent);
    legendLabel->setStyleSheet("font-weight: bold;");
    sidebarLayout->addWidget(legendLabel);
    
    legendScrollArea_ = new QScrollArea(parent);
    legendScrollArea_->setWidgetResizable(true);
    legendScrollArea_->setMinimumHeight(250);
    legendScrollArea_->setMaximumHeight(350);
    legendScrollArea_->setFrameShape(QFrame::StyledPanel);
    
    legendContent_ = new QWidget();
    legendLayout_ = new QVBoxLayout(legendContent_);
    legendLayout_->setContentsMargins(5, 5, 5, 5);
    legendLayout_->setSpacing(2);
    legendLayout_->addStretch();
    
    legendScrollArea_->setWidget(legendContent_);
    sidebarLayout->addWidget(legendScrollArea_);
    
    // ===== SETTINGS SECTION =====
    QLabel* settingsLabel = new QLabel("Graph Settings:", parent);
    settingsLabel->setStyleSheet("font-weight: bold;");
    sidebarLayout->addWidget(settingsLabel);
    
    gridCheckbox_ = new QCheckBox("Show Grid", parent);
    gridCheckbox_->setChecked(true);
    connect(gridCheckbox_, &QCheckBox::toggled, this, &MainWindow::OnGridToggled);
    sidebarLayout->addWidget(gridCheckbox_);
    
    labelsCheckbox_ = new QCheckBox("Show Axis Labels", parent);
    labelsCheckbox_->setChecked(true);
    connect(labelsCheckbox_, &QCheckBox::toggled, this, &MainWindow::OnLabelsToggled);
    sidebarLayout->addWidget(labelsCheckbox_);
    
    aspectRatioCheckbox_ = new QCheckBox("Preserve Aspect Ratio", parent);
    aspectRatioCheckbox_->setChecked(false);
    connect(aspectRatioCheckbox_, &QCheckBox::toggled, this, &MainWindow::OnAspectRatioToggled);
    sidebarLayout->addWidget(aspectRatioCheckbox_);
    
    sidebarLayout->addSpacing(10);
    
    // ===== BUTTONS SECTION =====
    loadButton_ = new QPushButton("Load File...", parent);
    loadButton_->setStyleSheet("QPushButton { background-color: #4682B4; color: white; font-weight: bold; padding: 8px; }");
    connect(loadButton_, &QPushButton::clicked, this, &MainWindow::LoadFile);
    sidebarLayout->addWidget(loadButton_);
    
    clearButton_ = new QPushButton("Clear All", parent);
    clearButton_->setStyleSheet("QPushButton { background-color: #B44646; color: white; padding: 8px; }");
    connect(clearButton_, &QPushButton::clicked, this, &MainWindow::ClearAll);
    sidebarLayout->addWidget(clearButton_);
    
    resetButton_ = new QPushButton("Reset View", parent);
    connect(resetButton_, &QPushButton::clicked, this, &MainWindow::ResetView);
    sidebarLayout->addWidget(resetButton_);
    
    sidebarLayout->addStretch();
    
    // ===== INSTRUCTIONS =====
    QLabel* controlsLabel = new QLabel("Controls:", parent);
    controlsLabel->setStyleSheet("font-weight: bold;");
    sidebarLayout->addWidget(controlsLabel);
    
    QLabel* instructions = new QLabel(
        "• Left/Right drag: Pan view\n"
        "• Mouse wheel: Zoom\n"
        "• Checkbox: Toggle visibility",
        parent);
    instructions->setWordWrap(true);
    instructions->setStyleSheet("color: #666;");
    sidebarLayout->addWidget(instructions);
}

void MainWindow::LoadFile() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Load Real Function Data",
        QString::fromStdString("../../WPF/MML_RealFunctionVisualizer/data"),
        "Data Files (*.txt);;All Files (*.*)"
    );

    if (!filename.isEmpty()) {
        LoadFunctionFile(filename);
    }
}

void MainWindow::LoadFunctionFile(const QString& filename) {
    try {
        auto func = MMLFileParser::ParseFile(filename.toStdString(), functionCounter_);
        
        if (func) {
            // Assign colors to functions
            if (func->GetDimension() == 1) {
                auto* singleFunc = dynamic_cast<LoadedRealFunction*>(func.get());
                if (singleFunc) {
                    singleFunc->SetColor(GetColorForIndex(functionCounter_));
                }
            } else {
                auto* multiFunc = dynamic_cast<MultiLoadedFunction*>(func.get());
                if (multiFunc) {
                    for (int i = 0; i < multiFunc->GetDimension(); ++i) {
                        multiFunc->SetFunctionColor(i, GetColorForIndex(i));
                    }
                }
            }
            
            // Update title from first loaded function if still default
            if (graphTitle_ == "Real Function Visualizer" && !func->GetTitle().empty()) {
                graphTitle_ = QString::fromStdString(func->GetTitle());
                titleEdit_->setText(graphTitle_);
                setWindowTitle("MML Real Function Visualizer - " + graphTitle_);
            }
            
            glWidget_->AddFunction(std::move(func));
            loadedFilenames_.push_back(filename.toStdString());
            functionCounter_++;
            
            UpdateLegend();
            statusBar_->showMessage("Loaded: " + filename, 3000);
        }
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Failed to load file:\n%1").arg(e.what()));
        statusBar_->showMessage("Error loading file", 3000);
    }
}

void MainWindow::ClearAll() {
    glWidget_->ClearFunctions();
    loadedFilenames_.clear();
    functionCounter_ = 0;
    graphTitle_ = "Real Function Visualizer";
    titleEdit_->setText(graphTitle_);
    setWindowTitle("MML Real Function Visualizer (Qt + OpenGL)");
    UpdateLegend();
    statusBar_->showMessage("Cleared all functions", 2000);
}

void MainWindow::ResetView() {
    glWidget_->ResetView();
    statusBar_->showMessage("View reset", 2000);
}

void MainWindow::OnTitleChanged() {
    graphTitle_ = titleEdit_->text();
    setWindowTitle("MML Real Function Visualizer - " + graphTitle_);
}

void MainWindow::OnGridToggled(bool checked) {
    glWidget_->SetGridVisible(checked);
}

void MainWindow::OnLabelsToggled(bool checked) {
    glWidget_->SetLabelsVisible(checked);
}

void MainWindow::OnAspectRatioToggled(bool checked) {
    glWidget_->SetPreserveAspectRatio(checked);
}

void MainWindow::OnLegendCheckboxToggled(bool checked) {
    QCheckBox* checkbox = qobject_cast<QCheckBox*>(sender());
    if (!checkbox) return;
    
    // Find which legend entry this checkbox belongs to
    for (const auto& entry : legendEntries_) {
        if (entry.checkbox == checkbox) {
            auto& functions = glWidget_->GetFunctions();
            
            if (entry.functionIndex < static_cast<int>(functions.size())) {
                auto& func = functions[entry.functionIndex];
                
                if (entry.subFunctionIndex >= 0) {
                    // Multi-function sub-entry
                    func->SetFunctionVisible(entry.subFunctionIndex, checked);
                } else {
                    // Single function
                    func->SetVisible(checked);
                }
                
                glWidget_->RecalculateBounds();
                glWidget_->update();
            }
            break;
        }
    }
}

void MainWindow::OnBoundsChanged() {
    // Could update status bar with current bounds if desired
}

void MainWindow::UpdateLegend() {
    // Clear all widgets from the legend layout
    // This properly deletes all child widgets
    QLayoutItem* item;
    while ((item = legendLayout_->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    legendEntries_.clear();
    
    const auto& functions = glWidget_->GetFunctions();
    int colorIndex = 0;
    
    for (size_t funcIdx = 0; funcIdx < functions.size(); ++funcIdx) {
        const auto& func = functions[funcIdx];
        
        if (func->GetDimension() == 1) {
            // Single function
            QWidget* entryWidget = new QWidget();
            QHBoxLayout* entryLayout = new QHBoxLayout(entryWidget);
            entryLayout->setContentsMargins(0, 2, 0, 2);
            entryLayout->setSpacing(5);
            
            // Checkbox
            QCheckBox* checkbox = new QCheckBox();
            checkbox->setChecked(func->IsVisible());
            connect(checkbox, &QCheckBox::toggled, this, &MainWindow::OnLegendCheckboxToggled);
            entryLayout->addWidget(checkbox);
            
            // Color box
            QWidget* colorBox = new QWidget();
            colorBox->setFixedSize(20, 15);
            Color c = func->GetFunctionColor(0);
            QString colorStyle = QString("background-color: rgb(%1,%2,%3); border: 1px solid black;")
                .arg(static_cast<int>(c.r * 255))
                .arg(static_cast<int>(c.g * 255))
                .arg(static_cast<int>(c.b * 255));
            colorBox->setStyleSheet(colorStyle);
            entryLayout->addWidget(colorBox);
            
            // Label
            QLabel* label = new QLabel(QString::fromStdString(func->GetTitle()));
            label->setWordWrap(true);
            entryLayout->addWidget(label, 1);
            
            legendLayout_->addWidget(entryWidget);
            
            LegendEntry entry;
            entry.checkbox = checkbox;
            entry.colorBox = colorBox;
            entry.functionIndex = static_cast<int>(funcIdx);
            entry.subFunctionIndex = -1;
            legendEntries_.push_back(entry);
            
            colorIndex++;
        } else {
            // Multi-function - add entry for each sub-function
            auto* multiFunc = dynamic_cast<const MultiLoadedFunction*>(func.get());
            if (multiFunc) {
                for (int i = 0; i < func->GetDimension(); ++i) {
                    QWidget* entryWidget = new QWidget();
                    QHBoxLayout* entryLayout = new QHBoxLayout(entryWidget);
                    entryLayout->setContentsMargins(0, 2, 0, 2);
                    entryLayout->setSpacing(5);
                    
                    // Checkbox
                    QCheckBox* checkbox = new QCheckBox();
                    checkbox->setChecked(multiFunc->IsFunctionVisible(i));
                    connect(checkbox, &QCheckBox::toggled, this, &MainWindow::OnLegendCheckboxToggled);
                    entryLayout->addWidget(checkbox);
                    
                    // Color box
                    QWidget* colorBox = new QWidget();
                    colorBox->setFixedSize(20, 15);
                    Color c = multiFunc->GetFunctionColor(i);
                    QString colorStyle = QString("background-color: rgb(%1,%2,%3); border: 1px solid black;")
                        .arg(static_cast<int>(c.r * 255))
                        .arg(static_cast<int>(c.g * 255))
                        .arg(static_cast<int>(c.b * 255));
                    colorBox->setStyleSheet(colorStyle);
                    entryLayout->addWidget(colorBox);
                    
                    // Label
                    QLabel* label = new QLabel(QString::fromStdString(multiFunc->GetFunctionTitle(i)));
                    label->setWordWrap(true);
                    entryLayout->addWidget(label, 1);
                    
                    legendLayout_->addWidget(entryWidget);
                    
                    LegendEntry entry;
                    entry.checkbox = checkbox;
                    entry.colorBox = colorBox;
                    entry.functionIndex = static_cast<int>(funcIdx);
                    entry.subFunctionIndex = i;
                    legendEntries_.push_back(entry);
                }
            }
        }
    }
    
    // Add stretch at the end
    legendLayout_->addStretch();
}

Color MainWindow::GetColorForIndex(int index) {
    return colorPalette_[index % colorPalette_.size()];
}
