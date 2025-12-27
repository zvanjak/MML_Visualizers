#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <cmath>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("MML Scalar Function 2D Visualizer");
    resize(1200, 800);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // === OpenGL Widget (main area) ===
    m_glWidget = new GLWidget(this);
    m_glWidget->setMinimumSize(600, 400);
    mainLayout->addWidget(m_glWidget, 1);

    // === Right Sidebar ===
    QWidget* sidebar = new QWidget(this);
    sidebar->setFixedWidth(220);
    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(8);

    // --- Graph Title Panel ---
    QGroupBox* titleGroup = new QGroupBox("Graph Title", sidebar);
    QVBoxLayout* titleLayout = new QVBoxLayout(titleGroup);
    
    m_titleLabel = new QLabel("No data loaded", titleGroup);
    m_titleLabel->setWordWrap(true);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 11px;");
    titleLayout->addWidget(m_titleLabel);
    
    m_titleEdit = new QLineEdit(titleGroup);
    m_titleEdit->setPlaceholderText("Edit title...");
    titleLayout->addWidget(m_titleEdit);
    
    sidebarLayout->addWidget(titleGroup);

    // --- Scaling Panel ---
    QGroupBox* scaleGroup = new QGroupBox("Domain Scaling", sidebar);
    QGridLayout* scaleLayout = new QGridLayout(scaleGroup);
    scaleLayout->setSpacing(4);
    
    // Scale X
    scaleLayout->addWidget(new QLabel("Scale X:"), 0, 0);
    m_scaleXSlider = new QSlider(Qt::Horizontal, scaleGroup);
    m_scaleXSlider->setMinimum(1);
    m_scaleXSlider->setMaximum(200);
    m_scaleXSlider->setValue(100);
    scaleLayout->addWidget(m_scaleXSlider, 0, 1);
    m_scaleXLabel = new QLabel("10.0", scaleGroup);
    m_scaleXLabel->setMinimumWidth(35);
    scaleLayout->addWidget(m_scaleXLabel, 0, 2);
    
    // Scale Y
    scaleLayout->addWidget(new QLabel("Scale Y:"), 1, 0);
    m_scaleYSlider = new QSlider(Qt::Horizontal, scaleGroup);
    m_scaleYSlider->setMinimum(1);
    m_scaleYSlider->setMaximum(200);
    m_scaleYSlider->setValue(100);
    scaleLayout->addWidget(m_scaleYSlider, 1, 1);
    m_scaleYLabel = new QLabel("10.0", scaleGroup);
    m_scaleYLabel->setMinimumWidth(35);
    scaleLayout->addWidget(m_scaleYLabel, 1, 2);
    
    sidebarLayout->addWidget(scaleGroup);

    // --- Display Options Panel ---
    QGroupBox* displayGroup = new QGroupBox("Display Options", sidebar);
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);
    displayLayout->setSpacing(6);
    
    m_showGridCheckBox = new QCheckBox("Show surface points", displayGroup);
    m_showGridCheckBox->setChecked(true);
    displayLayout->addWidget(m_showGridCheckBox);
    
    m_colorCheckBox = new QCheckBox("Color by height", displayGroup);
    m_colorCheckBox->setChecked(true);
    displayLayout->addWidget(m_colorCheckBox);
    
    // Surface color
    QHBoxLayout* surfColorLayout = new QHBoxLayout();
    surfColorLayout->addWidget(new QLabel("Surface:"));
    m_surfaceColorCombo = new QComboBox(displayGroup);
    m_surfaceColorCombo->addItems({"Red", "Blue", "Green", "Orange", "Purple", "Cyan", "Yellow", "Gray"});
    m_surfaceColorCombo->setCurrentIndex(0);
    surfColorLayout->addWidget(m_surfaceColorCombo);
    displayLayout->addLayout(surfColorLayout);
    
    // Points color
    QHBoxLayout* pointColorLayout = new QHBoxLayout();
    pointColorLayout->addWidget(new QLabel("Points:"));
    m_pointsColorCombo = new QComboBox(displayGroup);
    m_pointsColorCombo->addItems({"Blue", "Red", "Green", "Orange", "Purple", "Cyan", "Black", "White"});
    m_pointsColorCombo->setCurrentIndex(0);
    pointColorLayout->addWidget(m_pointsColorCombo);
    displayLayout->addLayout(pointColorLayout);
    
    sidebarLayout->addWidget(displayGroup);

    // --- Graph Bounds Panel ---
    QGroupBox* boundsGroup = new QGroupBox("Graph Bounds", sidebar);
    QGridLayout* boundsLayout = new QGridLayout(boundsGroup);
    boundsLayout->setSpacing(2);
    
    boundsLayout->addWidget(new QLabel("X min:"), 0, 0);
    m_xMinLabel = new QLabel("-", boundsGroup);
    boundsLayout->addWidget(m_xMinLabel, 0, 1);
    
    boundsLayout->addWidget(new QLabel("X max:"), 1, 0);
    m_xMaxLabel = new QLabel("-", boundsGroup);
    boundsLayout->addWidget(m_xMaxLabel, 1, 1);
    
    boundsLayout->addWidget(new QLabel("Y min:"), 2, 0);
    m_yMinLabel = new QLabel("-", boundsGroup);
    boundsLayout->addWidget(m_yMinLabel, 2, 1);
    
    boundsLayout->addWidget(new QLabel("Y max:"), 3, 0);
    m_yMaxLabel = new QLabel("-", boundsGroup);
    boundsLayout->addWidget(m_yMaxLabel, 3, 1);
    
    boundsLayout->addWidget(new QLabel("Z min:"), 4, 0);
    m_zMinLabel = new QLabel("-", boundsGroup);
    boundsLayout->addWidget(m_zMinLabel, 4, 1);
    
    boundsLayout->addWidget(new QLabel("Z max:"), 5, 0);
    m_zMaxLabel = new QLabel("-", boundsGroup);
    boundsLayout->addWidget(m_zMaxLabel, 5, 1);
    
    sidebarLayout->addWidget(boundsGroup);

    // --- Grid Info Panel ---
    QGroupBox* gridGroup = new QGroupBox("Grid Info", sidebar);
    QGridLayout* gridLayout = new QGridLayout(gridGroup);
    gridLayout->setSpacing(2);
    
    gridLayout->addWidget(new QLabel("Points X:"), 0, 0);
    m_pointsXLabel = new QLabel("-", gridGroup);
    gridLayout->addWidget(m_pointsXLabel, 0, 1);
    
    gridLayout->addWidget(new QLabel("Points Y:"), 1, 0);
    m_pointsYLabel = new QLabel("-", gridGroup);
    gridLayout->addWidget(m_pointsYLabel, 1, 1);
    
    gridLayout->addWidget(new QLabel("Total:"), 2, 0);
    m_totalPointsLabel = new QLabel("-", gridGroup);
    gridLayout->addWidget(m_totalPointsLabel, 2, 1);
    
    sidebarLayout->addWidget(gridGroup);

    // --- Information Panel ---
    QGroupBox* infoGroup = new QGroupBox("Controls", sidebar);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup);
    QLabel* infoLabel = new QLabel(
        "• Left drag: Rotate\n"
        "• Wheel: Zoom\n"
        "• X/Y scale: Domain stretch", infoGroup);
    infoLabel->setStyleSheet("font-size: 10px; color: #666;");
    infoLayout->addWidget(infoLabel);
    sidebarLayout->addWidget(infoGroup);

    // --- File Controls ---
    QGroupBox* fileGroup = new QGroupBox("File", sidebar);
    QVBoxLayout* fileLayout = new QVBoxLayout(fileGroup);
    
    m_loadBtn = new QPushButton("Load File...", fileGroup);
    fileLayout->addWidget(m_loadBtn);
    
    m_resetCameraBtn = new QPushButton("Reset Camera", fileGroup);
    fileLayout->addWidget(m_resetCameraBtn);
    
    m_resetBtn = new QPushButton("Clear Data", fileGroup);
    fileLayout->addWidget(m_resetBtn);
    
    sidebarLayout->addWidget(fileGroup);

    // Spacer at bottom
    sidebarLayout->addStretch();

    mainLayout->addWidget(sidebar);

    // === Connect signals ===
    connect(m_scaleXSlider, &QSlider::valueChanged, this, &MainWindow::onScaleXChanged);
    connect(m_scaleYSlider, &QSlider::valueChanged, this, &MainWindow::onScaleYChanged);
    connect(m_showGridCheckBox, &QCheckBox::checkStateChanged, this, &MainWindow::onShowGridPointsChanged);
    connect(m_colorCheckBox, &QCheckBox::checkStateChanged, this, &MainWindow::onColorModeChanged);
    connect(m_surfaceColorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onSurfaceColorChanged);
    connect(m_pointsColorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onPointsColorChanged);
    connect(m_resetCameraBtn, &QPushButton::clicked, this, &MainWindow::onResetCamera);
    connect(m_loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadFile);
    connect(m_resetBtn, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(m_titleEdit, &QLineEdit::textChanged, this, &MainWindow::onTitleChanged);
}

