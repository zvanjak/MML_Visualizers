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

    void setScalarFunction(const ScalarFunction2DData& data);
    void clearData();
    void resetCamera();
    
    // Display settings
    void setScaleX(double scale);
    void setScaleY(double scale);
    void setShowGridPoints(bool show);
    void setColorByHeight(bool enable);
    void setSurfaceColorIndex(int index);
    void setPointsColorIndex(int index);
    
    // Accessors
    const ScalarFunction2DData& getData() const { return m_data; }
    bool hasData() const { return m_hasData; }

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
    void drawSurface();
    void drawGridPoints();
    QVector3D getColorForHeight(double value);
    void drawQuad(const QVector3D& v0, const QVector3D& v1, const QVector3D& v2, const QVector3D& v3);

    ScalarFunction2DData m_data;
    double m_scaleX;
    double m_scaleY;
    bool m_showGridPoints;
    bool m_colorByHeight;
    bool m_hasData;
    int m_surfaceColorIndex;
    int m_pointsColorIndex;

    // Camera parameters
    float m_distance;
    float m_angleX;
    float m_angleY;
    QPoint m_lastMousePos;
    
    // Initial camera state for reset
    float m_initialDistance;
    float m_initialAngleX;
    float m_initialAngleY;
};
