#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include "GLWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void LoadDataFile(const QString& filename);

private slots:
    void OnLoadFile();
    void OnReset();
    void OnMagnitudeScaleChanged(int value);
    void OnArrowSizeChanged(int value);
    void OnNormalizeChanged(int state);
    void OnColorByMagnitudeChanged(int state);
    void OnPreserveAspectRatioChanged(int state);
    void OnArrowColorChanged(int index);

private:
    void SetupUI();
    void UpdateInfo();
    void UpdateStatistics();

    GLWidget* glWidget_;
    QTextEdit* infoText_;
    
    // Controls
    QSlider* magnitudeScaleSlider_;
    QLabel* magnitudeScaleLabel_;
    QSlider* arrowSizeSlider_;
    QLabel* arrowSizeLabel_;
    QCheckBox* normalizeCheckbox_;
    QCheckBox* colorByMagnitudeCheckbox_;
    QCheckBox* preserveAspectRatioCheckbox_;
    QComboBox* arrowColorCombo_;
    
    // Statistics labels
    QLabel* vectorCountLabel_;
    QLabel* minMagLabel_;
    QLabel* maxMagLabel_;
    QLabel* avgMagLabel_;
    
    QString currentFilename_;
};

#endif // MAINWINDOW_H
