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
    void onScaleChanged(int value);
    void onColorModeChanged(int state);

private:
    void setupUI();
    void updateInfo();

    GLWidget* m_glWidget;
    QLabel* m_infoLabel;
    QLabel* m_scaleLabel;
    QSlider* m_scaleSlider;
    QCheckBox* m_colorCheckBox;

    LoadedVectorField3D m_vectorField;
};
