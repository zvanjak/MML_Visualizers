#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QListWidget>
#include <vector>
#include <memory>
#include "GLWidget.h"
#include "MMLData.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const std::vector<std::string>& filenames = {}, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void LoadFile();
    void ResetView();

private:
    void LoadCurveFile(const QString& filename, int index);
    void UpdateInfoDisplay();
    void UpdateLegend();

    GLWidget* glWidget_;
    QTextEdit* infoDisplay_;
    QListWidget* legendList_;
    QPushButton* loadButton_;
    QPushButton* resetButton_;
    QStatusBar* statusBar_;

    std::vector<std::string> loadedFilenames_;
    int curveCounter_;
};

#endif // MAIN_WINDOW_H
