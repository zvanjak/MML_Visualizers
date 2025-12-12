#include "GLWidget.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , numTimesteps_(0)
    , currentTimestep_(0)
    , isPlaying_(false)
    , animationFPS_(10)
    , simWidth_(1000)
    , simHeight_(800)
    , scaleX_(1.0)
    , scaleY_(1.0)
    , offsetX_(0)
    , offsetY_(0)
    , zoom_(1.0)
    , isPanning_(false)
{
    animTimer_ = new QTimer(this);
    connect(animTimer_, &QTimer::timeout, this, &GLWidget::OnAnimationTimer);
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget() {
    delete animTimer_;
}

void GLWidget::LoadSimulation(const SimulationData& data) {
    simData_ = data;
    simWidth_ = data.width;
    simHeight_ = data.height;
    numTimesteps_ = data.numSteps;
    currentTimestep_ = 0;
    
    // Reset view
    zoom_ = 1.0;
    offsetX_ = 0;
    offsetY_ = 0;
    UpdateView();
    
    update();
}

void GLWidget::ClearSimulation() {
    simData_ = SimulationData();
    numTimesteps_ = 0;
    currentTimestep_ = 0;
    isPlaying_ = false;
    animTimer_->stop();
    update();
}

void GLWidget::Play() {
    if (numTimesteps_ > 0) {
        isPlaying_ = true;
        animTimer_->start(1000 / animationFPS_);
    }
}

void GLWidget::Pause() {
    isPlaying_ = false;
    animTimer_->stop();
}

void GLWidget::Stop() {
    isPlaying_ = false;
    animTimer_->stop();
    currentTimestep_ = 0;
    emit TimestepChanged(currentTimestep_);
    update();
}

void GLWidget::StepForward() {
    if (currentTimestep_ < numTimesteps_ - 1) {
        currentTimestep_++;
        emit TimestepChanged(currentTimestep_);
        update();
    }
}

void GLWidget::StepBackward() {
    if (currentTimestep_ > 0) {
        currentTimestep_--;
        emit TimestepChanged(currentTimestep_);
        update();
    }
}

void GLWidget::SetTimestep(int timestep) {
    if (timestep >= 0 && timestep < numTimesteps_) {
        currentTimestep_ = timestep;
        emit TimestepChanged(currentTimestep_);
        update();
    }
}

void GLWidget::SetAnimationSpeed(int fps) {
    animationFPS_ = fps;
    if (isPlaying_) {
        animTimer_->setInterval(1000 / animationFPS_);
    }
}

void GLWidget::OnAnimationTimer() {
    currentTimestep_++;
    if (currentTimestep_ >= numTimesteps_) {
        currentTimestep_ = 0;  // Loop animation
        // Or stop: Pause(); emit AnimationFinished();
    }
    emit TimestepChanged(currentTimestep_);
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
    UpdateView();
}

void GLWidget::UpdateView() {
    // View bounds will be calculated in paintGL based on window size
    // This function kept for future use if needed
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Calculate view bounds to preserve aspect ratio
    int w = width();
    int h = height();
    if (w == 0 || h == 0) return;
    
    double aspectRatio = simWidth_ / simHeight_;
    double windowRatio = static_cast<double>(w) / h;
    
    double viewWidth = simWidth_ / zoom_;
    double viewHeight = simHeight_ / zoom_;
    
    // Adjust for window aspect ratio
    if (aspectRatio > windowRatio) {
        // Simulation is wider - expand height
        viewHeight = viewWidth / windowRatio;
    } else {
        // Simulation is taller - expand width
        viewWidth = viewHeight * windowRatio;
    }
    
    // Center on simulation area with pan offset
    double centerX = simWidth_ / 2.0 + offsetX_;
    double centerY = simHeight_ / 2.0 + offsetY_;
    
    glOrtho(centerX - viewWidth / 2.0, centerX + viewWidth / 2.0,
            centerY - viewHeight / 2.0, centerY + viewHeight / 2.0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    DrawGrid();
    DrawAxes();
    DrawBalls();
}

void GLWidget::DrawGrid() {
    glColor3f(0.9f, 0.9f, 0.9f);
    glLineWidth(1.0f);
    
    double gridSpacing = 100.0;
    
    glBegin(GL_LINES);
    // Vertical lines
    for (double x = 0; x <= simWidth_; x += gridSpacing) {
        glVertex2d(x, 0);
        glVertex2d(x, simHeight_);
    }
    // Horizontal lines
    for (double y = 0; y <= simHeight_; y += gridSpacing) {
        glVertex2d(0, y);
        glVertex2d(simWidth_, y);
    }
    glEnd();
}

void GLWidget::DrawAxes() {
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(2.0f);
    
    glBegin(GL_LINES);
    // X-axis
    glVertex2d(0, 0);
    glVertex2d(simWidth_, 0);
    // Y-axis
    glVertex2d(0, 0);
    glVertex2d(0, simHeight_);
    glEnd();
}

void GLWidget::DrawBalls() {
    if (simData_.balls.empty() || currentTimestep_ >= numTimesteps_) {
        return;
    }
    
    for (const auto& ball : simData_.balls) {
        Vec2D pos = ball.GetPosition(currentTimestep_);
        DrawBall(pos.x, pos.y, ball.GetRadius(), ball.GetColor());
    }
}

void GLWidget::DrawBall(double centerX, double centerY, double radius, const std::string& colorName) {
    QColor color = GetColorFromName(colorName);
    glColor4f(color.redF(), color.greenF(), color.blueF(), 0.8f);
    
    const int segments = 32;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(centerX, centerY);
    for (int i = 0; i <= segments; i++) {
        double angle = 2.0 * M_PI * i / segments;
        double x = centerX + radius * std::cos(angle);
        double y = centerY + radius * std::sin(angle);
        glVertex2d(x, y);
    }
    glEnd();
    
    // Draw outline
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        double angle = 2.0 * M_PI * i / segments;
        double x = centerX + radius * std::cos(angle);
        double y = centerY + radius * std::sin(angle);
        glVertex2d(x, y);
    }
    glEnd();
}

QColor GLWidget::GetColorFromName(const std::string& colorName) const {
    if (colorName == "Black") return QColor(0, 0, 0);
    if (colorName == "Orange") return QColor(255, 165, 0);
    if (colorName == "Blue") return QColor(0, 0, 255);
    if (colorName == "Red") return QColor(255, 0, 0);
    if (colorName == "Green") return QColor(0, 128, 0);
    if (colorName == "Purple") return QColor(128, 0, 128);
    if (colorName == "Cyan") return QColor(0, 255, 255);
    if (colorName == "Brown") return QColor(165, 42, 42);
    if (colorName == "Magenta") return QColor(255, 0, 255);
    if (colorName == "Yellow") return QColor(255, 255, 0);
    return QColor(128, 128, 128);  // Default gray
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
        
        // Convert pixel delta to simulation coordinates
        double viewWidth = simWidth_ / zoom_;
        double viewHeight = simHeight_ / zoom_;
        
        int w = width();
        int h = height();
        if (w > 0 && h > 0) {
            double aspectRatio = simWidth_ / simHeight_;
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
    UpdateView();
    update();
}
