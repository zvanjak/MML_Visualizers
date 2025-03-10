// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "widget.h"
#include "helper.h"

#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

//! [0]
Widget::Widget(Helper *helper, QWidget *parent)
    : QWidget(parent), helper(helper)
{
    elapsed = 0;
    setFixedSize(200, 200);
    setAutoFillBackground(false);
}
//! [0]

//! [1]
void Widget::animate()
{
    elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval()) % 1000;
    update();
}
//! [1]

//! [2]
void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //helper->paint(&painter, event, elapsed);

    painter.fillRect(event->rect(), QBrush(QColor(23, 32, 64)));

    // Draw a simple graph
    painter.setPen(Qt::black);
    painter.drawLine(0, height() - 20, width() - 10, height() - 10); // X-axis
    painter.drawLine(20, height() - 10, 10, 10); // Y-axis

    // Example graph points
    QVector<QPointF> points = {
        QPointF(10, height() - 10),
        QPointF(50, height() - 50),
        QPointF(100, height() - 30),
        QPointF(150, height() - 70),
        QPointF(200, height() - 50)
    };

    painter.setPen(Qt::green);
    for (int i = 0; i < points.size() - 1; ++i) {
        painter.drawLine(points[i], points[i + 1]);
    }

    painter.end();

}
//! [2]
