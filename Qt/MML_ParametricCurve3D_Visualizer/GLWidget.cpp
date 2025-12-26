#define NOMINMAX
#include "GLWidget.h"
#include <QWheelEvent>
#include <QtMath>
#include <algorithm>
#include <cmath>

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
    , sceneRadius_(1.0)
    , lineWidth_(2.0f)
    , animationRunning_(false)
    , currentAnimationFrame_(0)
    , maxAnimationFrames_(0)
    , animationSpeed_(10.0)
{
    // Create animation timer
    animationTimer_ = new QTimer(this);
    connect(animationTimer_, &QTimer::timeout, this, &GLWidget::OnAnimationTimer);
}

GLWidget::~GLWidget() {
    StopAnimation();
}

void GLWidget::AddCurve(std::unique_ptr<LoadedParametricCurve3D> curve) {
    // Assign color based on index
    curve->SetColor(GetColorByIndex(curves_.size()));
    curves_.push_back(std::move(curve));
    UpdateBounds();
    
    // Update max animation frames
    maxAnimationFrames_ = 0;
    for (const auto& c : curves_) {
        maxAnimationFrames_ = std::max(maxAnimationFrames_, c->GetNumPoints());
    }
    
    update();
    emit boundsChanged();
}

void GLWidget::ClearCurves() {
    StopAnimation();
    curves_.clear();
    currentAnimationFrame_ = 0;
    maxAnimationFrames_ = 0;
    
    xMin_ = yMin_ = zMin_ = -1.0;
    xMax_ = yMax_ = zMax_ = 1.0;
    sceneRadius_ = 1.0;
    
    update();
    emit boundsChanged();
}

void GLWidget::ResetCamera() {
    cameraDistance_ = sceneRadius_ * 3.0f;
    cameraRotationX_ = 30.0f;
    cameraRotationY_ = 45.0f;
    cameraTarget_ = QVector3D((xMin_ + xMax_) / 2, (yMin_ + yMax_) / 2, (zMin_ + zMax_) / 2);
    update();
}

void GLWidget::SetCurveVisible(int index, bool visible) {
    if (index >= 0 && index < static_cast<int>(curves_.size())) {
        curves_[index]->SetVisible(visible);
        update();
    }
}

bool GLWidget::IsCurveVisible(int index) const {
    if (index >= 0 && index < static_cast<int>(curves_.size())) {
        return curves_[index]->IsVisible();
    }
    return false;
}

void GLWidget::SetLineWidth(float width) {
    lineWidth_ = std::max(0.5f, std::min(10.0f, width));
    update();
}

// Animation methods
void GLWidget::StartAnimation() {
    if (maxAnimationFrames_ == 0) return;
    
    animationRunning_ = true;
    int interval = static_cast<int>(1000.0 / animationSpeed_);
    animationTimer_->start(interval);
    update();
}

void GLWidget::PauseAnimation() {
    animationRunning_ = false;
    animationTimer_->stop();
}

void GLWidget::ResumeAnimation() {
    if (maxAnimationFrames_ == 0) return;
    
    animationRunning_ = true;
    int interval = static_cast<int>(1000.0 / animationSpeed_);
    animationTimer_->start(interval);
}

void GLWidget::ResetAnimation() {
    currentAnimationFrame_ = 0;
    update();
}

void GLWidget::StopAnimation() {
    animationRunning_ = false;
    animationTimer_->stop();
}

void GLWidget::SetAnimationSpeed(double pointsPerSecond) {
    animationSpeed_ = std::max(0.1, pointsPerSecond);
    
    if (animationRunning_) {
        int interval = static_cast<int>(1000.0 / animationSpeed_);
        animationTimer_->setInterval(interval);
    }
}

