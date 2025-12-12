#include <QApplication>
#include "MainWindow.h"
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    // Load data file from command line if provided
    if (argc > 1) {
        QString filename = QString::fromLocal8Bit(argv[1]);
        std::cout << "Loading data file: " << filename.toStdString() << std::endl;
        window.LoadDataFile(filename);
    }
    
    return app.exec();
}
