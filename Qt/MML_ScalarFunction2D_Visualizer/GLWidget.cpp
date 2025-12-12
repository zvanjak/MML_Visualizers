#define NOMINMAX
#define _USE_MATH_DEFINES
#include "GLWidget.h"
#include <GL/gl.h>
#include <cmath>
#include <iostream>
#include <algorithm>

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_scaleX(10.0)
    , m_scaleY(10.0)
    , m_showGridPoints(true)
    , m_colorByHeight(true)
    , m_hasData(false)
    , m_distance(300.0f)
    , m_angleX(45.0f)
    , m_angleY(30.0f)
{
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
}

void GLWidget::setScalarFunction(const ScalarFunction2DData& data)
{
    m_data = data;
    m_hasData = !data.values.empty();
    
    // Auto-adjust camera based on data
    if (m_hasData) {
        double rangeX = std::abs(data.xMax - data.xMin) * m_scaleX;
        double rangeY = std::abs(data.yMax - data.yMin) * m_scaleY;
        double rangeZ = std::abs(data.maxValue - data.minValue);
        double maxRange = std::max({rangeX, rangeY, rangeZ});
        m_distance = maxRange * 1.5f;
    }
    
    update();
}

void GLWidget::setScaleX(double scale)
{
    m_scaleX = scale;
    update();
}

void GLWidget::setScaleY(double scale)
{
    m_scaleY = scale;
    update();
}

void GLWidget::setShowGridPoints(bool show)
{
    m_showGridPoints = show;
    update();
}

void GLWidget::setColorByHeight(bool enable)
{
    m_colorByHeight = enable;
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
        drawSurface();
        
        if (m_showGridPoints) {
            drawGridPoints();
        }
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
    if (!m_hasData) return;
    
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    
    double axisLength = std::max({
        std::abs(m_data.xMax - m_data.xMin) * m_scaleX,
        std::abs(m_data.yMax - m_data.yMin) * m_scaleY,
        std::abs(m_data.maxValue - m_data.minValue)
    }) * 0.6;
    
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

void GLWidget::drawSurface()
{
    if (!m_hasData) return;
    
    // Draw surface as quads with interpolated colors
    for (int i = 0; i < m_data.numPointsX - 1; i++) {
        for (int j = 0; j < m_data.numPointsY - 1; j++) {
            // Get grid positions
            double x0 = m_data.getX(i) * m_scaleX;
            double x1 = m_data.getX(i + 1) * m_scaleX;
            double y0 = m_data.getY(j) * m_scaleY;
            double y1 = m_data.getY(j + 1) * m_scaleY;
            
            // Get function values
            double z00 = m_data.getValue(i, j);
            double z10 = m_data.getValue(i + 1, j);
            double z11 = m_data.getValue(i + 1, j + 1);
            double z01 = m_data.getValue(i, j + 1);
            
            // Create vertices
            QVector3D v0(x0, y0, z00);
            QVector3D v1(x1, y0, z10);
            QVector3D v2(x1, y1, z11);
            QVector3D v3(x0, y1, z01);
            
            drawQuad(v0, v1, v2, v3);
        }
    }
}

void GLWidget::drawQuad(const QVector3D& v0, const QVector3D& v1, const QVector3D& v2, const QVector3D& v3)
{
    // Calculate normal using cross product
    QVector3D edge1 = v1 - v0;
    QVector3D edge2 = v3 - v0;
    QVector3D normal = QVector3D::crossProduct(edge1, edge2).normalized();
    
    glBegin(GL_QUADS);
    
    glNormal3f(normal.x(), normal.y(), normal.z());
    
    // Set color based on height if enabled
    if (m_colorByHeight) {
        QVector3D color0 = getColorForHeight(v0.z());
        glColor3f(color0.x(), color0.y(), color0.z());
    } else {
        glColor3f(0.8f, 0.2f, 0.2f);  // Red surface
    }
    glVertex3f(v0.x(), v0.y(), v0.z());
    
    if (m_colorByHeight) {
        QVector3D color1 = getColorForHeight(v1.z());
        glColor3f(color1.x(), color1.y(), color1.z());
    }
    glVertex3f(v1.x(), v1.y(), v1.z());
    
    if (m_colorByHeight) {
        QVector3D color2 = getColorForHeight(v2.z());
        glColor3f(color2.x(), color2.y(), color2.z());
    }
    glVertex3f(v2.x(), v2.y(), v2.z());
    
    if (m_colorByHeight) {
        QVector3D color3 = getColorForHeight(v3.z());
        glColor3f(color3.x(), color3.y(), color3.z());
    }
    glVertex3f(v3.x(), v3.y(), v3.z());
    
    glEnd();
}

void GLWidget::drawGridPoints()
{
    if (!m_hasData) return;
    
    glDisable(GL_LIGHTING);
    glColor3f(0.2f, 0.4f, 1.0f);  // Blue points
    glPointSize(4.0f);
    
    glBegin(GL_POINTS);
    
    for (int i = 0; i < m_data.numPointsX; i++) {
        for (int j = 0; j < m_data.numPointsY; j++) {
            double x = m_data.getX(i) * m_scaleX;
            double y = m_data.getY(j) * m_scaleY;
            double z = m_data.getValue(i, j);
            
            glVertex3d(x, y, z);
        }
    }
    
    glEnd();
    
    glEnable(GL_LIGHTING);
}

QVector3D GLWidget::getColorForHeight(double value)
{
    if (!m_hasData) return QVector3D(0.8f, 0.2f, 0.2f);
    
    // Normalize value to [0, 1]
    double range = m_data.maxValue - m_data.minValue;
    double t = (range > 1e-10) ? (value - m_data.minValue) / range : 0.0;
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
