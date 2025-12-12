#include "MainWindow.h"
#include "MMLFileParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QGroupBox>
#include <QSplitter>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    SetupUI();
    setWindowTitle("MML Particle Visualizer 2D - Qt");
    resize(1400, 900);
}

MainWindow::~MainWindow() {
}

void MainWindow::SetupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Left panel - OpenGL widget
    glWidget_ = new GLWidget(this);
    glWidget_->setMinimumSize(800, 600);
    
    // Right panel - controls
    QWidget* rightPanel = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    
    // Animation controls
    QGroupBox* animGroup = new QGroupBox("Animation Controls", this);
    QVBoxLayout* animLayout = new QVBoxLayout(animGroup);
    
    // Play/Pause button
    playButton_ = new QPushButton("Play", this);
    connect(playButton_, &QPushButton::clicked, this, &MainWindow::OnPlayPause);
    animLayout->addWidget(playButton_);
    
    // Stop button
    stopButton_ = new QPushButton("Stop", this);
    connect(stopButton_, &QPushButton::clicked, this, &MainWindow::OnStop);
    animLayout->addWidget(stopButton_);
    
    // Step buttons
    QHBoxLayout* stepLayout = new QHBoxLayout();
    stepBackButton_ = new QPushButton("◄ Step Back", this);
    stepForwardButton_ = new QPushButton("Step Forward ►", this);
    connect(stepBackButton_, &QPushButton::clicked, this, &MainWindow::OnStepBackward);
    connect(stepForwardButton_, &QPushButton::clicked, this, &MainWindow::OnStepForward);
    stepLayout->addWidget(stepBackButton_);
    stepLayout->addWidget(stepForwardButton_);
    animLayout->addLayout(stepLayout);
    
    // Timestep slider
    QHBoxLayout* sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(new QLabel("Timestep:", this));
    timestepSlider_ = new QSlider(Qt::Horizontal, this);
    timestepSlider_->setMinimum(0);
    timestepSlider_->setMaximum(0);
    timestepSlider_->setValue(0);
    connect(timestepSlider_, &QSlider::valueChanged, this, &MainWindow::OnTimestepSliderChanged);
    sliderLayout->addWidget(timestepSlider_);
    timestepLabel_ = new QLabel("0 / 0", this);
    sliderLayout->addWidget(timestepLabel_);
    animLayout->addLayout(sliderLayout);
    
    // Speed control
    QHBoxLayout* speedLayout = new QHBoxLayout();
    speedLayout->addWidget(new QLabel("Speed:", this));
    speedSlider_ = new QSlider(Qt::Horizontal, this);
    speedSlider_->setMinimum(1);
    speedSlider_->setMaximum(60);
    speedSlider_->setValue(10);
    connect(speedSlider_, &QSlider::valueChanged, this, &MainWindow::OnSpeedChanged);
    speedLayout->addWidget(speedSlider_);
    speedLabel_ = new QLabel("10 FPS", this);
    speedLayout->addWidget(speedLabel_);
    animLayout->addLayout(speedLayout);
    
    rightLayout->addWidget(animGroup);
    
    // Legend
    QGroupBox* legendGroup = new QGroupBox("Particles", this);
    QVBoxLayout* legendLayout = new QVBoxLayout(legendGroup);
    legendList_ = new QListWidget(this);
    legendLayout->addWidget(legendList_);
    rightLayout->addWidget(legendGroup);
    
    // Info panel
    QGroupBox* infoGroup = new QGroupBox("Information", this);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup);
    infoText_ = new QTextEdit(this);
    infoText_->setReadOnly(true);
    infoText_->setMaximumHeight(150);
    infoLayout->addWidget(infoText_);
    rightLayout->addWidget(infoGroup);
    
    // File controls
    QGroupBox* fileGroup = new QGroupBox("File", this);
    QVBoxLayout* fileLayout = new QVBoxLayout(fileGroup);
    QPushButton* loadBtn = new QPushButton("Load Data File...", this);
    QPushButton* resetBtn = new QPushButton("Reset", this);
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::OnLoadFile);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::OnReset);
    fileLayout->addWidget(loadBtn);
    fileLayout->addWidget(resetBtn);
    rightLayout->addWidget(fileGroup);
    
    rightLayout->addStretch();
    rightPanel->setMaximumWidth(350);
    
    // Add to main layout
    mainLayout->addWidget(glWidget_, 1);
    mainLayout->addWidget(rightPanel);
    
    // Connect GLWidget signals
    connect(glWidget_, &GLWidget::TimestepChanged, this, &MainWindow::OnTimestepChanged);
    
    // Initial state
    UpdatePlayButtonText();
}

