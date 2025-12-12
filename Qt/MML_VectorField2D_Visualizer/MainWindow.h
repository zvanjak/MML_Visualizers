#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QSlider>
#include <QCheckBox>
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
    void OnArrowScaleChanged(int value);
    void OnNormalizeChanged(int state);
    void OnColorMagnitudeChanged(int state);

private:
    void SetupUI();
    void UpdateInfo();

    GLWidget* glWidget_;
    QTextEdit* infoText_;
    QSlider* arrowScaleSlider_;
    QCheckBox* normalizeCheckbox_;
    QCheckBox* colorMagnitudeCheckbox_;
    
    QString currentFilename_;
};

#endif // MAINWINDOW_H
