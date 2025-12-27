#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QFormLayout>
#include <QButtonGroup>
#include <QFileInfo>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , glWidget_(nullptr)
    , animationTimer_(nullptr)
    , isPlaying_(false)
    , currentStep_(0)
    , refreshCounter_(0)
    , refreshEvery_(1)
{
    SetupUI();
    
    animationTimer_ = new QTimer(this);
    connect(animationTimer_, &QTimer::timeout, this, &MainWindow::OnTimerTick);
    
    setWindowTitle("MML Particle Visualizer 3D");
    resize(1400, 900);
}

MainWindow::~MainWindow()
{
}

void MainWindow::SetupUI()
{
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);
    
    // Create sidebar
    QWidget* sidebar = new QWidget();
    sidebar->setFixedWidth(280);
    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(5, 5, 5, 5);
    sidebarLayout->setSpacing(8);
    
    // === File Panel ===
    QGroupBox* fileGroup = new QGroupBox("File");
    QVBoxLayout* fileLayout = new QVBoxLayout(fileGroup);
    loadDataButton_ = new QPushButton("Load Data...");
    connect(loadDataButton_, &QPushButton::clicked, this, &MainWindow::OnLoadData);
    fileLayout->addWidget(loadDataButton_);
    sidebarLayout->addWidget(fileGroup);
    
    // === Title Panel ===
    QGroupBox* titleGroup = new QGroupBox("Simulation Title");
    QVBoxLayout* titleLayout = new QVBoxLayout(titleGroup);
    titleEdit_ = new QLineEdit();
    titleEdit_->setPlaceholderText("Untitled Simulation");
    connect(titleEdit_, &QLineEdit::editingFinished, this, &MainWindow::OnTitleChanged);
    titleLayout->addWidget(titleEdit_);
    sidebarLayout->addWidget(titleGroup);
    
    // === Simulation Controls Panel ===
    QGroupBox* controlsGroup = new QGroupBox("Simulation Controls");
    QVBoxLayout* controlsLayout = new QVBoxLayout(controlsGroup);
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    startPauseButton_ = new QPushButton("Start");
    restartButton_ = new QPushButton("Restart");
    connect(startPauseButton_, &QPushButton::clicked, this, &MainWindow::OnStartPause);
    connect(restartButton_, &QPushButton::clicked, this, &MainWindow::OnRestart);
    buttonsLayout->addWidget(startPauseButton_);
    buttonsLayout->addWidget(restartButton_);
    controlsLayout->addLayout(buttonsLayout);
    
    progressBar_ = new QProgressBar();
    progressBar_->setMinimum(0);
    progressBar_->setMaximum(100);
    progressBar_->setValue(0);
    controlsLayout->addWidget(progressBar_);
    
    currentStepLabel_ = new QLabel("Step: 0 / 0");
    currentStepLabel_->setAlignment(Qt::AlignCenter);
    controlsLayout->addWidget(currentStepLabel_);
    
    sidebarLayout->addWidget(controlsGroup);
    
    // === Animation Settings Panel ===
    QGroupBox* animationGroup = new QGroupBox("Animation Settings");
    QFormLayout* animationLayout = new QFormLayout(animationGroup);
    
    totalStepsLabel_ = new QLabel("0");
    animationLayout->addRow("Total Steps:", totalStepsLabel_);
    
    delaySpinBox_ = new QSpinBox();
    delaySpinBox_->setRange(1, 1000);
    delaySpinBox_->setValue(50);
    delaySpinBox_->setSuffix(" ms");
    connect(delaySpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::OnDelayChanged);
    animationLayout->addRow("Step Delay:", delaySpinBox_);
    
    refreshEverySpinBox_ = new QSpinBox();
    refreshEverySpinBox_->setRange(1, 100);
    refreshEverySpinBox_->setValue(1);
    refreshEverySpinBox_->setSuffix(" steps");
    connect(refreshEverySpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::OnRefreshEveryChanged);
    animationLayout->addRow("Refresh Every:", refreshEverySpinBox_);
    
    sidebarLayout->addWidget(animationGroup);
    
    // === Camera Controls Panel ===
    QGroupBox* cameraGroup = new QGroupBox("Camera");
    QVBoxLayout* cameraLayout = new QVBoxLayout(cameraGroup);
    
    lookAtCenterButton_ = new QPushButton("Look at Center");
    resetCameraButton_ = new QPushButton("Reset Camera");
    connect(lookAtCenterButton_, &QPushButton::clicked, this, &MainWindow::OnLookAtCenter);
    connect(resetCameraButton_, &QPushButton::clicked, this, &MainWindow::OnResetCamera);
    cameraLayout->addWidget(lookAtCenterButton_);
    cameraLayout->addWidget(resetCameraButton_);
    
    QLabel* cameraHelpLabel = new QLabel("Mouse: Left=Rotate, Right=Pan\nWheel=Zoom");
    cameraHelpLabel->setStyleSheet("color: gray; font-size: 10px;");
    cameraLayout->addWidget(cameraHelpLabel);
    
    sidebarLayout->addWidget(cameraGroup);
    
    // === Display Mode Panel ===
    QGroupBox* displayGroup = new QGroupBox("Display Mode");
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);
    
    QButtonGroup* displayButtonGroup = new QButtonGroup(this);
    displayNoneRadio_ = new QRadioButton("None (Axes only)");
    displayBoundingBoxRadio_ = new QRadioButton("Bounding Box");
    displayCoordinatePlanesRadio_ = new QRadioButton("Coordinate Planes");
    
    displayButtonGroup->addButton(displayNoneRadio_);
    displayButtonGroup->addButton(displayBoundingBoxRadio_);
    displayButtonGroup->addButton(displayCoordinatePlanesRadio_);
    displayNoneRadio_->setChecked(true);
    
    connect(displayNoneRadio_, &QRadioButton::toggled, this, &MainWindow::OnDisplayModeChanged);
    connect(displayBoundingBoxRadio_, &QRadioButton::toggled, this, &MainWindow::OnDisplayModeChanged);
    connect(displayCoordinatePlanesRadio_, &QRadioButton::toggled, this, &MainWindow::OnDisplayModeChanged);
    
    displayLayout->addWidget(displayNoneRadio_);
    displayLayout->addWidget(displayBoundingBoxRadio_);
    displayLayout->addWidget(displayCoordinatePlanesRadio_);
    
    sidebarLayout->addWidget(displayGroup);
    
    // === Particles Panel ===
    particlesGroupBox_ = new QGroupBox("Particles");
    QVBoxLayout* particlesLayout = new QVBoxLayout(particlesGroupBox_);
    
    particlesScrollArea_ = new QScrollArea();
    particlesScrollArea_->setWidgetResizable(true);
    particlesScrollArea_->setMaximumHeight(150);
    QWidget* particlesContainer = new QWidget();
    particlesContainer->setLayout(new QVBoxLayout());
    particlesScrollArea_->setWidget(particlesContainer);
    particlesLayout->addWidget(particlesScrollArea_);
    
    sidebarLayout->addWidget(particlesGroupBox_);
    
    // === Container Info Panel ===
    QGroupBox* containerGroup = new QGroupBox("Container Info");
    QFormLayout* containerLayout = new QFormLayout(containerGroup);
    
    containerWidthLabel_ = new QLabel("0.0");
    containerHeightLabel_ = new QLabel("0.0");
    containerDepthLabel_ = new QLabel("0.0");
    numParticlesLabel_ = new QLabel("0");
    
    containerLayout->addRow("Width:", containerWidthLabel_);
    containerLayout->addRow("Height:", containerHeightLabel_);
    containerLayout->addRow("Depth:", containerDepthLabel_);
    containerLayout->addRow("Particles:", numParticlesLabel_);
    
    sidebarLayout->addWidget(containerGroup);
    
    // Add stretch to push everything up
    sidebarLayout->addStretch();
    
    // Create OpenGL widget
    glWidget_ = new GLWidget();
    glWidget_->setMinimumSize(800, 600);
    
    // Add to main layout
    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(glWidget_, 1);
    
    setCentralWidget(centralWidget);
}

