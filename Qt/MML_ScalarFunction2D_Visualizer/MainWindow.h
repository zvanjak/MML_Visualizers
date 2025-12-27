#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include "GLWidget.h"
#include "MMLData.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    bool loadDataFiles(const QStringList& filenames);

private slots:
    void onScaleXChanged(int value);
    void onScaleYChanged(int value);
    void onShowGridPointsChanged(int state);
    void onColorModeChanged(int state);
    void onSurfaceColorChanged(int index);
    void onPointsColorChanged(int index);
    void onResetCamera();
    void onLoadFile();
    void onReset();
    void onTitleChanged();

private:
    void setupUI();
    void updateInfo();
    void updateBoundsDisplay();
    QString formatValue(double value);

    GLWidget* m_glWidget;
    
    // Title panel
    QLineEdit* m_titleEdit;
    QLabel* m_titleLabel;
    
    // Scale sliders
    QLabel* m_scaleXLabel;
    QLabel* m_scaleYLabel;
    QSlider* m_scaleXSlider;
    QSlider* m_scaleYSlider;
    
    // Display options
    QCheckBox* m_showGridCheckBox;
    QCheckBox* m_colorCheckBox;
    QComboBox* m_surfaceColorCombo;
    QComboBox* m_pointsColorCombo;
    
    // Graph bounds labels
    QLabel* m_xMinLabel;
    QLabel* m_xMaxLabel;
    QLabel* m_yMinLabel;
    QLabel* m_yMaxLabel;
    QLabel* m_zMinLabel;
    QLabel* m_zMaxLabel;
    
    // Grid info labels
    QLabel* m_pointsXLabel;
    QLabel* m_pointsYLabel;
    QLabel* m_totalPointsLabel;
    
    // Buttons
    QPushButton* m_resetCameraBtn;
    QPushButton* m_loadBtn;
    QPushButton* m_resetBtn;

    ScalarFunction2DData m_data;
    QString m_currentFilePath;
};
