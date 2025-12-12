#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QMessageBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <sstream>
#include <iomanip>

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
    setWindowTitle("MML 2D Scalar Function Visualizer");
    resize(1200, 800);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Info label
    m_infoLabel = new QLabel("No data loaded", this);
    m_infoLabel->setStyleSheet("QLabel { background-color: #2b2b2b; color: #ffffff; padding: 8px; }");
    mainLayout->addWidget(m_infoLabel);

    // OpenGL widget
    m_glWidget = new GLWidget(this);
    mainLayout->addWidget(m_glWidget, 1);

    // Control panel
    QGroupBox* controlGroup = new QGroupBox("Controls", this);
    QGridLayout* controlLayout = new QGridLayout(controlGroup);

    // Scale X slider
    QLabel* scaleXTextLabel = new QLabel("Scale X:", this);
    controlLayout->addWidget(scaleXTextLabel, 0, 0);

    m_scaleXSlider = new QSlider(Qt::Horizontal, this);
    m_scaleXSlider->setMinimum(1);
    m_scaleXSlider->setMaximum(200);
    m_scaleXSlider->setValue(100);  // Default 10.0
    m_scaleXSlider->setTickPosition(QSlider::TicksBelow);
    m_scaleXSlider->setTickInterval(20);
    controlLayout->addWidget(m_scaleXSlider, 0, 1);

    m_scaleXLabel = new QLabel("10.0", this);
    m_scaleXLabel->setMinimumWidth(50);
    controlLayout->addWidget(m_scaleXLabel, 0, 2);

    // Scale Y slider
    QLabel* scaleYTextLabel = new QLabel("Scale Y:", this);
    controlLayout->addWidget(scaleYTextLabel, 1, 0);

    m_scaleYSlider = new QSlider(Qt::Horizontal, this);
    m_scaleYSlider->setMinimum(1);
    m_scaleYSlider->setMaximum(200);
    m_scaleYSlider->setValue(100);  // Default 10.0
    m_scaleYSlider->setTickPosition(QSlider::TicksBelow);
    m_scaleYSlider->setTickInterval(20);
    controlLayout->addWidget(m_scaleYSlider, 1, 1);

    m_scaleYLabel = new QLabel("10.0", this);
    m_scaleYLabel->setMinimumWidth(50);
    controlLayout->addWidget(m_scaleYLabel, 1, 2);

    // Checkboxes
    controlLayout->addWidget(new QLabel("Options:", this), 2, 0);
    
    QHBoxLayout* checkBoxLayout = new QHBoxLayout();
    
    m_showGridCheckBox = new QCheckBox("Show Grid Points", this);
    m_showGridCheckBox->setChecked(true);
    checkBoxLayout->addWidget(m_showGridCheckBox);

    m_colorCheckBox = new QCheckBox("Color by Height", this);
    m_colorCheckBox->setChecked(true);
    checkBoxLayout->addWidget(m_colorCheckBox);

    checkBoxLayout->addStretch();
    
    controlLayout->addLayout(checkBoxLayout, 2, 1, 1, 2);

    mainLayout->addWidget(controlGroup);

    // Connect signals
    connect(m_scaleXSlider, &QSlider::valueChanged, this, &MainWindow::onScaleXChanged);
    connect(m_scaleYSlider, &QSlider::valueChanged, this, &MainWindow::onScaleYChanged);
    connect(m_showGridCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onShowGridPointsChanged);
    connect(m_colorCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onColorModeChanged);
}

bool MainWindow::loadDataFiles(const QStringList& filenames)
{
    if (filenames.isEmpty()) {
        QMessageBox::warning(this, "Error", "No filename specified.");
        return false;
    }

    QString filename = filenames[0];

    if (!MMLFileParser::LoadScalarFunction2D(filename.toStdString(), m_data)) {
        QMessageBox::critical(this, "Error", "Failed to load scalar function data from:\n" + filename);
        return false;
    }

    m_glWidget->setScalarFunction(m_data);
    updateInfo();

    return true;
}

void MainWindow::onScaleXChanged(int value)
{
    double scale = value / 10.0;  // Scale: 0.1 to 20.0
    m_scaleXLabel->setText(QString::number(scale, 'f', 1));
    m_glWidget->setScaleX(scale);
}

void MainWindow::onScaleYChanged(int value)
{
    double scale = value / 10.0;  // Scale: 0.1 to 20.0
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
}

void MainWindow::updateInfo()
{
    if (m_data.values.empty()) {
        m_infoLabel->setText("No data loaded");
        return;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    oss << "Title: " << m_data.title
        << "  |  Domain: [" << m_data.xMin << ", " << m_data.xMax << "] x [" 
        << m_data.yMin << ", " << m_data.yMax << "]"
        << "  |  Grid: " << m_data.numPointsX << " x " << m_data.numPointsY 
        << " = " << m_data.values.size() << " points"
        << "  |  Value range: [" << m_data.minValue << ", " << m_data.maxValue << "]";

    m_infoLabel->setText(QString::fromStdString(oss.str()));
}
