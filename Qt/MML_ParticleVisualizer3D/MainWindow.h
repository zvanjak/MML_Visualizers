#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QTextBrowser>
#include <QSplitter>
#include "GLWidget.h"
#include "MMLData.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const std::vector<std::string>& filenames, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void OnStartAnimation();
    void OnPauseAnimation();
    void OnRestartAnimation();
    void OnAnimationStep();
    void OnDelayChanged(int value);
    void OnToggleBoundingBox(int state);

private:
    void SetupUI();
    void LoadSimulations();
    void UpdateInfoDisplay();
    void UpdateStepDisplay();
    
    GLWidget *glWidget_;
    QTimer *animationTimer_;
    
    // UI Controls
    QPushButton *startButton_;
    QPushButton *pauseButton_;
    QPushButton *restartButton_;
    QSpinBox *delaySpinBox_;
    QLabel *currentStepLabel_;
    QLabel *totalStepsLabel_;
    QCheckBox *boundingBoxCheckBox_;
    QTextBrowser *infoBrowser_;
    
    // Data
    std::vector<std::string> filenames_;
    LoadedParticleSimulation3D simulation_;
    int currentStep_;
    bool isAnimating_;
};

#endif // MAINWINDOW_H
