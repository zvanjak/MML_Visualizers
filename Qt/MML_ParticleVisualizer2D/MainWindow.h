#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include "GLWidget.h"
#include "MMLData.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void LoadDataFile(const QString& filename);

private slots:
    void OnLoadFile();
    void OnReset();
    void OnPlayPause();
    void OnStop();
    void OnStepForward();
    void OnStepBackward();
    void OnTimestepSliderChanged(int value);
    void OnTimestepChanged(int timestep);
    void OnSpeedChanged(int value);

private:
    void SetupUI();
    void UpdateLegend();
    void UpdateInfo();
    void UpdateTimestepLabel();
    void UpdatePlayButtonText();

    GLWidget* glWidget_;
    QListWidget* legendList_;
    QTextEdit* infoText_;
    QSlider* timestepSlider_;
    QLabel* timestepLabel_;
    QPushButton* playButton_;
    QPushButton* stopButton_;
    QPushButton* stepBackButton_;
    QPushButton* stepForwardButton_;
    QSlider* speedSlider_;
    QLabel* speedLabel_;
    
    SimulationData currentData_;
    QString currentFilename_;
};

#endif // MAINWINDOW_H
