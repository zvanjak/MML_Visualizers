#include <QApplication>
#include "MainWindow.h"
#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Collect filenames from command line arguments
    std::vector<std::string> filenames;
    for (int i = 1; i < argc; ++i) {
        filenames.push_back(argv[i]);
    }
    
    if (filenames.empty()) {
        std::cerr << "Usage: " << argv[0] << " <data_file>" << std::endl;
        std::cerr << "Example: " << argv[0] << " SimData3D.txt" << std::endl;
        return 1;
    }
    
    MainWindow window;
    window.LoadSimulation(QString::fromStdString(filenames[0]));
    window.show();
    
    return app.exec();
}
