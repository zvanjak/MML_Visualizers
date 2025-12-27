#define _USE_MATH_DEFINES
#include <cmath>

#include "GLWidget.h"
#include <QDebug>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , currentStep_(0)
    , displayMode_(DisplayMode::None)
    , cameraDistance_(15.0f)
    , cameraRotationX_(25.0f)
    , cameraRotationY_(-45.0f)
    , initialCameraDistance_(15.0f)
    , initialCameraRotationX_(25.0f)
    , initialCameraRotationY_(-45.0f)
    , isRotating_(false)
    , isPanning_(false)
{
    lookAtPoint_ = QVector3D(0, 0, 0);
    initialLookAtPoint_ = QVector3D(0, 0, 0);
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
}

void GLWidget::SetSimulation(const LoadedParticleSimulation3D& sim)
{
    simulation_ = sim;
    currentStep_ = 0;
    
    // Set initial camera based on container size
    auto center = simulation_.GetCenter();
    lookAtPoint_ = QVector3D(center.x, center.y, center.z);
    initialLookAtPoint_ = lookAtPoint_;
    
    float maxDim = simulation_.GetMaxDimension();
    cameraDistance_ = maxDim * 2.0f;
    initialCameraDistance_ = cameraDistance_;
    
    update();
}

void GLWidget::SetCurrentStep(int step)
{
    if (step >= 0 && step < simulation_.numSteps) {
        currentStep_ = step;
        update();
    }
}

void GLWidget::SetDisplayMode(DisplayMode mode)
{
    displayMode_ = mode;
    update();
}

void GLWidget::SetParticleVisible(int index, bool visible)
{
    if (index >= 0 && index < static_cast<int>(simulation_.particles.size())) {
        simulation_.particles[index].visible = visible;
        update();
    }
}

void GLWidget::ResetCamera()
{
    lookAtPoint_ = initialLookAtPoint_;
    cameraDistance_ = initialCameraDistance_;
    cameraRotationX_ = initialCameraRotationX_;
    cameraRotationY_ = initialCameraRotationY_;
    update();
}

