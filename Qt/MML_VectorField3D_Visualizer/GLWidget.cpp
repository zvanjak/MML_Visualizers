#define NOMINMAX
#define _USE_MATH_DEFINES
#include "GLWidget.h"
#include <GL/gl.h>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_vectorScale(5.0)
    , m_colorByMagnitude(true)
    , m_hasData(false)
    , m_distance(500.0f)
    , m_angleX(30.0f)
    , m_angleY(30.0f)
{
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
}

void GLWidget::setVectorField(const LoadedVectorField3D& field)
{
    m_vectorField = field;
    m_hasData = !field.vectors.empty();
    
    // Auto-adjust camera distance based on bounds
    if (m_hasData) {
        Vector3D center = (field.maxBounds + field.minBounds) * 0.5;
        Vector3D size = field.maxBounds - field.minBounds;
        double maxDim = std::max({size.x, size.y, size.z});
        m_distance = maxDim * 1.5f;
    }
    
    update();
}

void GLWidget::setVectorScale(double scale)
{
    m_vectorScale = scale;
    update();
}

void GLWidget::setColorByMagnitude(bool enable)
{
    m_colorByMagnitude = enable;
    update();
}

void GLWidget::initializeGL()
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Setup lighting
    GLfloat lightPos[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    setupCamera();
    drawCoordinateSystem();
    
    if (m_hasData) {
        drawVectorField();
    }
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    double aspect = double(w) / double(h);
    double fov = 45.0;
    double zNear = 1.0;
    double zFar = 10000.0;
    
    double fH = tan(fov / 360.0 * M_PI) * zNear;
    double fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::setupCamera()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glTranslatef(0, 0, -m_distance);
    glRotatef(m_angleX, 1, 0, 0);
    glRotatef(m_angleY, 0, 1, 0);
}

void GLWidget::drawCoordinateSystem()
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    
    double axisLength = m_hasData ? 
        std::max({m_vectorField.maxBounds.x - m_vectorField.minBounds.x,
                  m_vectorField.maxBounds.y - m_vectorField.minBounds.y,
                  m_vectorField.maxBounds.z - m_vectorField.minBounds.z}) * 0.6 : 100.0;
    
    glBegin(GL_LINES);
    
    // X axis (red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3d(0, 0, 0);
    glVertex3d(axisLength, 0, 0);
    
    // Y axis (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3d(0, 0, 0);
    glVertex3d(0, axisLength, 0);
    
    // Z axis (blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, axisLength);
    
    glEnd();
    
    glEnable(GL_LIGHTING);
}

void GLWidget::drawVectorField()
{
    if (!m_hasData) return;
    
    for (const auto& vf : m_vectorField.vectors) {
        double magnitude = vf.vector.norm();
        if (magnitude < 1e-10) continue;  // Skip zero vectors
        
        drawArrow(vf.position, vf.vector, magnitude);
    }
}

void GLWidget::drawArrow(const Vector3D& start, const Vector3D& direction, double magnitude)
{
    glPushMatrix();
    
    // Translate to start position
    glTranslated(start.x, start.y, start.z);
    
    // Rotate to align with direction
    rotateToDirection(direction);
    
    // Scale by magnitude and user scale
    double scaledLength = magnitude * m_vectorScale;
    double baseRadius = magnitude * 0.3;
    
    // Set color
    if (m_colorByMagnitude) {
        QVector3D color = getColorForMagnitude(magnitude);
        glColor3f(color.x(), color.y(), color.z());
    } else {
        glColor3f(0.2f, 0.6f, 1.0f);  // Default blue
    }
    
    // Draw cylinder shaft (70% of length)
    double shaftLength = scaledLength * 0.7;
    drawCylinder(baseRadius * 0.3, baseRadius * 0.3, shaftLength, 12);
    
    // Move to cone position
    glTranslated(0, 0, shaftLength);
    
    // Draw cone tip (30% of length)
    double coneHeight = scaledLength * 0.3;
    drawCone(baseRadius, coneHeight, 12);
    
    glPopMatrix();
}

void GLWidget::drawCylinder(double baseRadius, double topRadius, double height, int slices)
{
    glBegin(GL_QUAD_STRIP);
    
    for (int i = 0; i <= slices; i++) {
        double angle = 2.0 * M_PI * i / slices;
        double x = cos(angle);
        double y = sin(angle);
        
        glNormal3d(x, y, 0);
        glVertex3d(baseRadius * x, baseRadius * y, 0);
        glVertex3d(topRadius * x, topRadius * y, height);
    }
    
    glEnd();
    
    // Bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3d(0, 0, -1);
    glVertex3d(0, 0, 0);
    for (int i = 0; i <= slices; i++) {
        double angle = 2.0 * M_PI * i / slices;
        glVertex3d(baseRadius * cos(angle), baseRadius * sin(angle), 0);
    }
    glEnd();
    
    // Top cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3d(0, 0, 1);
    glVertex3d(0, 0, height);
    for (int i = slices; i >= 0; i--) {
        double angle = 2.0 * M_PI * i / slices;
        glVertex3d(topRadius * cos(angle), topRadius * sin(angle), height);
    }
    glEnd();
}

