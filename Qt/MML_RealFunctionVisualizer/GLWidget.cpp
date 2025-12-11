#include "GLWidget.h"
#include <GL/gl.h>
#include <cmath>
#include <algorithm>

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , viewMinX_(-10.0)
    , viewMaxX_(10.0)
    , viewMinY_(-10.0)
    , viewMaxY_(10.0)
    , defaultMinX_(-10.0)
    , defaultMaxX_(10.0)
    , defaultMinY_(-10.0)
    , defaultMaxY_(10.0)
    , isPanning_(false)
    , width_(800)
    , height_(600)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

GLWidget::~GLWidget() {
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void GLWidget::resizeGL(int w, int h) {
    width_ = w;
    height_ = h;
    glViewport(0, 0, w, h);
    UpdateProjection();
}

void GLWidget::UpdateProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // 2D orthographic projection
    double aspectRatio = static_cast<double>(width_) / static_cast<double>(height_);
    double rangeX = viewMaxX_ - viewMinX_;
    double rangeY = viewMaxY_ - viewMinY_;
    double centerX = (viewMinX_ + viewMaxX_) / 2.0;
    double centerY = (viewMinY_ + viewMaxY_) / 2.0;
    
    // Adjust for aspect ratio
    if (aspectRatio > 1.0) {
        double halfWidth = rangeX / 2.0 * aspectRatio;
        glOrtho(centerX - halfWidth, centerX + halfWidth,
                centerY - rangeY / 2.0, centerY + rangeY / 2.0,
                -1.0, 1.0);
    } else {
        double halfHeight = rangeY / 2.0 / aspectRatio;
        glOrtho(centerX - rangeX / 2.0, centerX + rangeX / 2.0,
                centerY - halfHeight, centerY + halfHeight,
                -1.0, 1.0);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    DrawGrid();
    DrawAxes();
    
    for (const auto& func : functions_) {
        DrawFunction(*func);
    }
}

void GLWidget::DrawAxes() {
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    
    // X-axis
    glBegin(GL_LINES);
    glVertex2d(viewMinX_, 0.0);
    glVertex2d(viewMaxX_, 0.0);
    glEnd();
    
    // Y-axis
    glBegin(GL_LINES);
    glVertex2d(0.0, viewMinY_);
    glVertex2d(0.0, viewMaxY_);
    glEnd();
    
    // Draw tick marks
    double rangeX = viewMaxX_ - viewMinX_;
    double rangeY = viewMaxY_ - viewMinY_;
    double tickSize = std::min(rangeX, rangeY) * 0.01;
    
    // X-axis ticks
    int numTicksX = 10;
    double stepX = rangeX / numTicksX;
    for (int i = 0; i <= numTicksX; i++) {
        double x = viewMinX_ + i * stepX;
        glBegin(GL_LINES);
        glVertex2d(x, -tickSize);
        glVertex2d(x, tickSize);
        glEnd();
    }
    
    // Y-axis ticks
    int numTicksY = 10;
    double stepY = rangeY / numTicksY;
    for (int i = 0; i <= numTicksY; i++) {
        double y = viewMinY_ + i * stepY;
        glBegin(GL_LINES);
        glVertex2d(-tickSize, y);
        glVertex2d(tickSize, y);
        glEnd();
    }
}

void GLWidget::DrawGrid() {
    glLineWidth(1.0f);
    glColor3f(0.9f, 0.9f, 0.9f);
    
    double rangeX = viewMaxX_ - viewMinX_;
    double rangeY = viewMaxY_ - viewMinY_;
    
    // Vertical grid lines
    int numGridX = 20;
    double stepX = rangeX / numGridX;
    for (int i = 0; i <= numGridX; i++) {
        double x = viewMinX_ + i * stepX;
        glBegin(GL_LINES);
        glVertex2d(x, viewMinY_);
        glVertex2d(x, viewMaxY_);
        glEnd();
    }
    
    // Horizontal grid lines
    int numGridY = 20;
    double stepY = rangeY / numGridY;
    for (int i = 0; i <= numGridY; i++) {
        double y = viewMinY_ + i * stepY;
        glBegin(GL_LINES);
        glVertex2d(viewMinX_, y);
        glVertex2d(viewMaxX_, y);
        glEnd();
    }
}

void GLWidget::DrawFunction(const LoadedRealFunction& func) {
    const auto& points = func.GetPoints();
    if (points.empty()) return;
    
    const Color& color = func.GetColor();
    glColor3f(color.r, color.g, color.b);
    glLineWidth(2.0f);
    
    glBegin(GL_LINE_STRIP);
    for (const auto& point : points) {
        glVertex2d(point.x, point.y);
    }
    glEnd();
}

void GLWidget::AddFunction(std::unique_ptr<LoadedRealFunction> func) {
    functions_.push_back(std::move(func));
    CalculateBounds();
    update();
}

void GLWidget::ClearFunctions() {
    functions_.clear();
    ResetView();
    update();
}

void GLWidget::CalculateBounds() {
    if (functions_.empty()) {
        defaultMinX_ = -10.0;
        defaultMaxX_ = 10.0;
        defaultMinY_ = -10.0;
        defaultMaxY_ = 10.0;
        return;
    }
    
    double minX = functions_[0]->GetMinX();
    double maxX = functions_[0]->GetMaxX();
    double minY = functions_[0]->GetMinY();
    double maxY = functions_[0]->GetMaxY();
    
    for (const auto& func : functions_) {
        minX = std::min(minX, func->GetMinX());
        maxX = std::max(maxX, func->GetMaxX());
        minY = std::min(minY, func->GetMinY());
        maxY = std::max(maxY, func->GetMaxY());
    }
    
    // Add padding
    double rangeX = maxX - minX;
    double rangeY = maxY - minY;
    double padX = rangeX * 0.1;
    double padY = rangeY * 0.1;
    
    defaultMinX_ = minX - padX;
    defaultMaxX_ = maxX + padX;
    defaultMinY_ = minY - padY;
    defaultMaxY_ = maxY + padY;
    
    viewMinX_ = defaultMinX_;
    viewMaxX_ = defaultMaxX_;
    viewMinY_ = defaultMinY_;
    viewMaxY_ = defaultMaxY_;
    
    UpdateProjection();
}

void GLWidget::ResetView() {
    viewMinX_ = defaultMinX_;
    viewMaxX_ = defaultMaxX_;
    viewMinY_ = defaultMinY_;
    viewMaxY_ = defaultMaxY_;
    UpdateProjection();
    update();
}

void GLWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        isPanning_ = true;
        lastMousePos_ = event->pos();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!isPanning_) return;
    
    QPoint delta = event->pos() - lastMousePos_;
    lastMousePos_ = event->pos();
    
    // Convert pixel delta to world coordinates
    double rangeX = viewMaxX_ - viewMinX_;
    double rangeY = viewMaxY_ - viewMinY_;
    
    double dx = -delta.x() * rangeX / width_;
    double dy = delta.y() * rangeY / height_;
    
    viewMinX_ += dx;
    viewMaxX_ += dx;
    viewMinY_ += dy;
    viewMaxY_ += dy;
    
    UpdateProjection();
    update();
}

void GLWidget::wheelEvent(QWheelEvent* event) {
    double zoomFactor = event->angleDelta().y() > 0 ? 0.9 : 1.1;
    
    double centerX = (viewMinX_ + viewMaxX_) / 2.0;
    double centerY = (viewMinY_ + viewMaxY_) / 2.0;
    double rangeX = (viewMaxX_ - viewMinX_) * zoomFactor;
    double rangeY = (viewMaxY_ - viewMinY_) * zoomFactor;
    
    viewMinX_ = centerX - rangeX / 2.0;
    viewMaxX_ = centerX + rangeX / 2.0;
    viewMinY_ = centerY - rangeY / 2.0;
    viewMaxY_ = centerY + rangeY / 2.0;
    
    UpdateProjection();
    update();
    
    event->accept();
}