void GLWidget::LookAtCenter()
{
    auto center = simulation_.GetCenter();
    lookAtPoint_ = QVector3D(center.x, center.y, center.z);
    update();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Light setup
    GLfloat lightPos[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

void GLWidget::UpdateProjectionMatrix()
{
    float aspectRatio = static_cast<float>(width()) / static_cast<float>(height());
    
    projectionMatrix_.setToIdentity();
    // Far plane scales with camera distance to support unlimited zoom out
    float farPlane = qMax(10000.0f, cameraDistance_ * 10.0f);
    projectionMatrix_.perspective(45.0f, aspectRatio, 0.1f, farPlane);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    UpdateProjectionMatrix();
    
    // Calculate camera position
    float rotX = cameraRotationX_ * M_PI / 180.0f;
    float rotY = cameraRotationY_ * M_PI / 180.0f;
    
    float cosX = cos(rotX);
    float sinX = sin(rotX);
    float cosY = cos(rotY);
    float sinY = sin(rotY);
    
    cameraPosition_.setX(lookAtPoint_.x() + cameraDistance_ * cosX * sinY);
    cameraPosition_.setY(lookAtPoint_.y() + cameraDistance_ * sinX);
    cameraPosition_.setZ(lookAtPoint_.z() + cameraDistance_ * cosX * cosY);
    
    viewMatrix_.setToIdentity();
    viewMatrix_.lookAt(cameraPosition_, lookAtPoint_, QVector3D(0, 1, 0));
    
    // Apply matrices to OpenGL
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix_.constData());
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(viewMatrix_.constData());
    
    // Update light position relative to camera
    GLfloat lightPos[] = { cameraPosition_.x(), cameraPosition_.y() + 5.0f, cameraPosition_.z(), 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    // Draw based on display mode
    switch (displayMode_) {
        case DisplayMode::None:
            DrawAxes(false);
            break;
        case DisplayMode::BoundingBox:
            DrawBoundingBox();
            DrawAxes(false);
            break;
        case DisplayMode::CoordinatePlanes:
            DrawCoordinatePlanes();
            DrawAxes(true);
            break;
    }
    
    // Draw particles
    if (currentStep_ < simulation_.numSteps) {
        glEnable(GL_LIGHTING);
        for (size_t i = 0; i < simulation_.particles.size(); ++i) {
            const auto& particle = simulation_.particles[i];
            if (!particle.visible) continue;
            
            const auto& pos = particle.trajectory[currentStep_];
            DrawSphere(pos, particle.size, particle.color);
        }
    }
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::DrawSphere(const Point3D& center, double radius, const Color& color)
{
    glPushMatrix();
    glTranslatef(center.x, center.y, center.z);
    glColor4f(color.r, color.g, color.b, color.a);
    
    for (int i = 0; i < SPHERE_STACKS; ++i) {
        float lat0 = M_PI * (-0.5f + static_cast<float>(i) / SPHERE_STACKS);
        float z0 = sin(lat0) * radius;
        float zr0 = cos(lat0) * radius;
        
        float lat1 = M_PI * (-0.5f + static_cast<float>(i + 1) / SPHERE_STACKS);
        float z1 = sin(lat1) * radius;
        float zr1 = cos(lat1) * radius;
        
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= SPHERE_SLICES; ++j) {
            float lng = 2 * M_PI * static_cast<float>(j) / SPHERE_SLICES;
            float x = cos(lng);
            float y = sin(lng);
            
            glNormal3f(x * zr0 / radius, y * zr0 / radius, z0 / radius);
            glVertex3f(x * zr0, y * zr0, z0);
            
            glNormal3f(x * zr1 / radius, y * zr1 / radius, z1 / radius);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
    
    glPopMatrix();
}

void GLWidget::DrawAxes(bool extendBothDirections)
{
    float w = simulation_.containerWidth;
    float h = simulation_.containerHeight;
    float d = simulation_.containerDepth;
    
    float maxDim = std::max({w, h, d});
    float axisLength = maxDim * 1.2f;
    float sphereRadius = maxDim * 0.03f;
    
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    
    float start = extendBothDirections ? -axisLength : 0.0f;
    
    // X axis - Red
    glBegin(GL_LINES);
    glColor3f(0.8f, 0.0f, 0.0f);
    glVertex3f(start, 0, 0);
    glVertex3f(axisLength, 0, 0);
    glEnd();
    
    // Y axis - Green
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.6f, 0.0f);
    glVertex3f(0, start, 0);
    glVertex3f(0, axisLength, 0);
    glEnd();
    
    // Z axis - Blue
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.8f);
    glVertex3f(0, 0, start);
    glVertex3f(0, 0, axisLength);
    glEnd();
    
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
    
    // Draw sphere tips on axes
    DrawSphere({axisLength, 0, 0}, sphereRadius, {0.8f, 0.0f, 0.0f, 1.0f});
    DrawSphere({0, axisLength, 0}, sphereRadius, {0.0f, 0.6f, 0.0f, 1.0f});
    DrawSphere({0, 0, axisLength}, sphereRadius, {0.0f, 0.0f, 0.8f, 1.0f});
    
    if (extendBothDirections) {
        DrawSphere({start, 0, 0}, sphereRadius, {0.8f, 0.0f, 0.0f, 1.0f});
        DrawSphere({0, start, 0}, sphereRadius, {0.0f, 0.6f, 0.0f, 1.0f});
        DrawSphere({0, 0, start}, sphereRadius, {0.0f, 0.0f, 0.8f, 1.0f});
    }
}

void GLWidget::DrawSemiTransparentPlane(float x1, float y1, float z1,
                                         float x2, float y2, float z2,
                                         float x3, float y3, float z3,
                                         float x4, float y4, float z4,
                                         const Color& color)
{
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(color.r, color.g, color.b, color.a);
    
    glBegin(GL_QUADS);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
    glVertex3f(x4, y4, z4);
    glEnd();
    
    glEnable(GL_LIGHTING);
}

void GLWidget::DrawBoundingBox()
{
    float w = simulation_.containerWidth;
    float h = simulation_.containerHeight;
    float d = simulation_.containerDepth;
    
    // LightSkyBlue with opacity 0.2
    Color boxColor = {0.53f, 0.81f, 0.98f, 0.2f};
    
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw three half-planes at origin (z=0, y=0, x=0)
    // XY plane at z=0 (LightBlue)
    Color xyColor = {0.68f, 0.85f, 0.90f, 0.2f};
    DrawSemiTransparentPlane(0, 0, 0,  w, 0, 0,  w, h, 0,  0, h, 0, xyColor);
    
    // XZ plane at y=0 (LightGreen)
    Color xzColor = {0.56f, 0.93f, 0.56f, 0.2f};
    DrawSemiTransparentPlane(0, 0, 0,  w, 0, 0,  w, 0, d,  0, 0, d, xzColor);
    
    // YZ plane at x=0 (LightCoral)
    Color yzColor = {0.94f, 0.50f, 0.50f, 0.2f};
    DrawSemiTransparentPlane(0, 0, 0,  0, h, 0,  0, h, d,  0, 0, d, yzColor);
    
    // Draw wireframe edges
    glColor4f(0.5f, 0.5f, 0.5f, 0.6f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0, 0, 0);
    glVertex3f(w, 0, 0);
    glVertex3f(w, h, 0);
    glVertex3f(0, h, 0);
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    glVertex3f(0, 0, d);
    glVertex3f(w, 0, d);
    glVertex3f(w, h, d);
    glVertex3f(0, h, d);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0); glVertex3f(0, 0, d);
    glVertex3f(w, 0, 0); glVertex3f(w, 0, d);
    glVertex3f(w, h, 0); glVertex3f(w, h, d);
    glVertex3f(0, h, 0); glVertex3f(0, h, d);
    glEnd();
    
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void GLWidget::DrawCoordinatePlanes()
{
    float w = simulation_.containerWidth;
    float h = simulation_.containerHeight;
    float d = simulation_.containerDepth;
    
    float maxDim = std::max({w, h, d}) * 1.2f;
    
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // XY plane (LightBlue with opacity 0.12)
    Color xyColor = {0.68f, 0.85f, 0.90f, 0.12f};
    DrawSemiTransparentPlane(-maxDim, -maxDim, 0,  maxDim, -maxDim, 0,  
                              maxDim, maxDim, 0,  -maxDim, maxDim, 0, xyColor);
    
    // XZ plane (LightGreen with opacity 0.12)
    Color xzColor = {0.56f, 0.93f, 0.56f, 0.12f};
    DrawSemiTransparentPlane(-maxDim, 0, -maxDim,  maxDim, 0, -maxDim,  
                              maxDim, 0, maxDim,  -maxDim, 0, maxDim, xzColor);
    
    // YZ plane (LightCoral with opacity 0.12)
    Color yzColor = {0.94f, 0.50f, 0.50f, 0.12f};
    DrawSemiTransparentPlane(0, -maxDim, -maxDim,  0, maxDim, -maxDim,  
                              0, maxDim, maxDim,  0, -maxDim, maxDim, yzColor);
    
    glEnable(GL_LIGHTING);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePos_ = event->pos();
    
    if (event->button() == Qt::LeftButton) {
        isRotating_ = true;
    } else if (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton) {
        isPanning_ = true;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint delta = event->pos() - lastMousePos_;
    
    if (isRotating_) {
        cameraRotationY_ += delta.x() * 0.5f;
        cameraRotationX_ += delta.y() * 0.5f;
        
        // Clamp rotation
        cameraRotationX_ = qBound(-89.0f, cameraRotationX_, 89.0f);
        
        update();
    } else if (isPanning_) {
        // Pan the look-at point
        float panSpeed = cameraDistance_ * 0.002f;
        
        // Calculate right and up vectors
        float rotY = cameraRotationY_ * M_PI / 180.0f;
        QVector3D right(cos(rotY), 0, -sin(rotY));
        QVector3D up(0, 1, 0);
        
        lookAtPoint_ -= right * delta.x() * panSpeed;
        lookAtPoint_ += up * delta.y() * panSpeed;
        
        update();
    }
    
    lastMousePos_ = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    isRotating_ = false;
    isPanning_ = false;
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y() / 120.0f;
    cameraDistance_ *= (1.0f - delta * 0.1f);
    cameraDistance_ = qMax(0.1f, cameraDistance_);  // Only enforce minimum, no upper limit
    
    update();
}
