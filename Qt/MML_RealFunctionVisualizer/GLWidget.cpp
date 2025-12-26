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
    , displayMinX_(-10.0)
    , displayMaxX_(10.0)
    , displayMinY_(-10.0)
    , displayMaxY_(10.0)
    , defaultMinX_(-10.0)
    , defaultMaxX_(10.0)
    , defaultMinY_(-10.0)
    , defaultMaxY_(10.0)
    , showGrid_(true)
    , showLabels_(true)
    , preserveAspectRatio_(false)
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
}

GLWidget::~GLWidget() {
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
    // Projection will be set up in paintGL
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
    
    // Set up projection (must be done here when context is current)
    SetupProjection();
    
    // Draw OpenGL elements
    if (showGrid_) {
        DrawGrid();
    }
    DrawAxes();
    
    // Draw all functions
    for (const auto& func : functions_) {
        if (!func->IsVisible()) continue;
        
        if (func->GetDimension() == 1) {
            auto* singleFunc = dynamic_cast<LoadedRealFunction*>(func.get());
            if (singleFunc) {
                DrawSingleFunction(*singleFunc);
            }
        } else {
            auto* multiFunc = dynamic_cast<MultiLoadedFunction*>(func.get());
            if (multiFunc) {
                DrawMultiFunction(*multiFunc);
            }
        }
    }
    
    // Draw labels using QPainter overlay
    if (showLabels_) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        DrawAxisLabels(painter);
        painter.end();
    }
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
    
    // Determine axis positions (at 0 if in range, otherwise at edge)
    double xAxisY = 0.0;
    if (displayMinY_ > 0) xAxisY = displayMinY_;
    else if (displayMaxY_ < 0) xAxisY = displayMaxY_;
    
    double yAxisX = 0.0;
    if (displayMinX_ > 0) yAxisX = displayMinX_;
    else if (displayMaxX_ < 0) yAxisX = displayMaxX_;
    
    // X-axis
    glBegin(GL_LINES);
    glVertex2d(displayMinX_, xAxisY);
    glVertex2d(displayMaxX_, xAxisY);
    glEnd();
    
    // Y-axis
    glBegin(GL_LINES);
    glVertex2d(yAxisX, displayMinY_);
    glVertex2d(yAxisX, displayMaxY_);
    glEnd();
    
    // Draw tick marks
    double tickSize = std::min(displayMaxX_ - displayMinX_, displayMaxY_ - displayMinY_) * 0.01;
    
    // X-axis ticks
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (const auto& tick : xTickInfo_.ticks) {
        glVertex2d(tick.value, xAxisY - tickSize);
        glVertex2d(tick.value, xAxisY + tickSize);
    }
    glEnd();
    
    // Y-axis ticks
    glBegin(GL_LINES);
    for (const auto& tick : yTickInfo_.ticks) {
        glVertex2d(yAxisX - tickSize, tick.value);
        glVertex2d(yAxisX + tickSize, tick.value);
    }
    glEnd();
}

