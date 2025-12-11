#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QStatusBar>
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
    void LoadFunctionFile(const QString& filename);
    void UpdateInfoDisplay();
    Color GetColorForIndex(int index);

    GLWidget* glWidget_;
    QTextEdit* infoDisplay_;
    QPushButton* loadButton_;
    QPushButton* resetButton_;
    QStatusBar* statusBar_;

    std::vector<std::string> loadedFilenames_;
    int functionCounter_;
};

#endif // MAIN_WINDOW_H
