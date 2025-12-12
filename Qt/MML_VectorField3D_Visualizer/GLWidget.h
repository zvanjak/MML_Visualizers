#pragma once

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMouseEvent>
#include <QWheelEvent>
#include "MMLData.h"

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget* parent = nullptr);
    ~GLWidget();

    void setVectorField(const LoadedVectorField3D& field);
    void setVectorScale(double scale);
    void setColorByMagnitude(bool enable);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void setupCamera();
    void drawCoordinateSystem();
    void drawVectorField();
    void drawArrow(const Vector3D& start, const Vector3D& direction, double magnitude);
    void drawCylinder(double baseRadius, double topRadius, double height, int slices);
    void drawCone(double baseRadius, double height, int slices);
    void rotateToDirection(const Vector3D& direction);
    QVector3D getColorForMagnitude(double magnitude);

    LoadedVectorField3D m_vectorField;
    double m_vectorScale;
    bool m_colorByMagnitude;
    bool m_hasData;

    // Camera parameters
    float m_distance;
    float m_angleX;
    float m_angleY;
    QPoint m_lastMousePos;
};
