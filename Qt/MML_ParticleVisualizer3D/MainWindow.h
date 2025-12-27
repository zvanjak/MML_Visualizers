#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QSlider>
#include <QProgressBar>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QScrollArea>
#include <QFileDialog>
#include <vector>
#include "GLWidget.h"
#include "MMLData.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void LoadSimulation(const QString& filePath);

private slots:
    void OnLoadData();
    void OnStartPause();
    void OnRestart();
    void OnTimerTick();
    void OnDelayChanged(int value);
    void OnRefreshEveryChanged(int value);
    void OnDisplayModeChanged();
    void OnLookAtCenter();
    void OnResetCamera();
    void OnTitleChanged();
    void OnParticleVisibilityChanged(int index);

private:
    void SetupUI();
    void UpdateControls();
    void UpdateParticleCheckboxes();
    void UpdateContainerInfo();
    
    // Central widget
    GLWidget* glWidget_;
    
    // Sidebar panels
    QLineEdit* titleEdit_;
    
    // File controls
    QPushButton* loadDataButton_;
    
    // Simulation controls
    QPushButton* startPauseButton_;
    QPushButton* restartButton_;
    QProgressBar* progressBar_;
    QLabel* currentStepLabel_;
    
    // Animation settings
    QLabel* totalStepsLabel_;
    QSpinBox* delaySpinBox_;
    QSpinBox* refreshEverySpinBox_;
    
    // Camera controls
    QPushButton* lookAtCenterButton_;
    QPushButton* resetCameraButton_;
    
    // Display mode
    QRadioButton* displayNoneRadio_;
    QRadioButton* displayBoundingBoxRadio_;
    QRadioButton* displayCoordinatePlanesRadio_;
    
    // Particles panel
    QGroupBox* particlesGroupBox_;
    QScrollArea* particlesScrollArea_;
    std::vector<QCheckBox*> particleCheckboxes_;
    
    // Container info
    QLabel* containerWidthLabel_;
    QLabel* containerHeightLabel_;
    QLabel* containerDepthLabel_;
    QLabel* numParticlesLabel_;
    
    // Animation state
    QTimer* animationTimer_;
    bool isPlaying_;
    int currentStep_;
    int refreshCounter_;
    int refreshEvery_;
    
    LoadedParticleSimulation3D simulation_;
};

#endif // MAINWINDOW_H
