// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "glwidget.h"
#include "helper.h"

#include <QPainter>
#include <QTimer>

//! [0]
GLWidget::GLWidget(Helper *helper, QWidget *parent)
    : QOpenGLWidget(parent), helper(helper), elapsed(0)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GLWidget::animate);
    timer->start(16); // roughly 60 FPS
    setFixedSize(200, 200);
    setAutoFillBackground(false);
}
//! [0]

void GLWidget::initializeGL()
{
    // Initialize OpenGL settings if needed
}

void GLWidget::resizeGL(int w, int h)
{
    // Handle resizing if needed
}

//! [1]
void GLWidget::animate()
{
    elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval()) % 1000;
    update();
}
//! [1]

//! [2]
void GLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw a simple graph
    painter.setPen(Qt::black);
    painter.drawLine(10, height() - 10, width() - 10, height() - 10); // X-axis
    painter.drawLine(10, height() - 10, 10, 10); // Y-axis

    // Example graph points
    QVector<QPointF> points = {
        QPointF(10, height() - 10),
        QPointF(50, height() - 50),
        QPointF(100, height() - 30),
        QPointF(150, height() - 70),
        QPointF(200, height() - 50)
    };

    painter.setPen(Qt::blue);
    for (int i = 0; i < points.size() - 1; ++i) {
        painter.drawLine(points[i], points[i + 1]);
    }
}
//! [2]
