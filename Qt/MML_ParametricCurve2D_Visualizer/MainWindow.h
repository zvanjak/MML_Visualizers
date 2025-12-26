#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QWidget>
#include <QFrame>
#include <vector>
#include <memory>
#include "GLWidget.h"
#include "MMLData.h"

// Structure to hold legend entry with checkbox and label
struct LegendEntry {
    QCheckBox* checkbox;
    QFrame* colorSwatch;
    QLabel* nameLabel;
    int curveIndex;
    
    LegendEntry() : checkbox(nullptr), colorSwatch(nullptr), nameLabel(nullptr), curveIndex(-1) {}
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const std::vector<std::string>& filenames = {}, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void LoadFile();
    void ResetView();
    void ClearAll();
    
    // Animation slots
    void OnStartAnimation();
    void OnPauseAnimation();
    void OnResetAnimation();
    void OnAnimationSpeedChanged();
    void OnAnimationFrame();
    
    // Display settings slots
    void OnGridToggled(bool checked);
    void OnLabelsToggled(bool checked);
    void OnAspectRatioToggled(bool checked);
    
    // Legend checkbox slot
    void OnLegendCheckboxToggled(bool checked);

private:
    void CreateSidebar();
    void LoadCurveFile(const QString& filename, int index);
    void UpdateInfoDisplay();
    void UpdateLegend();
    void UpdateAnimationUI();
    LegendEntry CreateLegendEntry(const QString& name, const QColor& color, int index);

    // Main widgets
    GLWidget* glWidget_;
    QWidget* sidebarWidget_;
    
    // Sidebar groups
    QGroupBox* fileGroup_;
    QGroupBox* displayGroup_;
    QGroupBox* animationGroup_;
    QGroupBox* legendGroup_;
    QGroupBox* infoGroup_;
    
    // File buttons
    QPushButton* loadButton_;
    QPushButton* clearButton_;
    QPushButton* resetViewButton_;
    
    // Display settings
    QCheckBox* showGridCheckbox_;
    QCheckBox* showLabelsCheckbox_;
    QCheckBox* aspectRatioCheckbox_;
    
    // Animation controls
    QPushButton* startButton_;
    QPushButton* pauseButton_;
    QPushButton* resetAnimButton_;
    QLineEdit* speedInput_;
    QLabel* frameLabel_;
    QLabel* tValueLabel_;
    
    // Legend
    QVBoxLayout* legendLayout_;
    std::vector<LegendEntry> legendEntries_;
    
    // Info display
    QTextEdit* infoDisplay_;
    
    // Status bar
    QStatusBar* statusBar_;

    // Data
    std::vector<std::string> loadedFilenames_;
    int curveCounter_;
};

#endif // MAIN_WINDOW_H