bool MainWindow::loadDataFiles(const QStringList& filenames)
{
    if (filenames.isEmpty()) {
        QMessageBox::warning(this, "Error", "No filename specified.");
        return false;
    }

    QString filename = filenames[0];
    m_currentFilePath = filename;

    if (!MMLFileParser::LoadScalarFunction2D(filename.toStdString(), m_data)) {
        QMessageBox::critical(this, "Error", "Failed to load scalar function data from:\n" + filename);
        return false;
    }

    m_glWidget->setScalarFunction(m_data);
    
    // Update title
    m_titleLabel->setText(QString::fromStdString(m_data.title));
    m_titleEdit->setText(QString::fromStdString(m_data.title));
    
    updateBoundsDisplay();
    updateInfo();

    return true;
}

void MainWindow::onScaleXChanged(int value)
{
    double scale = value / 10.0;
    m_scaleXLabel->setText(QString::number(scale, 'f', 1));
    m_glWidget->setScaleX(scale);
}

void MainWindow::onScaleYChanged(int value)
{
    double scale = value / 10.0;
    m_scaleYLabel->setText(QString::number(scale, 'f', 1));
    m_glWidget->setScaleY(scale);
}

void MainWindow::onShowGridPointsChanged(int state)
{
    m_glWidget->setShowGridPoints(state == Qt::Checked);
}

