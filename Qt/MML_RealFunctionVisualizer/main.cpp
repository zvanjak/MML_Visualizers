#include "MainWindow.h"
#include <QApplication>
#include <vector>
#include <string>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Collect filenames from command line
    std::vector<std::string> filenames;
    for (int i = 1; i < argc; i++) {
        filenames.push_back(argv[i]);
    }

    MainWindow window(filenames);
    window.show();

    return app.exec();
}
