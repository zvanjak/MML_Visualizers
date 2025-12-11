#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
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
    void LoadCurveFile(const QString& filename);
    void UpdateInfoDisplay();
    Color GetColorForIndex(int index);

    GLWidget* glWidget_;
    QTextEdit* infoDisplay_;
    QPushButton* loadButton_;
    QPushButton* resetButton_;
    QLabel* statusLabel_;

    std::vector<std::string> loadedFilenames_;
    int curveCounter_;
};

#endif // MAIN_WINDOW_H
