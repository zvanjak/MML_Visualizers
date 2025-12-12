#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QApplication>
#include <sstream>
#include <iomanip>

MainWindow::MainWindow(const std::vector<std::string>& filenames, QWidget *parent)
    : QMainWindow(parent)
    , filenames_(filenames)
    , currentStep_(0)
    , isAnimating_(false)
{
    setWindowTitle("MML Particle Visualizer 3D");
    resize(1200, 800);
    
    SetupUI();
    LoadSimulations();
    UpdateInfoDisplay();
}

MainWindow::~MainWindow()
{
}

void MainWindow::SetupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    // Create splitter
    QSplitter *splitter = new QSplitter(Qt::Horizontal, centralWidget);
    
    // Left panel - OpenGL widget
    glWidget_ = new GLWidget(splitter);
    glWidget_->setMinimumSize(600, 600);
    splitter->addWidget(glWidget_);
    
    // Right panel - controls and info
    QWidget *rightPanel = new QWidget(splitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    // Animation controls
    QGroupBox *animationGroup = new QGroupBox("Animation Controls", rightPanel);
    QVBoxLayout *animGroupLayout = new QVBoxLayout(animationGroup);
    
    // Start/Pause/Restart buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    startButton_ = new QPushButton("Start", animationGroup);
    pauseButton_ = new QPushButton("Pause", animationGroup);
    restartButton_ = new QPushButton("Restart", animationGroup);
    
    pauseButton_->setEnabled(false);
    restartButton_->setEnabled(false);
    
    buttonLayout->addWidget(startButton_);
    buttonLayout->addWidget(pauseButton_);
    buttonLayout->addWidget(restartButton_);
    animGroupLayout->addLayout(buttonLayout);
    
    // Delay control
    QHBoxLayout *delayLayout = new QHBoxLayout();
    delayLayout->addWidget(new QLabel("Delay (ms):", animationGroup));
    delaySpinBox_ = new QSpinBox(animationGroup);
    delaySpinBox_->setRange(1, 1000);
    delaySpinBox_->setValue(50);
    delayLayout->addWidget(delaySpinBox_);
    animGroupLayout->addLayout(delayLayout);
    
    // Step display
    QHBoxLayout *stepLayout = new QHBoxLayout();
    stepLayout->addWidget(new QLabel("Current Step:", animationGroup));
    currentStepLabel_ = new QLabel("0", animationGroup);
    stepLayout->addWidget(currentStepLabel_);
    stepLayout->addWidget(new QLabel("/", animationGroup));
    totalStepsLabel_ = new QLabel("0", animationGroup);
    stepLayout->addWidget(totalStepsLabel_);
    stepLayout->addStretch();
    animGroupLayout->addLayout(stepLayout);
    
    rightLayout->addWidget(animationGroup);
    
    // Display options
    QGroupBox *displayGroup = new QGroupBox("Display Options", rightPanel);
    QVBoxLayout *displayLayout = new QVBoxLayout(displayGroup);
    
    boundingBoxCheckBox_ = new QCheckBox("Show Bounding Box", displayGroup);
    boundingBoxCheckBox_->setChecked(true);
    displayLayout->addWidget(boundingBoxCheckBox_);
    
    rightLayout->addWidget(displayGroup);
    
    // Info display
    QGroupBox *infoGroup = new QGroupBox("Simulation Info", rightPanel);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
    
    infoBrowser_ = new QTextBrowser(infoGroup);
    infoBrowser_->setMaximumHeight(300);
    infoLayout->addWidget(infoBrowser_);
    
    rightLayout->addWidget(infoGroup);
    rightLayout->addStretch();
    
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(splitter);
    
    // Animation timer
    animationTimer_ = new QTimer(this);
    
    // Connect signals
    connect(startButton_, &QPushButton::clicked, this, &MainWindow::OnStartAnimation);
    connect(pauseButton_, &QPushButton::clicked, this, &MainWindow::OnPauseAnimation);
    connect(restartButton_, &QPushButton::clicked, this, &MainWindow::OnRestartAnimation);
    connect(animationTimer_, &QTimer::timeout, this, &MainWindow::OnAnimationStep);
    connect(delaySpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &MainWindow::OnDelayChanged);
    connect(boundingBoxCheckBox_, &QCheckBox::stateChanged, 
            this, &MainWindow::OnToggleBoundingBox);
}