void MainWindow::LoadSimulation(const QString& filePath)
{
    MMLFileParser parser;
    if (!parser.ParseFile(filePath.toStdString(), simulation_)) {
        QMessageBox::critical(this, "Error", "Failed to load simulation file:\n" + filePath);
        return;
    }
    
    // Set window title
    QFileInfo fileInfo(filePath);
    setWindowTitle("MML Particle Visualizer 3D - " + fileInfo.fileName());
    
    // Update title edit
    titleEdit_->setText(QString::fromStdString(simulation_.title));
    
    // Reset animation state
    currentStep_ = 0;
    refreshCounter_ = 0;
    isPlaying_ = false;
    startPauseButton_->setText("Start");
    
    // Update UI
    glWidget_->SetSimulation(simulation_);
    UpdateParticleCheckboxes();
    UpdateContainerInfo();
    UpdateControls();
}

void MainWindow::UpdateControls()
{
    int totalSteps = simulation_.numSteps;
    totalStepsLabel_->setText(QString::number(totalSteps));
    
    progressBar_->setMaximum(totalSteps > 0 ? totalSteps - 1 : 0);
    progressBar_->setValue(currentStep_);
    
    currentStepLabel_->setText(QString("Step: %1 / %2").arg(currentStep_ + 1).arg(totalSteps));
}

