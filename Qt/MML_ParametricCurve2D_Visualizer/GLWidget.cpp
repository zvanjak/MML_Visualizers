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
    // Initialize curve colors (matching WPF's _brushes)
    curveColors_ = {
        QColor(0, 0, 0),         // Black
        QColor(255, 165, 0),     // Orange
        QColor(0, 0, 255),       // Blue
        QColor(255, 0, 0),       // Red
        QColor(0, 128, 0),       // Green
        QColor(128, 0, 128),     // Purple
        QColor(0, 255, 255),     // Cyan
        QColor(165, 42, 42),     // Brown
        QColor(255, 0, 255),     // Magenta
        QColor(255, 255, 0)      // Yellow
    };
    
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

GLWidget::~GLWidget() {
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // White background
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
    
    // Draw curves with their respective colors
    for (size_t i = 0; i < curves_.size(); ++i) {
        QColor color = GetCurveColor(i);
        DrawCurve(*curves_[i], color);
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

void GLWidget::DrawCurve(const LoadedParamCurve2D& curve, const QColor& color) {
    const auto& xVals = curve.GetXVals();
    const auto& yVals = curve.GetYVals();
    
    if (xVals.empty() || yVals.empty()) return;
    
    glColor3f(color.redF(), color.greenF(), color.blueF());
    glLineWidth(2.0f);
    
    // Draw curve as connected line segments (matching WPF's Draw logic)
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < xVals.size(); ++i) {
        glVertex2d(xVals[i], yVals[i]);
    }
    glEnd();
}

QColor GLWidget::GetCurveColor(int index) const {
    if (index < 0 || index >= static_cast<int>(curveColors_.size())) {
        return curveColors_[0];  // Default to black
    }
    return curveColors_[index];
}

void GLWidget::AddCurve(std::unique_ptr<LoadedParamCurve2D> curve) {
    curves_.push_back(std::move(curve));
    CalculateBounds();
    update();
}

void GLWidget::ClearCurves() {
    curves_.clear();
    ResetView();
    update();
}

void GLWidget::CalculateBounds() {
    if (curves_.empty()) {
        defaultMinX_ = -10.0;
        defaultMaxX_ = 10.0;
        defaultMinY_ = -10.0;
        defaultMaxY_ = 10.0;
        return;
    }
    
    // Calculate bounds from all curves (matching WPF's InitializeCoordSysParams)
    double minX = curves_[0]->GetMinX();
    double maxX = curves_[0]->GetMaxX();
    double minY = curves_[0]->GetMinY();
    double maxY = curves_[0]->GetMaxY();
    
    for (size_t i = 1; i < curves_.size(); ++i) {
        minX = std::min(minX, curves_[i]->GetMinX());
        maxX = std::max(maxX, curves_[i]->GetMaxX());
        minY = std::min(minY, curves_[i]->GetMinY());
        maxY = std::max(maxY, curves_[i]->GetMaxY());
    }
    
    // Add 10% padding
    double paddingX = (maxX - minX) * 0.1;
    double paddingY = (maxY - minY) * 0.1;
    
    defaultMinX_ = minX - paddingX;
    defaultMaxX_ = maxX + paddingX;
    defaultMinY_ = minY - paddingY;
    defaultMaxY_ = maxY + paddingY;
    
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
    if (event->button() == Qt::LeftButton) {
        isPanning_ = true;
        lastMousePos_ = event->pos();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
    if (isPanning_) {
        QPoint delta = event->pos() - lastMousePos_;
        lastMousePos_ = event->pos();
        
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
}

void GLWidget::wheelEvent(QWheelEvent* event) {
    double zoomFactor = 1.1;
    
    if (event->angleDelta().y() > 0) {
        // Zoom in
        double rangeX = (viewMaxX_ - viewMinX_) / zoomFactor;
        double rangeY = (viewMaxY_ - viewMinY_) / zoomFactor;
        double centerX = (viewMinX_ + viewMaxX_) / 2.0;
        double centerY = (viewMinY_ + viewMaxY_) / 2.0;
        
        viewMinX_ = centerX - rangeX / 2.0;
        viewMaxX_ = centerX + rangeX / 2.0;
        viewMinY_ = centerY - rangeY / 2.0;
        viewMaxY_ = centerY + rangeY / 2.0;
    } else {
        // Zoom out
        double rangeX = (viewMaxX_ - viewMinX_) * zoomFactor;
        double rangeY = (viewMaxY_ - viewMinY_) * zoomFactor;
        double centerX = (viewMinX_ + viewMaxX_) / 2.0;
        double centerY = (viewMinY_ + viewMaxY_) / 2.0;
        
        viewMinX_ = centerX - rangeX / 2.0;
        viewMaxX_ = centerX + rangeX / 2.0;
        viewMinY_ = centerY - rangeY / 2.0;
        viewMaxY_ = centerY + rangeY / 2.0;
    }
    
    UpdateProjection();
    update();
}
