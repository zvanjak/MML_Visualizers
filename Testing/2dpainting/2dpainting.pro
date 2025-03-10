QT += widgets opengl openglwidgets

HEADERS     = glwidget.h \
              helper.h \
              widget.h \
              window.h
SOURCES     = glwidget.cpp \
              helper.cpp \
              main.cpp \
              widget.cpp \
              window.cpp

# install
target.path = C:/Projects/MML_Visualizers/2dpainting
INSTALLS += target
