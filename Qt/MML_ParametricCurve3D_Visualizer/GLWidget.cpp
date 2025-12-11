#define NOMINMAX
#include "GLWidget.h"
#include <QWheelEvent>
#include <QtMath>
#include <algorithm>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , cameraDistance_(5.0f)
    , cameraRotationX_(30.0f)
    , cameraRotationY_(45.0f)
    , cameraTarget_(0, 0, 0)
    , isRotating_(false)
    , isPanning_(false)
    , xMin_(-1), xMax_(1)
    , yMin_(-1), yMax_(1)
    , zMin_(-1), zMax_(1)
    , sceneRadius_(1.0) {
}

GLWidget::~GLWidget() {
}

void GLWidget::AddCurve(std::unique_ptr<LoadedParametricCurve3D> curve, const Color& color) {
    curves_.push_back({std::move(curve), color});
    UpdateBounds();
    update();
}

void GLWidget::ClearCurves() {
    curves_.clear();
    update();
}

void GLWidget::ResetCamera() {
    cameraDistance_ = sceneRadius_ * 3.0f;
    cameraRotationX_ = 30.0f;
    cameraRotationY_ = 45.0f;
    cameraTarget_ = QVector3D((xMin_ + xMax_) / 2, (yMin_ + yMax_) / 2, (zMin_ + zMax_) / 2);
    update();
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    
    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(2.0f);
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    SetupCamera();
    
    // Apply view transformation
    QMatrix4x4 modelViewMatrix = viewMatrix_;
    
    // Draw grid and axes
    DrawGrid();
    DrawAxes();
    
    // Draw all curves
    for (const auto& curveData : curves_) {
        DrawCurve(curveData.curve.get(), curveData.color);
    }
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    
    projectionMatrix_.setToIdentity();
    float aspect = float(w) / float(h ? h : 1);
    projectionMatrix_.perspective(45.0f, aspect, 0.1f, 1000.0f);
}

void GLWidget::SetupCamera() {
    viewMatrix_.setToIdentity();
    viewMatrix_.translate(0, 0, -cameraDistance_);
    viewMatrix_.rotate(cameraRotationX_, 1, 0, 0);
    viewMatrix_.rotate(cameraRotationY_, 0, 1, 0);
    viewMatrix_.translate(-cameraTarget_);
}

void GLWidget::DrawCurve(const LoadedParametricCurve3D* curve, const Color& color) {
    if (!curve || curve->GetNumPoints() < 2) return;
    
    glColor3f(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f);
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix_.constData());
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(viewMatrix_.constData());
    
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < curve->GetNumPoints(); ++i) {
        Point3D p = curve->GetPoint(i);
        glVertex3d(p.x, p.y, p.z);
    }
    glEnd();
    
    // Draw points
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < curve->GetNumPoints(); ++i) {
        Point3D p = curve->GetPoint(i);
        glVertex3d(p.x, p.y, p.z);
    }
    glEnd();
}

void GLWidget::DrawAxes() {
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix_.constData());
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(viewMatrix_.constData());
    
    glLineWidth(2.0f);
    
    double axisLength = sceneRadius_ * 1.5;
    
    // X axis - Red
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(axisLength, 0, 0);
    glEnd();
    
    // Y axis - Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(0, axisLength, 0);
    glEnd();
    
    // Z axis - Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, axisLength);
    glEnd();
}

void GLWidget::DrawGrid() {
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix_.constData());
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(viewMatrix_.constData());
    
    glLineWidth(1.0f);
    glColor3f(0.7f, 0.7f, 0.7f);
    
    double gridSize = sceneRadius_ * 2.0;
    int gridLines = 10;
    double step = gridSize / gridLines;
    
    // XY plane grid
    glBegin(GL_LINES);
    for (int i = -gridLines; i <= gridLines; ++i) {
        double offset = i * step;
        // Lines parallel to X axis
        glVertex3d(-gridSize, offset, 0);
        glVertex3d(gridSize, offset, 0);
        // Lines parallel to Y axis
        glVertex3d(offset, -gridSize, 0);
        glVertex3d(offset, gridSize, 0);
    }
    glEnd();
}

void GLWidget::UpdateBounds() {
    if (curves_.empty()) {
        xMin_ = yMin_ = zMin_ = -1.0;
        xMax_ = yMax_ = zMax_ = 1.0;
        sceneRadius_ = 1.0;
        return;
    }
    
    double xMin, xMax, yMin, yMax, zMin, zMax;
    curves_[0].curve->GetBounds(xMin, xMax, yMin, yMax, zMin, zMax);
    
    xMin_ = xMin;
    xMax_ = xMax;
    yMin_ = yMin;
    yMax_ = yMax;
    zMin_ = zMin;
    zMax_ = zMax;
    
    // Expand bounds for all curves
    for (size_t i = 1; i < curves_.size(); ++i) {
        curves_[i].curve->GetBounds(xMin, xMax, yMin, yMax, zMin, zMax);
        xMin_ = std::min(xMin_, xMin);
        xMax_ = std::max(xMax_, xMax);
        yMin_ = std::min(yMin_, yMin);
        yMax_ = std::max(yMax_, yMax);
        zMin_ = std::min(zMin_, zMin);
        zMax_ = std::max(zMax_, zMax);
    }
    
    // Calculate scene radius
    double dx = xMax_ - xMin_;
    double dy = yMax_ - yMin_;
    double dz = zMax_ - zMin_;
    sceneRadius_ = std::sqrt(dx * dx + dy * dy + dz * dz) / 2.0;
    
    ResetCamera();
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    lastMousePos_ = event->pos();
    
    if (event->button() == Qt::LeftButton) {
        isRotating_ = true;
    } else if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        isPanning_ = true;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->pos().x() - lastMousePos_.x();
    int dy = event->pos().y() - lastMousePos_.y();
    
    if (isRotating_) {
        cameraRotationY_ += dx * 0.5f;
        cameraRotationX_ += dy * 0.5f;
        
        // Clamp vertical rotation
        cameraRotationX_ = qBound(-89.0f, cameraRotationX_, 89.0f);
        
        update();
    } else if (isPanning_) {
        float panSpeed = sceneRadius_ * 0.002f;
        
        QVector3D right(1, 0, 0);
        QVector3D up(0, 1, 0);
        
        QMatrix4x4 rotation;
        rotation.rotate(cameraRotationY_, 0, 1, 0);
        rotation.rotate(cameraRotationX_, 1, 0, 0);
        
        right = rotation * right;
        up = rotation * up;
        
        cameraTarget_ -= right * dx * panSpeed;
        cameraTarget_ += up * dy * panSpeed;
        
        update();
    }
    
    lastMousePos_ = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    float delta = event->angleDelta().y() / 120.0f;
    cameraDistance_ -= delta * sceneRadius_ * 0.1f;
    
    // Clamp distance
    cameraDistance_ = qBound(sceneRadius_ * 0.5f, cameraDistance_, sceneRadius_ * 10.0f);
    
    update();
}
