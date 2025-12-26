#include "GLWidget.h"
#include <GL/gl.h>
#include <cmath>
#include <algorithm>

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , dataMinX_(-10.0), dataMaxX_(10.0)
    , dataMinY_(-10.0), dataMaxY_(10.0)
    , dataMinT_(0.0), dataMaxT_(1.0)
    , viewMinX_(-10.0), viewMaxX_(10.0)
    , viewMinY_(-10.0), viewMaxY_(10.0)
    , displayMinX_(-10.0), displayMaxX_(10.0)
    , displayMinY_(-10.0), displayMaxY_(10.0)
    , defaultMinX_(-10.0), defaultMaxX_(10.0)
    , defaultMinY_(-10.0), defaultMaxY_(10.0)
    , showGrid_(true)
    , showLabels_(true)
    , preserveAspectRatio_(true)  // Default true for parametric curves
    , animationRunning_(false)
    , currentAnimationFrame_(0)
    , maxAnimationFrames_(0)
    , animationSpeed_(10.0)
    , isPanning_(false)
    , glInitialized_(false)
    , width_(800)
    , height_(600)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    
    // Initialize tick info for default view
    auto [xTicks, yTicks] = AxisTickCalculator::CalculateAxisTicks(
        viewMinX_, viewMaxX_, viewMinY_, viewMaxY_, 10, 8);
    xTickInfo_ = xTicks;
    yTickInfo_ = yTicks;
    
    // Create animation timer
    animationTimer_ = new QTimer(this);
    connect(animationTimer_, &QTimer::timeout, this, &GLWidget::OnAnimationTimer);
}

