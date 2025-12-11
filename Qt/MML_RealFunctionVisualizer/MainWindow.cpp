#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <sstream>
#include <iomanip>

MainWindow::MainWindow(const std::vector<std::string>& filenames, QWidget *parent)
    : QMainWindow(parent)
    , functionCounter_(0)
{
    setWindowTitle("MML Real Function Visualizer (Qt + OpenGL)");
    resize(1200, 700);

    // Create central widget and main layout
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Create splitter for GL widget and info panel
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    // Create GL widget
    glWidget_ = new GLWidget(this);
    splitter->addWidget(glWidget_);

    // Create right panel
    QWidget* rightPanel = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

    // Create buttons
    loadButton_ = new QPushButton("Load Function", this);
    resetButton_ = new QPushButton("Reset View", this);

    rightLayout->addWidget(loadButton_);
    rightLayout->addWidget(resetButton_);

    // Create info display
    QLabel* infoLabel = new QLabel("Loaded Functions:", this);
    rightLayout->addWidget(infoLabel);

    infoDisplay_ = new QTextEdit(this);
    infoDisplay_->setReadOnly(true);
    infoDisplay_->setMaximumHeight(300);
    rightLayout->addWidget(infoDisplay_);

    // Add instructions
    QLabel* instructionsLabel = new QLabel("Controls:", this);
    rightLayout->addWidget(instructionsLabel);

    QTextEdit* instructions = new QTextEdit(this);
    instructions->setReadOnly(true);
    instructions->setMaximumHeight(150);
    instructions->setHtml(
        "<b>Mouse Controls:</b><br>"
        "• Left/Right button + drag: Pan view<br>"
        "• Mouse wheel: Zoom in/out<br>"
        "<br>"
        "<b>Buttons:</b><br>"
        "• Load Function: Add new function<br>"
        "• Reset View: Fit all functions"
    );
    rightLayout->addWidget(instructions);

    rightLayout->addStretch();

    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);

    // Create status bar
    statusBar_ = new QStatusBar(this);
    setStatusBar(statusBar_);
    statusBar_->showMessage("Ready");

    // Connect signals
    connect(loadButton_, &QPushButton::clicked, this, &MainWindow::LoadFile);
    connect(resetButton_, &QPushButton::clicked, this, &MainWindow::ResetView);

    // Load initial files
    for (const auto& filename : filenames) {
        LoadFunctionFile(QString::fromStdString(filename));
    }
}

MainWindow::~MainWindow() {
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
        Color color = GetColorForIndex(functionCounter_);
        auto func = MMLFileParser::ParseRealFunction(filename.toStdString(), color);
        
        glWidget_->AddFunction(std::move(func));
        loadedFilenames_.push_back(filename.toStdString());
        functionCounter_++;
        
        UpdateInfoDisplay();
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

void MainWindow::UpdateInfoDisplay() {
    QString html = "<table border='0' cellpadding='3'>";
    html += "<tr><th align='left'>Function</th><th>Points</th><th>X Range</th><th>Y Range</th></tr>";

    const auto& functions = glWidget_->GetFunctions();
    for (size_t i = 0; i < functions.size(); i++) {
        const auto& func = functions[i];
        const Color& color = func->GetColor();
        
        QString colorStr = QString("rgb(%1,%2,%3)")
            .arg(static_cast<int>(color.r * 255))
            .arg(static_cast<int>(color.g * 255))
            .arg(static_cast<int>(color.b * 255));

        std::ostringstream xRange, yRange;
        xRange << std::fixed << std::setprecision(2) 
               << "[" << func->GetMinX() << ", " << func->GetMaxX() << "]";
        yRange << std::fixed << std::setprecision(2)
               << "[" << func->GetMinY() << ", " << func->GetMaxY() << "]";

        html += QString("<tr>"
                       "<td><span style='color:%1'>■</span> %2</td>"
                       "<td align='center'>%3</td>"
                       "<td>%4</td>"
                       "<td>%5</td>"
                       "</tr>")
            .arg(colorStr)
            .arg(QString::fromStdString(func->GetTitle()))
            .arg(func->GetNumPoints())
            .arg(QString::fromStdString(xRange.str()))
            .arg(QString::fromStdString(yRange.str()));
    }

    html += "</table>";
    infoDisplay_->setHtml(html);
}

Color MainWindow::GetColorForIndex(int index) {
    static const Color colors[] = {
        Color(1.0f, 0.0f, 0.0f),   // Red
        Color(0.0f, 0.5f, 1.0f),   // Blue
        Color(0.0f, 0.7f, 0.0f),   // Green
        Color(1.0f, 0.5f, 0.0f),   // Orange
        Color(0.6f, 0.0f, 0.8f),   // Purple
        Color(0.0f, 0.8f, 0.8f),   // Cyan
        Color(1.0f, 0.8f, 0.0f),   // Yellow
        Color(1.0f, 0.0f, 0.5f),   // Pink
    };
    
    return colors[index % 8];
}
