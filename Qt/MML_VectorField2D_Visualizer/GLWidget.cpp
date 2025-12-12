#include "GLWidget.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , xMin_(-1), xMax_(1)
    , yMin_(-1), yMax_(1)
    , zoom_(1.0)
    , offsetX_(0)
    , offsetY_(0)
    , arrowScale_(0.5)
    , normalizeVectors_(false)
    , showMagnitudeColor_(true)
    , maxMagnitude_(1.0)
    , isPanning_(false)
{
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget() {
}

void GLWidget::LoadVectorField(std::unique_ptr<VectorField2D> field) {
    vectorField_ = std::move(field);
    
    // Get bounds from vector field
    vectorField_->GetBounds(xMin_, xMax_, yMin_, yMax_);
    maxMagnitude_ = vectorField_->GetMaxMagnitude();
    
    // Add some margin
    double xMargin = (xMax_ - xMin_) * 0.1;
    double yMargin = (yMax_ - yMin_) * 0.1;
    xMin_ -= xMargin;
    xMax_ += xMargin;
    yMin_ -= yMargin;
    yMax_ += yMargin;
    
    // Reset view
    zoom_ = 1.0;
    offsetX_ = 0;
    offsetY_ = 0;
    
    update();
}

void GLWidget::ClearVectorField() {
    vectorField_.reset();
    xMin_ = -1; xMax_ = 1;
    yMin_ = -1; yMax_ = 1;
    zoom_ = 1.0;
    offsetX_ = 0;
    offsetY_ = 0;
    update();
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Calculate view bounds with aspect ratio
    int w = width();
    int h = height();
    if (w == 0 || h == 0) return;
    
    double dataWidth = xMax_ - xMin_;
    double dataHeight = yMax_ - yMin_;
    double aspectRatio = dataWidth / dataHeight;
    double windowRatio = static_cast<double>(w) / h;
    
    double viewWidth = dataWidth / zoom_;
    double viewHeight = dataHeight / zoom_;
    
    if (aspectRatio > windowRatio) {
        viewHeight = viewWidth / windowRatio;
    } else {
        viewWidth = viewHeight * windowRatio;
    }
    
    double centerX = (xMin_ + xMax_) / 2.0 + offsetX_;
    double centerY = (yMin_ + yMax_) / 2.0 + offsetY_;
    
    glOrtho(centerX - viewWidth / 2.0, centerX + viewWidth / 2.0,
            centerY - viewHeight / 2.0, centerY + viewHeight / 2.0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    DrawGrid();
    DrawAxes();
    DrawVectors();
}

void GLWidget::DrawGrid() {
    glColor3f(0.9f, 0.9f, 0.9f);
    glLineWidth(1.0f);
    
    double gridSpacing = std::pow(10, std::floor(std::log10((xMax_ - xMin_) / 10)));
    
    glBegin(GL_LINES);
    // Vertical lines
    for (double x = std::floor(xMin_ / gridSpacing) * gridSpacing; x <= xMax_; x += gridSpacing) {
        glVertex2d(x, yMin_);
        glVertex2d(x, yMax_);
    }
    // Horizontal lines
    for (double y = std::floor(yMin_ / gridSpacing) * gridSpacing; y <= yMax_; y += gridSpacing) {
        glVertex2d(xMin_, y);
        glVertex2d(xMax_, y);
    }
    glEnd();
}

void GLWidget::DrawAxes() {
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(2.0f);
    
    glBegin(GL_LINES);
    // X-axis
    if (yMin_ <= 0 && yMax_ >= 0) {
        glVertex2d(xMin_, 0);
        glVertex2d(xMax_, 0);
    }
    // Y-axis
    if (xMin_ <= 0 && xMax_ >= 0) {
        glVertex2d(0, yMin_);
        glVertex2d(0, yMax_);
    }
    glEnd();
}

void GLWidget::DrawVectors() {
    if (!vectorField_) return;
    
    const auto& vectors = vectorField_->GetVectors();
    
    for (const auto& v : vectors) {
        double magnitude = v.vector.Magnitude();
        DrawArrow(v.position.x, v.position.y, v.vector.x, v.vector.y, magnitude);
    }
}

void GLWidget::DrawArrow(double px, double py, double vx, double vy, double magnitude) {
    // Determine arrow vector
    double arrowVx = vx;
    double arrowVy = vy;
    
    if (normalizeVectors_ && magnitude > 0) {
        arrowVx = vx / magnitude;
        arrowVy = vy / magnitude;
    }
    
    // Scale arrow
    arrowVx *= arrowScale_;
    arrowVy *= arrowScale_;
    
    // Color by magnitude if enabled
    if (showMagnitudeColor_ && maxMagnitude_ > 0) {
        QColor color = GetMagnitudeColor(magnitude, maxMagnitude_);
        glColor3f(color.redF(), color.greenF(), color.blueF());
    } else {
        glColor3f(0.0f, 0.0f, 0.0f);
    }
    
    // Draw arrow shaft
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2d(px, py);
    glVertex2d(px + arrowVx, py + arrowVy);
    glEnd();
    
    // Draw arrowhead
    if (magnitude > 0) {
        double angle = std::atan2(arrowVy, arrowVx);
        double arrowHeadLength = (xMax_ - xMin_) * 0.02;
        double arrowHeadAngle = M_PI / 6;  // 30 degrees
        
        double x2 = px + arrowVx;
        double y2 = py + arrowVy;
        
        double x1 = x2 - arrowHeadLength * std::cos(angle + arrowHeadAngle);
        double y1 = y2 - arrowHeadLength * std::sin(angle + arrowHeadAngle);
        double x3 = x2 - arrowHeadLength * std::cos(angle - arrowHeadAngle);
        double y3 = y2 - arrowHeadLength * std::sin(angle - arrowHeadAngle);
        
        glBegin(GL_TRIANGLES);
        glVertex2d(x2, y2);
        glVertex2d(x1, y1);
        glVertex2d(x3, y3);
        glEnd();
    }
}

QColor GLWidget::GetMagnitudeColor(double magnitude, double maxMagnitude) const {
    // Color gradient: Blue (low) -> Green -> Yellow -> Red (high)
    if (maxMagnitude <= 0) return QColor(0, 0, 0);
    
    double normalized = magnitude / maxMagnitude;
    normalized = std::min(1.0, std::max(0.0, normalized));
    
    int r, g, b;
    if (normalized < 0.25) {
        // Blue to Cyan
        double t = normalized / 0.25;
        r = 0;
        g = static_cast<int>(t * 255);
        b = 255;
    } else if (normalized < 0.5) {
        // Cyan to Green
        double t = (normalized - 0.25) / 0.25;
        r = 0;
        g = 255;
        b = static_cast<int>((1 - t) * 255);
    } else if (normalized < 0.75) {
        // Green to Yellow
        double t = (normalized - 0.5) / 0.25;
        r = static_cast<int>(t * 255);
        g = 255;
        b = 0;
    } else {
        // Yellow to Red
        double t = (normalized - 0.75) / 0.25;
        r = 255;
        g = static_cast<int>((1 - t) * 255);
        b = 0;
    }
    
    return QColor(r, g, b);
}

void GLWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePos_ = event->pos();
        isPanning_ = true;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
    if (isPanning_) {
        QPoint delta = event->pos() - lastMousePos_;
        
        double viewWidth = (xMax_ - xMin_) / zoom_;
        double viewHeight = (yMax_ - yMin_) / zoom_;
        
        int w = width();
        int h = height();
        if (w > 0 && h > 0) {
            double dataWidth = xMax_ - xMin_;
            double dataHeight = yMax_ - yMin_;
            double aspectRatio = dataWidth / dataHeight;
            double windowRatio = static_cast<double>(w) / h;
            
            if (aspectRatio > windowRatio) {
                viewHeight = viewWidth / windowRatio;
            } else {
                viewWidth = viewHeight * windowRatio;
            }
            
            offsetX_ -= delta.x() * viewWidth / w;
            offsetY_ += delta.y() * viewHeight / h;
        }
        
        lastMousePos_ = event->pos();
        update();
    }
}

void GLWidget::wheelEvent(QWheelEvent* event) {
    double zoomFactor = 1.1;
    if (event->angleDelta().y() > 0) {
        zoom_ *= zoomFactor;
    } else {
        zoom_ /= zoomFactor;
    }
    update();
}
