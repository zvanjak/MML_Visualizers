#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <vector>
#include <string>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    std::vector<std::string> filenames;
    
    // Check command line arguments for data files
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            filenames.push_back(argv[i]);
        }
    }

    MainWindow window(filenames);
    window.show();

    return app.exec();
}