void MainWindow::UpdateParticleCheckboxes()
{
    // Clear existing checkboxes
    QWidget* container = particlesScrollArea_->widget();
    QLayout* layout = container->layout();
    
    while (QLayoutItem* item = layout->takeAt(0)) {
        delete item->widget();
        delete item;
    }
    particleCheckboxes_.clear();
    
    // Create new checkboxes
    for (size_t i = 0; i < simulation_.particles.size(); ++i) {
        const auto& particle = simulation_.particles[i];
        
        QCheckBox* checkbox = new QCheckBox();
        checkbox->setChecked(particle.visible);
        
        // Create label with color indicator
        QString label = QString("Particle %1").arg(i + 1);
        if (!particle.name.empty()) {
            label = QString::fromStdString(particle.name);
        }
        
        // Create color indicator
        QColor color(particle.color.r * 255, particle.color.g * 255, 
                     particle.color.b * 255);
        QString colorStyle = QString("background-color: %1; border: 1px solid gray;").arg(color.name());
        
        QWidget* row = new QWidget();
        QHBoxLayout* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(2, 2, 2, 2);
        
        QLabel* colorBox = new QLabel();
        colorBox->setFixedSize(16, 16);
        colorBox->setStyleSheet(colorStyle);
        
        checkbox->setText(label);
        
        rowLayout->addWidget(checkbox);
        rowLayout->addStretch();
        rowLayout->addWidget(colorBox);
        
        layout->addWidget(row);
        particleCheckboxes_.push_back(checkbox);
        
        // Connect with index capture
        int index = static_cast<int>(i);
        connect(checkbox, &QCheckBox::toggled, this, [this, index]() {
            OnParticleVisibilityChanged(index);
        });
    }
}

void MainWindow::UpdateContainerInfo()
{
    containerWidthLabel_->setText(QString::number(simulation_.containerWidth, 'f', 2));
    containerHeightLabel_->setText(QString::number(simulation_.containerHeight, 'f', 2));
    containerDepthLabel_->setText(QString::number(simulation_.containerDepth, 'f', 2));
    numParticlesLabel_->setText(QString::number(simulation_.particles.size()));
}

void MainWindow::OnLoadData()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open Particle Simulation Data",
        QString(),
        "Data Files (*.txt);;All Files (*)");
    
    if (!filePath.isEmpty()) {
        // Stop any running animation
        if (isPlaying_) {
            animationTimer_->stop();
            startPauseButton_->setText("Start");
            isPlaying_ = false;
        }
        LoadSimulation(filePath);
    }
}

void MainWindow::OnStartPause()
{
    if (isPlaying_) {
        animationTimer_->stop();
        startPauseButton_->setText("Start");
        isPlaying_ = false;
    } else {
        animationTimer_->start(delaySpinBox_->value());
        startPauseButton_->setText("Pause");
        isPlaying_ = true;
    }
}

void MainWindow::OnRestart()
{
    currentStep_ = 0;
    refreshCounter_ = 0;
    glWidget_->SetCurrentStep(currentStep_);
    UpdateControls();
}

void MainWindow::OnTimerTick()
{
    refreshCounter_++;
    
    if (refreshCounter_ >= refreshEvery_) {
        refreshCounter_ = 0;
        currentStep_++;
        
        if (currentStep_ >= simulation_.numSteps) {
            currentStep_ = 0;  // Loop
        }
        
        glWidget_->SetCurrentStep(currentStep_);
        UpdateControls();
    }
}

void MainWindow::OnDelayChanged(int value)
{
    if (isPlaying_) {
        animationTimer_->setInterval(value);
    }
}

void MainWindow::OnRefreshEveryChanged(int value)
{
    refreshEvery_ = value;
    refreshCounter_ = 0;
}

void MainWindow::OnDisplayModeChanged()
{
    if (displayNoneRadio_->isChecked()) {
        glWidget_->SetDisplayMode(DisplayMode::None);
    } else if (displayBoundingBoxRadio_->isChecked()) {
        glWidget_->SetDisplayMode(DisplayMode::BoundingBox);
    } else if (displayCoordinatePlanesRadio_->isChecked()) {
        glWidget_->SetDisplayMode(DisplayMode::CoordinatePlanes);
    }
}

void MainWindow::OnLookAtCenter()
{
    glWidget_->LookAtCenter();
}

void MainWindow::OnResetCamera()
{
    glWidget_->ResetCamera();
}

void MainWindow::OnTitleChanged()
{
    simulation_.title = titleEdit_->text().toStdString();
}

void MainWindow::OnParticleVisibilityChanged(int index)
{
    if (index >= 0 && index < static_cast<int>(particleCheckboxes_.size())) {
        bool visible = particleCheckboxes_[index]->isChecked();
        glWidget_->SetParticleVisible(index, visible);
    }
}