void MainWindow::onColorModeChanged(int state)
{
    m_glWidget->setColorByHeight(state == Qt::Checked);
    
    // Enable/disable surface color combo based on color by height
    m_surfaceColorCombo->setEnabled(state != Qt::Checked);
}

void MainWindow::onSurfaceColorChanged(int index)
{
    m_glWidget->setSurfaceColorIndex(index);
}

void MainWindow::onPointsColorChanged(int index)
{
    m_glWidget->setPointsColorIndex(index);
}

void MainWindow::onResetCamera()
{
    m_glWidget->resetCamera();
}

void MainWindow::onLoadFile()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Open Scalar Function Data",
        "",
        "Text Files (*.txt);;All Files (*)"
    );
    
    if (!filename.isEmpty()) {
        loadDataFiles({filename});
    }
}

void MainWindow::onReset()
{
    m_glWidget->clearData();
    m_data = ScalarFunction2DData();
    m_currentFilePath.clear();
    
    m_titleLabel->setText("No data loaded");
    m_titleEdit->clear();
    
    m_xMinLabel->setText("-");
    m_xMaxLabel->setText("-");
    m_yMinLabel->setText("-");
    m_yMaxLabel->setText("-");
    m_zMinLabel->setText("-");
    m_zMaxLabel->setText("-");
    
    m_pointsXLabel->setText("-");
    m_pointsYLabel->setText("-");
    m_totalPointsLabel->setText("-");
}

void MainWindow::onTitleChanged()
{
    m_titleLabel->setText(m_titleEdit->text());
}

QString MainWindow::formatValue(double value)
{
    if (std::abs(value) >= 10000 || (std::abs(value) > 0 && std::abs(value) < 0.01)) {
        return QString::number(value, 'e', 2);
    }
    if (std::abs(value - std::round(value)) < 1e-10) {
        return QString::number(static_cast<long long>(std::round(value)));
    }
    return QString::number(value, 'g', 6);
}

void MainWindow::updateBoundsDisplay()
{
    if (m_data.values.empty()) return;
    
    m_xMinLabel->setText(formatValue(m_data.xMin));
    m_xMaxLabel->setText(formatValue(m_data.xMax));
    m_yMinLabel->setText(formatValue(m_data.yMin));
    m_yMaxLabel->setText(formatValue(m_data.yMax));
    m_zMinLabel->setText(formatValue(m_data.minValue));
    m_zMaxLabel->setText(formatValue(m_data.maxValue));
    
    m_pointsXLabel->setText(QString::number(m_data.numPointsX));
    m_pointsYLabel->setText(QString::number(m_data.numPointsY));
    m_totalPointsLabel->setText(QString::number(m_data.getTotalPoints()));
}

void MainWindow::updateInfo()
{
    // Update window title with filename
    if (!m_currentFilePath.isEmpty()) {
        QFileInfo fileInfo(m_currentFilePath);
        setWindowTitle("MML Scalar Function 2D Visualizer - " + fileInfo.fileName());
    } else {
        setWindowTitle("MML Scalar Function 2D Visualizer");
    }
}