void GLWidget::OnAnimationTimer() {
    if (!animationRunning_) return;
    
    currentAnimationFrame_++;
    
    // Loop animation
    if (currentAnimationFrame_ >= maxAnimationFrames_) {
        currentAnimationFrame_ = 0;
    }
    
    update();
    
    if (animationCallback_) {
        animationCallback_();
    }
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    
    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    SetupCamera();
    
    // Draw grid and axes
    DrawGrid();
    DrawAxes();
    
    // Draw all visible curves
    glLineWidth(lineWidth_);
    for (const auto& curve : curves_) {
        if (curve && curve->IsVisible()) {
            DrawCurve(curve.get());
        }
    }
    
    // Draw animation markers
    if (animationRunning_ || currentAnimationFrame_ > 0) {
        DrawAnimationMarkers();
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

void GLWidget::DrawCurve(const LoadedParametricCurve3D* curve) {
    if (!curve || curve->GetNumPoints() < 2) return;
    
    Color color = curve->GetColor();
    glColor3f(color.r, color.g, color.b);
    
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
}

void GLWidget::DrawAnimationMarkers() {
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix_.constData());
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(viewMatrix_.constData());
    
    int curveIndex = 0;
    for (const auto& curve : curves_) {
        if (!curve || !curve->IsVisible()) {
            curveIndex++;
            continue;
        }
        if (currentAnimationFrame_ >= curve->GetNumPoints()) {
            curveIndex++;
            continue;
        }
        
        Point3D p = curve->GetPoint(currentAnimationFrame_);
        
        // First curve gets larger marker (5 units), others get 2 units
        // Scale by scene radius for visibility
        float baseRadius = (curveIndex == 0) ? 0.05f : 0.02f;
        float radius = static_cast<float>(sceneRadius_) * baseRadius;
        
        DrawSphere(p, radius, curve->GetColor());
        curveIndex++;
    }
}

void GLWidget::DrawSphere(const Point3D& center, float radius, const Color& color) {
    glColor3f(color.r, color.g, color.b);
    
    // Simple sphere approximation using triangles
    const int slices = 16;
    const int stacks = 8;
    
    for (int i = 0; i < stacks; ++i) {
        float lat0 = M_PI * (-0.5f + float(i) / stacks);
        float lat1 = M_PI * (-0.5f + float(i + 1) / stacks);
        float y0 = std::sin(lat0);
        float y1 = std::sin(lat1);
        float r0 = std::cos(lat0);
        float r1 = std::cos(lat1);
        
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2 * M_PI * float(j) / slices;
            float x = std::cos(lng);
            float z = std::sin(lng);
            
            glNormal3f(x * r0, y0, z * r0);
            glVertex3f(center.x + radius * x * r0, center.y + radius * y0, center.z + radius * z * r0);
            
            glNormal3f(x * r1, y1, z * r1);
            glVertex3f(center.x + radius * x * r1, center.y + radius * y1, center.z + radius * z * r1);
        }
        glEnd();
    }
}

void GLWidget::DrawAxes() {
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix_.constData());
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(viewMatrix_.constData());
    
    glLineWidth(lineWidth_ * 1.0f);  // Axis width = base line width
    
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
    
    // XY plane grid (at z=0)
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
    curves_[0]->GetBounds(xMin, xMax, yMin, yMax, zMin, zMax);
    
    xMin_ = xMin;
    xMax_ = xMax;
    yMin_ = yMin;
    yMax_ = yMax;
    zMin_ = zMin;
    zMax_ = zMax;
    
    // Expand bounds for all curves
    for (size_t i = 1; i < curves_.size(); ++i) {
        curves_[i]->GetBounds(xMin, xMax, yMin, yMax, zMin, zMax);
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
    
    // Ensure minimum radius
    if (sceneRadius_ < 0.1) sceneRadius_ = 1.0;
    
    ResetCamera();
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    lastMousePos_ = event->pos();
    
    if (event->button() == Qt::LeftButton) {
        isRotating_ = true;
        setCursor(Qt::ClosedHandCursor);
    } else if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        isPanning_ = true;
        setCursor(Qt::SizeAllCursor);
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
        
        right = rotation.map(right);
        up = rotation.map(up);
        
        cameraTarget_ -= right * dx * panSpeed;
        cameraTarget_ += up * dy * panSpeed;
        
        update();
    }
    
    lastMousePos_ = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isRotating_ = false;
    } else if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        isPanning_ = false;
    }
    setCursor(Qt::ArrowCursor);
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    float delta = event->angleDelta().y() / 120.0f;
    cameraDistance_ -= delta * sceneRadius_ * 0.1f;
    
    // Clamp distance
    cameraDistance_ = qBound(sceneRadius_ * 0.5f, cameraDistance_, sceneRadius_ * 10.0f);
    
    update();
}