void GLWidget::DrawAxisLabels(QPainter& painter) {
    painter.setPen(Qt::black);
    QFont font = painter.font();
    font.setPointSize(9);
    painter.setFont(font);
    
    int drawWidth = width_ - MARGIN_LEFT - MARGIN_RIGHT;
    int drawHeight = height_ - MARGIN_TOP - MARGIN_BOTTOM;
    
    // Convert world coordinates to screen coordinates (using display bounds)
    auto worldToScreenX = [&](double worldX) {
        return MARGIN_LEFT + static_cast<int>((worldX - displayMinX_) / (displayMaxX_ - displayMinX_) * drawWidth);
    };
    
    auto worldToScreenY = [&](double worldY) {
        return height_ - MARGIN_BOTTOM - static_cast<int>((worldY - displayMinY_) / (displayMaxY_ - displayMinY_) * drawHeight);
    };
    
    // Determine axis position for label placement
    double xAxisY = 0.0;
    if (displayMinY_ > 0) xAxisY = displayMinY_;
    else if (displayMaxY_ < 0) xAxisY = displayMaxY_;
    
    double yAxisX = 0.0;
    if (displayMinX_ > 0) yAxisX = displayMinX_;
    else if (displayMaxX_ < 0) yAxisX = displayMaxX_;
    
    // X-axis labels
    for (const auto& tick : xTickInfo_.ticks) {
        int screenX = worldToScreenX(tick.value);
        int screenY = worldToScreenY(xAxisY) + 15;  // Below the axis
        
        QRect textRect(screenX - 40, screenY, 80, 20);
        painter.drawText(textRect, Qt::AlignCenter, QString::fromStdString(tick.label));
    }
    
    // Y-axis labels
    for (const auto& tick : yTickInfo_.ticks) {
        int screenX = worldToScreenX(yAxisX) - 45;  // Left of the axis
        int screenY = worldToScreenY(tick.value) - 8;
        
        QRect textRect(screenX, screenY, 40, 16);
        painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, QString::fromStdString(tick.label));
    }
}

void GLWidget::DrawSingleFunction(const LoadedRealFunction& func) {
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

void GLWidget::DrawMultiFunction(const MultiLoadedFunction& func) {
    const auto& xValues = func.GetXValues();
    const auto& yValues = func.GetYValues();
    
    if (xValues.empty()) return;
    
    for (int i = 0; i < func.GetDimension(); ++i) {
        if (!func.IsFunctionVisible(i)) continue;
        
        Color color = func.GetFunctionColor(i);
        glColor3f(color.r, color.g, color.b);
        glLineWidth(2.0f);
        
        glBegin(GL_LINE_STRIP);
        for (size_t j = 0; j < xValues.size() && j < yValues[i].size(); ++j) {
            glVertex2d(xValues[j], yValues[i][j]);
        }
        glEnd();
    }
}

void GLWidget::AddFunction(std::unique_ptr<LoadedFunction> func) {
    functions_.push_back(std::move(func));
    CalculateBounds();
    update();
    emit boundsChanged();
}

void GLWidget::ClearFunctions() {
    functions_.clear();
    
    // Reset to defaults
    defaultMinX_ = -10.0;
    defaultMaxX_ = 10.0;
    defaultMinY_ = -10.0;
    defaultMaxY_ = 10.0;
    
    viewMinX_ = defaultMinX_;
    viewMaxX_ = defaultMaxX_;
    viewMinY_ = defaultMinY_;
    viewMaxY_ = defaultMaxY_;
    
    // Calculate ticks for default range
    auto [xTicks, yTicks] = AxisTickCalculator::CalculateAxisTicks(
        viewMinX_, viewMaxX_, viewMinY_, viewMaxY_, 10, 8);
    xTickInfo_ = xTicks;
    yTickInfo_ = yTicks;
    
    update();
    emit boundsChanged();
}

void GLWidget::CalculateBounds() {
    if (functions_.empty()) {
        defaultMinX_ = -10.0;
        defaultMaxX_ = 10.0;
        defaultMinY_ = -10.0;
        defaultMaxY_ = 10.0;
    } else {
        // Find visible bounds
        bool firstVisible = true;
        double minX = 0, maxX = 1, minY = 0, maxY = 1;
        
        for (const auto& func : functions_) {
            if (!func->IsVisible()) continue;
            
            if (firstVisible) {
                minX = func->GetMinX();
                maxX = func->GetMaxX();
                minY = func->GetMinY();
                maxY = func->GetMaxY();
                firstVisible = false;
            } else {
                minX = std::min(minX, func->GetMinX());
                maxX = std::max(maxX, func->GetMaxX());
                minY = std::min(minY, func->GetMinY());
                maxY = std::max(maxY, func->GetMaxY());
            }
        }
        
        if (firstVisible) {
            // No visible functions
            minX = -10.0; maxX = 10.0;
            minY = -10.0; maxY = 10.0;
        }
        
        // Handle degenerate ranges
        if (std::abs(maxX - minX) < 1e-10) {
            double pad = std::abs(minX) * 0.1;
            if (pad < 1e-10) pad = 1.0;
            minX -= pad; maxX += pad;
        }
        if (std::abs(maxY - minY) < 1e-10) {
            double pad = std::abs(minY) * 0.1;
            if (pad < 1e-10) pad = 1.0;
            minY -= pad; maxY += pad;
        }
        
        defaultMinX_ = minX;
        defaultMaxX_ = maxX;
        defaultMinY_ = minY;
        defaultMaxY_ = maxY;
    }
    
    // Calculate nice tick values
    auto [xTicks, yTicks] = AxisTickCalculator::CalculateAxisTicks(
        defaultMinX_, defaultMaxX_, defaultMinY_, defaultMaxY_, 10, 8);
    
    xTickInfo_ = xTicks;
    yTickInfo_ = yTicks;
    
    // Use nice bounds for view
    viewMinX_ = xTicks.min;
    viewMaxX_ = xTicks.max;
    viewMinY_ = yTicks.min;
    viewMaxY_ = yTicks.max;
    
    // Projection will be set up in paintGL when update() triggers a repaint
}

void GLWidget::RecalculateBounds() {
    CalculateBounds();
    update();
}

void GLWidget::ResetView() {
    CalculateBounds();
    update();
}

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
    update();  // Projection will be set up in paintGL
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
    int drawWidth = width_ - MARGIN_LEFT - MARGIN_RIGHT;
    int drawHeight = height_ - MARGIN_TOP - MARGIN_BOTTOM;
    
    double rangeX = viewMaxX_ - viewMinX_;
    double rangeY = viewMaxY_ - viewMinY_;
    
    double dx = -delta.x() * rangeX / drawWidth;
    double dy = delta.y() * rangeY / drawHeight;
    
    viewMinX_ += dx;
    viewMaxX_ += dx;
    viewMinY_ += dy;
    viewMaxY_ += dy;
    
    // Recalculate ticks for new view
    auto [xTicks, yTicks] = AxisTickCalculator::CalculateAxisTicks(
        viewMinX_, viewMaxX_, viewMinY_, viewMaxY_, 10, 8);
    xTickInfo_ = xTicks;
    yTickInfo_ = yTicks;
    
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        isPanning_ = false;
    }
}