GLWidget::~GLWidget() {
    StopAnimation();
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glInitialized_ = true;
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void GLWidget::resizeGL(int w, int h) {
    width_ = w;
    height_ = h;
}

void GLWidget::SetupProjection() {
    // Calculate the drawable area (excluding margins)
    int drawWidth = width_ - MARGIN_LEFT - MARGIN_RIGHT;
    int drawHeight = height_ - MARGIN_TOP - MARGIN_BOTTOM;
    
    if (drawWidth <= 0) drawWidth = 1;
    if (drawHeight <= 0) drawHeight = 1;
    
    glViewport(MARGIN_LEFT, MARGIN_BOTTOM, drawWidth, drawHeight);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    double rangeX = viewMaxX_ - viewMinX_;
    double rangeY = viewMaxY_ - viewMinY_;
    
    // Default: display bounds equal view bounds
    displayMinX_ = viewMinX_;
    displayMaxX_ = viewMaxX_;
    displayMinY_ = viewMinY_;
    displayMaxY_ = viewMaxY_;
    
    if (preserveAspectRatio_) {
        double aspectRatio = static_cast<double>(drawWidth) / static_cast<double>(drawHeight);
        double dataAspect = rangeX / rangeY;
        
        double centerX = (viewMinX_ + viewMaxX_) / 2.0;
        double centerY = (viewMinY_ + viewMaxY_) / 2.0;
        
        if (dataAspect > aspectRatio) {
            // Data is wider - adjust Y range
            double newRangeY = rangeX / aspectRatio;
            displayMinY_ = centerY - newRangeY / 2.0;
            displayMaxY_ = centerY + newRangeY / 2.0;
        } else {
            // Data is taller - adjust X range
            double newRangeX = rangeY * aspectRatio;
            displayMinX_ = centerX - newRangeX / 2.0;
            displayMaxX_ = centerX + newRangeX / 2.0;
        }
    }
    
    glOrtho(displayMinX_, displayMaxX_, displayMinY_, displayMaxY_, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    SetupProjection();
    
    // Draw OpenGL elements
    if (showGrid_) {
        DrawGrid();
    }
    DrawAxes();
    
    // Draw all visible curves
    for (const auto& curve : curves_) {
        if (curve && curve->IsVisible()) {
            DrawCurve(*curve);
        }
    }
    
    // Draw labels and animation markers using QPainter overlay
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (showLabels_) {
        DrawAxisLabels(painter);
    }
    
    if (animationRunning_ || currentAnimationFrame_ > 0) {
        DrawAnimationMarkers(painter);
    }
    
    painter.end();
}

void GLWidget::DrawGrid() {
    glLineWidth(1.0f);
    glColor3f(0.9f, 0.9f, 0.9f);
    
    // Draw vertical grid lines at X tick positions
    glBegin(GL_LINES);
    for (const auto& tick : xTickInfo_.ticks) {
        glVertex2d(tick.value, displayMinY_);
        glVertex2d(tick.value, displayMaxY_);
    }
    glEnd();
    
    // Draw horizontal grid lines at Y tick positions
    glBegin(GL_LINES);
    for (const auto& tick : yTickInfo_.ticks) {
        glVertex2d(displayMinX_, tick.value);
        glVertex2d(displayMaxX_, tick.value);
    }
    glEnd();
}

void GLWidget::DrawAxes() {
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    
    // X-axis (at y=0 if in range, otherwise at bottom)
    double xAxisY = (displayMinY_ <= 0 && displayMaxY_ >= 0) ? 0 : displayMinY_;
    glBegin(GL_LINES);
    glVertex2d(displayMinX_, xAxisY);
    glVertex2d(displayMaxX_, xAxisY);
    glEnd();
    
    // Y-axis (at x=0 if in range, otherwise at left)
    double yAxisX = (displayMinX_ <= 0 && displayMaxX_ >= 0) ? 0 : displayMinX_;
    glBegin(GL_LINES);
    glVertex2d(yAxisX, displayMinY_);
    glVertex2d(yAxisX, displayMaxY_);
    glEnd();
}

void GLWidget::DrawAxisLabels(QPainter& painter) {
    painter.setFont(QFont("Helvetica", 9));
    painter.setPen(Qt::black);
    
    int drawWidth = width_ - MARGIN_LEFT - MARGIN_RIGHT;
    int drawHeight = height_ - MARGIN_TOP - MARGIN_BOTTOM;
    
    double rangeX = displayMaxX_ - displayMinX_;
    double rangeY = displayMaxY_ - displayMinY_;
    
    // X-axis labels
    for (const auto& tick : xTickInfo_.ticks) {
        double normX = (tick.value - displayMinX_) / rangeX;
        int screenX = MARGIN_LEFT + static_cast<int>(normX * drawWidth);
        int screenY = height_ - MARGIN_BOTTOM + 15;
        
        QString label = QString::fromStdString(tick.label);
        QFontMetrics fm(painter.font());
        int textWidth = fm.horizontalAdvance(label);
        
        painter.drawText(screenX - textWidth / 2, screenY, label);
    }
    
    // Y-axis labels
    for (const auto& tick : yTickInfo_.ticks) {
        double normY = (tick.value - displayMinY_) / rangeY;
        int screenX = MARGIN_LEFT - 5;
        int screenY = height_ - MARGIN_BOTTOM - static_cast<int>(normY * drawHeight);
        
        QString label = QString::fromStdString(tick.label);
        QFontMetrics fm(painter.font());
        int textWidth = fm.horizontalAdvance(label);
        
        painter.drawText(screenX - textWidth, screenY + 4, label);
    }
}

void GLWidget::DrawCurve(const LoadedParamCurve2D& curve) {
    const auto& xVals = curve.GetXVals();
    const auto& yVals = curve.GetYVals();
    
    if (xVals.size() < 2) return;
    
    Color color = curve.GetColor();
    glColor3f(color.r, color.g, color.b);
    glLineWidth(2.0f);
    
    // Draw curve as connected line segments
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < xVals.size(); ++i) {
        glVertex2d(xVals[i], yVals[i]);
    }
    glEnd();
}

void GLWidget::DrawAnimationMarkers(QPainter& painter) {
    int drawWidth = width_ - MARGIN_LEFT - MARGIN_RIGHT;
    int drawHeight = height_ - MARGIN_TOP - MARGIN_BOTTOM;
    
    double rangeX = displayMaxX_ - displayMinX_;
    double rangeY = displayMaxY_ - displayMinY_;
    
    for (const auto& curve : curves_) {
        if (!curve || !curve->IsVisible()) continue;
        if (currentAnimationFrame_ >= curve->GetNumPoints()) continue;
        
        double x, y;
        if (!curve->GetPointAt(currentAnimationFrame_, x, y)) continue;
        
        // Convert to screen coordinates
        double normX = (x - displayMinX_) / rangeX;
        double normY = (y - displayMinY_) / rangeY;
        int screenX = MARGIN_LEFT + static_cast<int>(normX * drawWidth);
        int screenY = height_ - MARGIN_BOTTOM - static_cast<int>(normY * drawHeight);
        
        // Draw filled circle marker
        Color color = curve->GetColor();
        QColor qColor(static_cast<int>(color.r * 255), 
                      static_cast<int>(color.g * 255), 
                      static_cast<int>(color.b * 255));
        
        painter.setBrush(qColor);
        painter.setPen(QPen(Qt::black, 1));
        painter.drawEllipse(QPoint(screenX, screenY), 6, 6);
    }
}

QColor GLWidget::GetCurveColor(int index) const {
    Color color = GetColorByIndex(index);
    return QColor(static_cast<int>(color.r * 255),
                  static_cast<int>(color.g * 255),
                  static_cast<int>(color.b * 255));
}

void GLWidget::AddCurve(std::unique_ptr<LoadedParamCurve2D> curve) {
    curves_.push_back(std::move(curve));
    CalculateBounds();
    
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
    
    // Reset to default view
    viewMinX_ = defaultMinX_ = -10.0;
    viewMaxX_ = defaultMaxX_ = 10.0;
    viewMinY_ = defaultMinY_ = -10.0;
    viewMaxY_ = defaultMaxY_ = 10.0;
    
    auto [xTicks, yTicks] = AxisTickCalculator::CalculateAxisTicks(
        viewMinX_, viewMaxX_, viewMinY_, viewMaxY_, 10, 8);
    xTickInfo_ = xTicks;
    yTickInfo_ = yTicks;
    
    update();
    emit boundsChanged();
}

void GLWidget::CalculateBounds() {
    if (curves_.empty()) {
        dataMinX_ = dataMinY_ = -10.0;
        dataMaxX_ = dataMaxY_ = 10.0;
        dataMinT_ = 0; dataMaxT_ = 1;
        return;
    }
    
    // Initialize with first curve's bounds
    dataMinX_ = curves_[0]->GetMinX();
    dataMaxX_ = curves_[0]->GetMaxX();
    dataMinY_ = curves_[0]->GetMinY();
    dataMaxY_ = curves_[0]->GetMaxY();
    dataMinT_ = curves_[0]->GetMinT();
    dataMaxT_ = curves_[0]->GetMaxT();
    
    // Expand bounds to include all curves
    for (const auto& curve : curves_) {
        dataMinX_ = std::min(dataMinX_, curve->GetMinX());
        dataMaxX_ = std::max(dataMaxX_, curve->GetMaxX());
        dataMinY_ = std::min(dataMinY_, curve->GetMinY());
        dataMaxY_ = std::max(dataMaxY_, curve->GetMaxY());
        dataMinT_ = std::min(dataMinT_, curve->GetMinT());
        dataMaxT_ = std::max(dataMaxT_, curve->GetMaxT());
    }
    
    // Handle equal bounds
    if (std::abs(dataMaxX_ - dataMinX_) < 1e-10) {
        dataMinX_ -= 1; dataMaxX_ += 1;
    }
    if (std::abs(dataMaxY_ - dataMinY_) < 1e-10) {
        dataMinY_ -= 1; dataMaxY_ += 1;
    }
    
    // Add 5% padding and calculate nice tick values
    double xPadding = (dataMaxX_ - dataMinX_) * 0.05;
    double yPadding = (dataMaxY_ - dataMinY_) * 0.05;
    
    auto [xTicks, yTicks] = AxisTickCalculator::CalculateAxisTicks(
        dataMinX_ - xPadding, dataMaxX_ + xPadding,
        dataMinY_ - yPadding, dataMaxY_ + yPadding,
        10, 8);
    
    xTickInfo_ = xTicks;
    yTickInfo_ = yTicks;
    
    // Set view and default bounds from nice tick bounds
    viewMinX_ = defaultMinX_ = xTickInfo_.min;
    viewMaxX_ = defaultMaxX_ = xTickInfo_.max;
    viewMinY_ = defaultMinY_ = yTickInfo_.min;
    viewMaxY_ = defaultMaxY_ = yTickInfo_.max;
}

void GLWidget::ResetView() {
    viewMinX_ = defaultMinX_;
    viewMaxX_ = defaultMaxX_;
    viewMinY_ = defaultMinY_;
    viewMaxY_ = defaultMaxY_;
    update();
    emit boundsChanged();
}

// Display settings
void GLWidget::SetGridVisible(bool visible) {
    showGrid_ = visible;
    update();
}

void GLWidget::SetLabelsVisible(bool visible) {
    showLabels_ = visible;
    update();
}

void GLWidget::SetPreserveAspectRatio(bool preserve) {
    preserveAspectRatio_ = preserve;
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

// Visibility
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

// Mouse events
void GLWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isPanning_ = true;
        lastMousePos_ = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
    if (isPanning_) {
        QPoint delta = event->pos() - lastMousePos_;
        lastMousePos_ = event->pos();
        
        // Calculate pan distance in world coordinates
        int drawWidth = width_ - MARGIN_LEFT - MARGIN_RIGHT;
        int drawHeight = height_ - MARGIN_TOP - MARGIN_BOTTOM;
        
        double rangeX = displayMaxX_ - displayMinX_;
        double rangeY = displayMaxY_ - displayMinY_;
        
        double dx = -delta.x() * rangeX / drawWidth;
        double dy = delta.y() * rangeY / drawHeight;
        
        viewMinX_ += dx;
        viewMaxX_ += dx;
        viewMinY_ += dy;
        viewMaxY_ += dy;
        
        update();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isPanning_ = false;
        setCursor(Qt::ArrowCursor);
    }
}

void GLWidget::wheelEvent(QWheelEvent* event) {
    double zoomFactor = 1.1;
    
    double centerX = (viewMinX_ + viewMaxX_) / 2.0;
    double centerY = (viewMinY_ + viewMaxY_) / 2.0;
    
    if (event->angleDelta().y() > 0) {
        // Zoom in
        double rangeX = (viewMaxX_ - viewMinX_) / zoomFactor;
        double rangeY = (viewMaxY_ - viewMinY_) / zoomFactor;
        
        viewMinX_ = centerX - rangeX / 2.0;
        viewMaxX_ = centerX + rangeX / 2.0;
        viewMinY_ = centerY - rangeY / 2.0;
        viewMaxY_ = centerY + rangeY / 2.0;
    } else {
        // Zoom out
        double rangeX = (viewMaxX_ - viewMinX_) * zoomFactor;
        double rangeY = (viewMaxY_ - viewMinY_) * zoomFactor;
        
        viewMinX_ = centerX - rangeX / 2.0;
        viewMaxX_ = centerX + rangeX / 2.0;
        viewMinY_ = centerY - rangeY / 2.0;
        viewMaxY_ = centerY + rangeY / 2.0;
    }
    
    update();
}
