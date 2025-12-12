#include "GLWidget.h"
#include <QOpenGLFunctions>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , currentStep_(0)
    , showBoundingBox_(true)
    , cameraDistance_(2000.0f)
    , cameraRotationX_(30.0f)
    , cameraRotationY_(45.0f)
    , isRotating_(false)
    , isPanning_(false)
{
    lookAtPoint_ = QVector3D(500, 500, 500);
}

GLWidget::~GLWidget()
{
}

void GLWidget::SetSimulation(const LoadedParticleSimulation3D& sim)
{
    simulation_ = sim;
    currentStep_ = 0;
    
    // Center camera on simulation bounds
    lookAtPoint_ = QVector3D(
        static_cast<float>(sim.width / 2),
        static_cast<float>(sim.height / 2),
        static_cast<float>(sim.depth / 2)
    );
    
    // Set appropriate camera distance based on simulation size
    double maxDim = std::max({sim.width, sim.height, sim.depth});
    cameraDistance_ = static_cast<float>(maxDim * 2.0);
    
    update();
}

void GLWidget::SetCurrentStep(int step)
{
    if (step >= 0 && step < simulation_.numSteps) {
        currentStep_ = step;
        update();
    }
}

void GLWidget::SetShowBoundingBox(bool show)
{
    showBoundingBox_ = show;
    update();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Setup lighting
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
}

void GLWidget::UpdateProjectionMatrix()
{
    projectionMatrix_.setToIdentity();
    float aspect = static_cast<float>(width()) / static_cast<float>(height());
    projectionMatrix_.perspective(45.0f, aspect, 1.0f, 10000.0f);
    
    // Update view matrix
    viewMatrix_.setToIdentity();
    
    // Calculate camera position based on rotation and distance
    float camX = cameraDistance_ * std::cos(cameraRotationX_ * M_PI / 180.0f) * std::cos(cameraRotationY_ * M_PI / 180.0f);
    float camY = cameraDistance_ * std::sin(cameraRotationX_ * M_PI / 180.0f);
    float camZ = cameraDistance_ * std::cos(cameraRotationX_ * M_PI / 180.0f) * std::sin(cameraRotationY_ * M_PI / 180.0f);
    
    cameraPosition_ = lookAtPoint_ + QVector3D(camX, camY, camZ);
    
    viewMatrix_.lookAt(cameraPosition_, lookAtPoint_, QVector3D(0, 1, 0));
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    UpdateProjectionMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix_.data());
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(viewMatrix_.data());
    
    // Draw bounding box or axes
    if (showBoundingBox_) {
        DrawBoundingBox();
    } else {
        DrawAxes();
    }
    
    // Draw particles at current timestep
    for (const auto& particle : simulation_.particles) {
        if (currentStep_ < particle.GetNumSteps()) {
            Point3D pos = particle.GetPosition(currentStep_);
            DrawSphere(pos, particle.radius, particle.color);
        }
    }
}

void GLWidget::DrawSphere(const Point3D& center, double radius, const Color& color)
{
    glPushMatrix();
    glTranslated(center.x, center.y, center.z);
    glColor4f(color.r, color.g, color.b, color.a);
    
    // Draw sphere using triangulation
    for (int i = 0; i < SPHERE_STACKS; ++i) {
        float lat0 = M_PI * (-0.5f + (float)i / SPHERE_STACKS);
        float z0 = std::sin(lat0);
        float zr0 = std::cos(lat0);
        
        float lat1 = M_PI * (-0.5f + (float)(i + 1) / SPHERE_STACKS);
        float z1 = std::sin(lat1);
        float zr1 = std::cos(lat1);
        
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= SPHERE_SLICES; ++j) {
            float lng = 2 * M_PI * (float)j / SPHERE_SLICES;
            float x = std::cos(lng);
            float y = std::sin(lng);
            
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(radius * x * zr0, radius * y * zr0, radius * z0);
            
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(radius * x * zr1, radius * y * zr1, radius * z1);
        }
        glEnd();
    }
    
    glPopMatrix();
}

void GLWidget::DrawBoundingBox()
{
    glDisable(GL_LIGHTING);
    glColor4f(0.5f, 0.5f, 0.7f, 0.3f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float w = static_cast<float>(simulation_.width);
    float h = static_cast<float>(simulation_.height);
    float d = static_cast<float>(simulation_.depth);
    
    // Draw semi-transparent faces
    glBegin(GL_QUADS);
    
    // Bottom face (z=0)
    glVertex3f(0, 0, 0);
    glVertex3f(w, 0, 0);
    glVertex3f(w, h, 0);
    glVertex3f(0, h, 0);
    
    // Top face (z=d)
    glVertex3f(0, 0, d);
    glVertex3f(0, h, d);
    glVertex3f(w, h, d);
    glVertex3f(w, 0, d);
    
    // Left face (x=0)
    glVertex3f(0, 0, 0);
    glVertex3f(0, h, 0);
    glVertex3f(0, h, d);
    glVertex3f(0, 0, d);
    
    // Right face (x=w)
    glVertex3f(w, 0, 0);
    glVertex3f(w, 0, d);
    glVertex3f(w, h, d);
    glVertex3f(w, h, 0);
    
    // Front face (y=0)
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, d);
    glVertex3f(w, 0, d);
    glVertex3f(w, 0, 0);
    
    // Back face (y=h)
    glVertex3f(0, h, 0);
    glVertex3f(w, h, 0);
    glVertex3f(w, h, d);
    glVertex3f(0, h, d);
    
    glEnd();
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void GLWidget::DrawAxes()
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    
    float axisLen = static_cast<float>(std::max({simulation_.width, simulation_.height, simulation_.depth}) * 0.5);
    
    glBegin(GL_LINES);
    
    // X axis (red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(axisLen, 0, 0);
    
    // Y axis (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(0, axisLen, 0);
    
    // Z axis (blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, axisLen);
    
    glEnd();
    
    glEnable(GL_LIGHTING);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    UpdateProjectionMatrix();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePos_ = event->pos();
    
    if (event->button() == Qt::LeftButton) {
        isRotating_ = true;
    }
    else if (event->button() == Qt::RightButton) {
        isPanning_ = true;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint delta = event->pos() - lastMousePos_;
    
    if (isRotating_) {
        cameraRotationY_ += delta.x() * 0.5f;
        cameraRotationX_ += delta.y() * 0.5f;
        
        // Clamp X rotation
        if (cameraRotationX_ > 89.0f) cameraRotationX_ = 89.0f;
        if (cameraRotationX_ < -89.0f) cameraRotationX_ = -89.0f;
        
        update();
    }
    else if (isPanning_) {
        float panSpeed = cameraDistance_ * 0.001f;
        
        QVector3D right = QVector3D::crossProduct(
            lookAtPoint_ - cameraPosition_, 
            QVector3D(0, 1, 0)
        ).normalized();
        QVector3D up(0, 1, 0);
        
        lookAtPoint_ += right * delta.x() * panSpeed;
        lookAtPoint_ -= up * delta.y() * panSpeed;
        
        update();
    }
    
    lastMousePos_ = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    float zoomFactor = 1.0f + event->angleDelta().y() * 0.001f;
    cameraDistance_ /= zoomFactor;
    
    // Clamp distance
    if (cameraDistance_ < 100.0f) cameraDistance_ = 100.0f;
    if (cameraDistance_ > 10000.0f) cameraDistance_ = 10000.0f;
    
    update();
}