void GLWidget::drawCone(double baseRadius, double height, int slices)
{
    glBegin(GL_TRIANGLE_FAN);
    glNormal3d(0, 0, 1);
    glVertex3d(0, 0, height);
    
    for (int i = 0; i <= slices; i++) {
        double angle = 2.0 * M_PI * i / slices;
        double x = cos(angle);
        double y = sin(angle);
        
        // Normal for cone surface
        double len = sqrt(baseRadius * baseRadius + height * height);
        glNormal3d(x * height / len, y * height / len, baseRadius / len);
        glVertex3d(baseRadius * x, baseRadius * y, 0);
    }
    glEnd();
    
    // Base
    glBegin(GL_TRIANGLE_FAN);
    glNormal3d(0, 0, -1);
    glVertex3d(0, 0, 0);
    for (int i = 0; i <= slices; i++) {
        double angle = 2.0 * M_PI * i / slices;
        glVertex3d(baseRadius * cos(angle), baseRadius * sin(angle), 0);
    }
    glEnd();
}

void GLWidget::rotateToDirection(const Vector3D& direction)
{
    // Normalize direction
    Vector3D dir = direction.normalized();
    
    // Default direction is along +Z axis
    Vector3D defaultDir(0, 0, 1);
    
    // Calculate rotation axis (cross product)
    Vector3D axis = defaultDir.cross(dir);
    double axisLength = axis.norm();
    
    if (axisLength > 1e-6) {
        // Calculate rotation angle
        double angle = acos(std::max(-1.0, std::min(1.0, defaultDir.dot(dir))));
        angle = angle * 180.0 / M_PI;
        
        // Normalize axis
        axis = axis * (1.0 / axisLength);
        
        // Apply rotation
        glRotated(angle, axis.x, axis.y, axis.z);
    } else if (defaultDir.dot(dir) < 0) {
        // Vectors are opposite, rotate 180 degrees around any perpendicular axis
        glRotated(180.0, 1, 0, 0);
    }
}

QVector3D GLWidget::getColorForMagnitude(double magnitude)
{
    if (!m_hasData) return QVector3D(0.2f, 0.6f, 1.0f);
    
    // Normalize magnitude to [0, 1]
    double range = m_vectorField.maxMagnitude - m_vectorField.minMagnitude;
    double t = (range > 1e-10) ? (magnitude - m_vectorField.minMagnitude) / range : 0.0;
    t = std::max(0.0, std::min(1.0, t));
    
    // Color gradient: Blue (low) -> Cyan -> Green -> Yellow -> Red (high)
    if (t < 0.25) {
        // Blue to Cyan
        double s = t / 0.25;
        return QVector3D(0.0f, s, 1.0f);
    } else if (t < 0.5) {
        // Cyan to Green
        double s = (t - 0.25) / 0.25;
        return QVector3D(0.0f, 1.0f, 1.0f - s);
    } else if (t < 0.75) {
        // Green to Yellow
        double s = (t - 0.5) / 0.25;
        return QVector3D(s, 1.0f, 0.0f);
    } else {
        // Yellow to Red
        double s = (t - 0.75) / 0.25;
        return QVector3D(1.0f, 1.0f - s, 0.0f);
    }
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
    m_lastMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
    int dx = event->pos().x() - m_lastMousePos.x();
    int dy = event->pos().y() - m_lastMousePos.y();
    
    if (event->buttons() & Qt::LeftButton) {
        m_angleY += dx * 0.5f;
        m_angleX += dy * 0.5f;
        
        // Clamp X rotation
        if (m_angleX > 89.0f) m_angleX = 89.0f;
        if (m_angleX < -89.0f) m_angleX = -89.0f;
        
        update();
    }
    
    m_lastMousePos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
    int delta = event->angleDelta().y();
    
    if (delta > 0) {
        m_distance *= 0.9f;
    } else {
        m_distance *= 1.1f;
    }
    
    if (m_distance < 10.0f) m_distance = 10.0f;
    if (m_distance > 5000.0f) m_distance = 5000.0f;
    
    update();
}