void MainWindow::LoadDataFile(const QString& filename) {
    std::string errorMsg;
    SimulationData data;
    
    if (!MMLFileParser::ParseFile(filename.toStdString(), data, errorMsg)) {
        QMessageBox::critical(this, "Error", QString("Failed to load file:\n%1").arg(QString::fromStdString(errorMsg)));
        return;
    }
    
    currentData_ = data;
    currentFilename_ = filename;
    
    glWidget_->LoadSimulation(data);
    
    // Update UI
    timestepSlider_->setMaximum(data.numSteps - 1);
    timestepSlider_->setValue(0);
    
    UpdateLegend();
    UpdateInfo();
    UpdateTimestepLabel();
    UpdatePlayButtonText();
}

void MainWindow::OnLoadFile() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Select Particle Simulation Data File",
        QString(),
        "Text Files (*.txt);;All Files (*.*)"
    );
    
    if (!filename.isEmpty()) {
        LoadDataFile(filename);
    }
}

void MainWindow::OnReset() {
    glWidget_->ClearSimulation();
    legendList_->clear();
    infoText_->clear();
    timestepSlider_->setMaximum(0);
    timestepSlider_->setValue(0);
    currentData_ = SimulationData();
    currentFilename_.clear();
    UpdateTimestepLabel();
    UpdatePlayButtonText();
}

void MainWindow::OnPlayPause() {
    if (glWidget_->IsPlaying()) {
        glWidget_->Pause();
    } else {
        glWidget_->Play();
    }
    UpdatePlayButtonText();
}

void MainWindow::OnStop() {
    glWidget_->Stop();
    UpdatePlayButtonText();
}

void MainWindow::OnStepForward() {
    glWidget_->StepForward();
}

void MainWindow::OnStepBackward() {
    glWidget_->StepBackward();
}

void MainWindow::OnTimestepSliderChanged(int value) {
    glWidget_->SetTimestep(value);
}

void MainWindow::OnTimestepChanged(int timestep) {
    timestepSlider_->blockSignals(true);
    timestepSlider_->setValue(timestep);
    timestepSlider_->blockSignals(false);
    UpdateTimestepLabel();
}

void MainWindow::OnSpeedChanged(int value) {
    glWidget_->SetAnimationSpeed(value);
    speedLabel_->setText(QString("%1 FPS").arg(value));
}

void MainWindow::UpdateLegend() {
    legendList_->clear();
    
    for (const auto& ball : currentData_.balls) {
        QString text = QString("%1 (r=%2)").arg(QString::fromStdString(ball.GetName())).arg(ball.GetRadius());
        QListWidgetItem* item = new QListWidgetItem(text, legendList_);
        
        // Set color
        QString colorName = QString::fromStdString(ball.GetColor());
        QColor color;
        if (colorName == "Black") color = QColor(0, 0, 0);
        else if (colorName == "Orange") color = QColor(255, 165, 0);
        else if (colorName == "Blue") color = QColor(0, 0, 255);
        else if (colorName == "Red") color = QColor(255, 0, 0);
        else if (colorName == "Green") color = QColor(0, 128, 0);
        else if (colorName == "Purple") color = QColor(128, 0, 128);
        else if (colorName == "Cyan") color = QColor(0, 255, 255);
        else if (colorName == "Brown") color = QColor(165, 42, 42);
        else if (colorName == "Magenta") color = QColor(255, 0, 255);
        else if (colorName == "Yellow") color = QColor(255, 255, 0);
        
        item->setForeground(QBrush(color));
    }
}

void MainWindow::UpdateInfo() {
    QString info;
    info += QString("File: %1\n").arg(QFileInfo(currentFilename_).fileName());
    info += QString("Simulation Area: %1 x %2\n").arg(currentData_.width).arg(currentData_.height);
    info += QString("Number of Particles: %1\n").arg(currentData_.balls.size());
    info += QString("Number of Timesteps: %1\n").arg(currentData_.numSteps);
    info += "\n";
    info += "Controls:\n";
    info += "- Mouse drag: Pan view\n";
    info += "- Mouse wheel: Zoom\n";
    info += "- Slider: Scrub timesteps\n";
    
    infoText_->setPlainText(info);
}

void MainWindow::UpdateTimestepLabel() {
    int current = glWidget_->GetCurrentTimestep();
    int total = glWidget_->GetNumTimesteps();
    timestepLabel_->setText(QString("%1 / %2").arg(current).arg(total));
}

void MainWindow::UpdatePlayButtonText() {
    if (glWidget_->IsPlaying()) {
        playButton_->setText("Pause");
    } else {
        playButton_->setText("Play");
    }
}