void MainWindow::LoadSimulations()
{
    if (filenames_.empty()) {
        QMessageBox::warning(this, "No Files", "No data files specified.");
        return;
    }
    
    try {
        // Load first file
        simulation_ = MMLFileParser::LoadParticleSimulation3D(filenames_[0]);
        
        glWidget_->SetSimulation(simulation_);
        glWidget_->SetCurrentStep(0);
        
        totalStepsLabel_->setText(QString::number(simulation_.numSteps));
        currentStepLabel_->setText("0");
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error Loading File", 
                            QString("Failed to load simulation:\n%1").arg(e.what()));
    }
}

void MainWindow::UpdateInfoDisplay()
{
    std::ostringstream html;
    
    html << "<html><body style='font-family: Arial; font-size: 10pt;'>";
    html << "<h3>Simulation Information</h3>";
    html << "<table border='1' cellpadding='5' cellspacing='0' style='border-collapse: collapse;'>";
    html << "<tr><th>Property</th><th>Value</th></tr>";
    
    html << "<tr><td><b>Dimensions</b></td><td>" 
         << simulation_.width << " × " 
         << simulation_.height << " × " 
         << simulation_.depth << "</td></tr>";
    
    html << "<tr><td><b>Particles</b></td><td>" 
         << simulation_.particles.size() << "</td></tr>";
    
    html << "<tr><td><b>Steps</b></td><td>" 
         << simulation_.numSteps << "</td></tr>";
    
    html << "</table>";
    
    html << "<h3>Particles</h3>";
    html << "<table border='1' cellpadding='5' cellspacing='0' style='border-collapse: collapse;'>";
    html << "<tr><th>#</th><th>Name</th><th>Radius</th><th>Color</th></tr>";
    
    for (size_t i = 0; i < simulation_.particles.size() && i < 10; ++i) {
        const auto& p = simulation_.particles[i];
        html << "<tr>";
        html << "<td>" << (i + 1) << "</td>";
        html << "<td>" << p.name << "</td>";
        html << "<td>" << std::fixed << std::setprecision(1) << p.radius << "</td>";
        
        // Color swatch
        int r = static_cast<int>(p.color.r * 255);
        int g = static_cast<int>(p.color.g * 255);
        int b = static_cast<int>(p.color.b * 255);
        html << "<td style='background-color: rgb(" << r << "," << g << "," << b << ");'>&nbsp;&nbsp;&nbsp;</td>";
        html << "</tr>";
    }
    
    if (simulation_.particles.size() > 10) {
        html << "<tr><td colspan='4' style='text-align: center;'><i>... and " 
             << (simulation_.particles.size() - 10) << " more</i></td></tr>";
    }
    
    html << "</table>";
    
    html << "<h3>Controls</h3>";
    html << "<ul>";
    html << "<li><b>Left Mouse</b>: Rotate camera</li>";
    html << "<li><b>Right Mouse</b>: Pan view</li>";
    html << "<li><b>Mouse Wheel</b>: Zoom in/out</li>";
    html << "</ul>";
    
    html << "</body></html>";
    
    infoBrowser_->setHtml(QString::fromStdString(html.str()));
}

void MainWindow::UpdateStepDisplay()
{
    currentStepLabel_->setText(QString::number(currentStep_));
}

void MainWindow::OnStartAnimation()
{
    isAnimating_ = true;
    startButton_->setEnabled(false);
    pauseButton_->setEnabled(true);
    restartButton_->setEnabled(false);
    
    animationTimer_->start(delaySpinBox_->value());
}

void MainWindow::OnPauseAnimation()
{
    isAnimating_ = false;
    animationTimer_->stop();
    
    startButton_->setEnabled(true);
    pauseButton_->setEnabled(false);
    restartButton_->setEnabled(true);
}

void MainWindow::OnRestartAnimation()
{
    currentStep_ = 0;
    glWidget_->SetCurrentStep(currentStep_);
    UpdateStepDisplay();
    
    startButton_->setEnabled(true);
    pauseButton_->setEnabled(false);
    restartButton_->setEnabled(false);
}

void MainWindow::OnAnimationStep()
{
    currentStep_++;
    
    if (currentStep_ >= simulation_.numSteps) {
        // Animation complete
        OnPauseAnimation();
        restartButton_->setEnabled(true);
        currentStep_ = simulation_.numSteps - 1;
    }
    
    glWidget_->SetCurrentStep(currentStep_);
    UpdateStepDisplay();
}

void MainWindow::OnDelayChanged(int value)
{
    if (isAnimating_) {
        animationTimer_->setInterval(value);
    }
}

void MainWindow::OnToggleBoundingBox(int state)
{
    glWidget_->SetShowBoundingBox(state == Qt::Checked);
}
