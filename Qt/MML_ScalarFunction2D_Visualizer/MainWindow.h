#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QVBoxLayout>
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

private:
    void setupUI();
    void updateInfo();

    GLWidget* m_glWidget;
    QLabel* m_infoLabel;
    QLabel* m_scaleXLabel;
    QLabel* m_scaleYLabel;
    QSlider* m_scaleXSlider;
    QSlider* m_scaleYSlider;
    QCheckBox* m_showGridCheckBox;
    QCheckBox* m_colorCheckBox;

    ScalarFunction2DData m_data;
};
