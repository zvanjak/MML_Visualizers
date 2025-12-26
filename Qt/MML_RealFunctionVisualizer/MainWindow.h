#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QGroupBox>
#include <QScrollArea>
#include <QCheckBox>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <vector>
#include <memory>
#include "GLWidget.h"
#include "MMLData.h"

// Structure to hold legend entry widgets
struct LegendEntry {
    QCheckBox* checkbox;
    QWidget* colorBox;
    int functionIndex;      // Index into the function list
    int subFunctionIndex;   // For multi-function: which sub-function (-1 for single)
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const std::vector<std::string>& filenames = {}, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void LoadFile();
    void ClearAll();
    void ResetView();
    void OnTitleChanged();
    void OnGridToggled(bool checked);
    void OnLabelsToggled(bool checked);
    void OnAspectRatioToggled(bool checked);
    void OnLegendCheckboxToggled(bool checked);
    void OnBoundsChanged();

private:
    void LoadFunctionFile(const QString& filename);
    void UpdateLegend();
    void CreateSidebar(QWidget* parent);
    Color GetColorForIndex(int index);
    
    // Main widgets
    GLWidget* glWidget_;
    
    // Sidebar components
    QWidget* sidebarWidget_;
    QLineEdit* titleEdit_;
    
    // Legend section
    QScrollArea* legendScrollArea_;
    QWidget* legendContent_;
    QVBoxLayout* legendLayout_;
    std::vector<LegendEntry> legendEntries_;
    
    // Settings checkboxes
    QCheckBox* gridCheckbox_;
    QCheckBox* labelsCheckbox_;
    QCheckBox* aspectRatioCheckbox_;
    
    // Buttons
    QPushButton* loadButton_;
    QPushButton* clearButton_;
    QPushButton* resetButton_;
    
    // Status bar
    QStatusBar* statusBar_;
    
    // State
    std::vector<std::string> loadedFilenames_;
    int functionCounter_;
    QString graphTitle_;
    
    // Color palette (matching WPF)
    static const std::vector<Color> colorPalette_;
};

#endif // MAIN_WINDOW_H
