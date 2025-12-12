#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QMessageBox>
#include <QGroupBox>
#include <QHBoxLayout>
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
    setWindowTitle("MML 3D Vector Field Visualizer");
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
    QHBoxLayout* controlLayout = new QHBoxLayout(controlGroup);

    // Vector scale slider
    QLabel* scaleTextLabel = new QLabel("Vector Scale:", this);
    controlLayout->addWidget(scaleTextLabel);

    m_scaleSlider = new QSlider(Qt::Horizontal, this);
    m_scaleSlider->setMinimum(1);
    m_scaleSlider->setMaximum(100);
    m_scaleSlider->setValue(50);
    m_scaleSlider->setTickPosition(QSlider::TicksBelow);
    m_scaleSlider->setTickInterval(10);
    controlLayout->addWidget(m_scaleSlider);

    m_scaleLabel = new QLabel("5.0", this);
    m_scaleLabel->setMinimumWidth(50);
    controlLayout->addWidget(m_scaleLabel);

    controlLayout->addSpacing(20);

    // Color mode checkbox
    m_colorCheckBox = new QCheckBox("Color by Magnitude", this);
    m_colorCheckBox->setChecked(true);
    controlLayout->addWidget(m_colorCheckBox);

    controlLayout->addStretch();

    mainLayout->addWidget(controlGroup);

    // Connect signals
    connect(m_scaleSlider, &QSlider::valueChanged, this, &MainWindow::onScaleChanged);
    connect(m_colorCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onColorModeChanged);
}

bool MainWindow::loadDataFiles(const QStringList& filenames)
{
    if (filenames.isEmpty()) {
        QMessageBox::warning(this, "Error", "No filename specified.");
        return false;
    }

    QString filename = filenames[0];

    if (!MMLFileParser::LoadVectorField3D(filename.toStdString(), m_vectorField)) {
        QMessageBox::critical(this, "Error", "Failed to load vector field data from:\n" + filename);
        return false;
    }

    m_glWidget->setVectorField(m_vectorField);
    updateInfo();

    return true;
}

void MainWindow::onScaleChanged(int value)
{
    double scale = value / 10.0;  // Scale: 0.1 to 10.0
    m_scaleLabel->setText(QString::number(scale, 'f', 1));
    m_glWidget->setVectorScale(scale);
}

void MainWindow::onColorModeChanged(int state)
{
    m_glWidget->setColorByMagnitude(state == Qt::Checked);
}

void MainWindow::updateInfo()
{
    if (m_vectorField.vectors.empty()) {
        m_infoLabel->setText("No data loaded");
        return;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    oss << "Title: " << m_vectorField.title
        << "  |  Vectors: " << m_vectorField.vectors.size()
        << "  |  Magnitude range: [" << m_vectorField.minMagnitude << ", " << m_vectorField.maxMagnitude << "]"
        << "  |  Bounds: (" << m_vectorField.minBounds.x << ", " << m_vectorField.minBounds.y << ", " << m_vectorField.minBounds.z << ") to ("
        << m_vectorField.maxBounds.x << ", " << m_vectorField.maxBounds.y << ", " << m_vectorField.maxBounds.z << ")";

    m_infoLabel->setText(QString::fromStdString(oss.str()));
}
