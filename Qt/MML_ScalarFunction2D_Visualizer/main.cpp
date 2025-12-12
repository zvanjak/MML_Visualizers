#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    
    // Load data files from command line
    QStringList filenames;
    for (int i = 1; i < argc; i++) {
        filenames.append(argv[i]);
    }
    
    if (!filenames.isEmpty()) {
        window.loadDataFiles(filenames);
    }
    
    window.show();

    return app.exec();
}