void GLWidget::wheelEvent(QWheelEvent* event) {
    double zoomFactor = event->angleDelta().y() > 0 ? 0.9 : 1.1;
    
    // Get mouse position in world coordinates
    int drawWidth = width_ - MARGIN_LEFT - MARGIN_RIGHT;
    int drawHeight = height_ - MARGIN_TOP - MARGIN_BOTTOM;
    
    QPointF pos = event->position();
    double mouseX = viewMinX_ + (pos.x() - MARGIN_LEFT) / drawWidth * (viewMaxX_ - viewMinX_);
    double mouseY = viewMaxY_ - (pos.y() - MARGIN_TOP) / drawHeight * (viewMaxY_ - viewMinY_);
    
    // Zoom around mouse position
    viewMinX_ = mouseX + (viewMinX_ - mouseX) * zoomFactor;
    viewMaxX_ = mouseX + (viewMaxX_ - mouseX) * zoomFactor;
    viewMinY_ = mouseY + (viewMinY_ - mouseY) * zoomFactor;
    viewMaxY_ = mouseY + (viewMaxY_ - mouseY) * zoomFactor;
    
    // Recalculate ticks for new view
    auto [xTicks, yTicks] = AxisTickCalculator::CalculateAxisTicks(
        viewMinX_, viewMaxX_, viewMinY_, viewMaxY_, 10, 8);
    xTickInfo_ = xTicks;
    yTickInfo_ = yTicks;
    
    update();
}
