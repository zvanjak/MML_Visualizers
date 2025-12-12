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
    , curveCounter_(0)
{
    setWindowTitle("MML Parametric Curve 2D Visualizer (Qt + OpenGL)");
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
    loadButton_ = new QPushButton("Load Curve", this);
    resetButton_ = new QPushButton("Reset View", this);

    rightLayout->addWidget(loadButton_);
    rightLayout->addWidget(resetButton_);

    // Create curve legend
    QLabel* legendLabel = new QLabel("Loaded Curves:", this);
    rightLayout->addWidget(legendLabel);

    legendList_ = new QListWidget(this);
    legendList_->setMaximumHeight(200);
    rightLayout->addWidget(legendList_);

    // Create info display
    QLabel* infoLabel = new QLabel("Curve Info:", this);
    rightLayout->addWidget(infoLabel);

    infoDisplay_ = new QTextEdit(this);
    infoDisplay_->setReadOnly(true);
    infoDisplay_->setMaximumHeight(200);
    rightLayout->addWidget(infoDisplay_);

    // Add instructions
    QLabel* instructionsLabel = new QLabel("Controls:", this);
    rightLayout->addWidget(instructionsLabel);

    QTextEdit* instructions = new QTextEdit(this);
    instructions->setReadOnly(true);
    instructions->setMaximumHeight(150);
    instructions->setHtml(
        "<b>Mouse Controls:</b><br>"
        "• Left button + drag: Pan view<br>"
        "• Mouse wheel: Zoom in/out<br>"
        "<br>"
        "<b>Buttons:</b><br>"
        "• Load Curve: Add new parametric curve<br>"
        "• Reset View: Fit all curves"
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

    // Load initial files from command line arguments
    for (size_t i = 0; i < filenames.size(); i++) {
        LoadCurveFile(QString::fromStdString(filenames[i]), i);
    }
}

MainWindow::~MainWindow() {
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
        
        glWidget_->AddCurve(std::move(curve));
        loadedFilenames_.push_back(filename.toStdString());
        curveCounter_++;
        
        UpdateInfoDisplay();
        UpdateLegend();
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

void MainWindow::UpdateLegend() {
    legendList_->clear();
    
    const auto& curves = glWidget_->GetCurves();
    for (size_t i = 0; i < curves.size() && i < 10; i++) {
        const auto& curve = curves[i];
        QColor color = glWidget_->GetCurveColor(i);
        
        // Create colored item
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(curve->GetTitle()));
        item->setForeground(QBrush(color));
        legendList_->addItem(item);
    }
}

void MainWindow::UpdateInfoDisplay() {
    QString html = "<table border='0' cellpadding='3'>";
    html += "<tr><th align='left'>Curve</th><th>Points</th><th>X Range</th><th>Y Range</th></tr>";

    const auto& curves = glWidget_->GetCurves();
    for (size_t i = 0; i < curves.size(); i++) {
        const auto& curve = curves[i];
        QColor color = glWidget_->GetCurveColor(i);
        
        QString colorStr = QString("rgb(%1,%2,%3)")
            .arg(color.red())
            .arg(color.green())
            .arg(color.blue());

        std::ostringstream xRange, yRange;
        xRange << std::fixed << std::setprecision(2) 
               << "[" << curve->GetMinX() << ", " << curve->GetMaxX() << "]";
        yRange << std::fixed << std::setprecision(2)
               << "[" << curve->GetMinY() << ", " << curve->GetMaxY() << "]";

        html += QString("<tr>"
                        "<td><span style='color:%1'>●</span> %2</td>"
                        "<td align='center'>%3</td>"
                        "<td>%4</td>"
                        "<td>%5</td>"
                        "</tr>")
                    .arg(colorStr)
                    .arg(QString::fromStdString(curve->GetTitle()).left(20))
                    .arg(curve->GetNumPoints())
                    .arg(QString::fromStdString(xRange.str()))
                    .arg(QString::fromStdString(yRange.str()));
    }

    html += "</table>";
    infoDisplay_->setHtml(html);
}
