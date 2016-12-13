/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "qscreendistribution.h"

#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>
#include <QSize>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QWidgetAction>
#include <QLinearGradient>
#include <QRadialGradient>

namespace udg {

const int QScreenDistribution::WidgetMargin = 20;

QScreenDistribution::QScreenDistribution(QWidget *parent)
 : QWidget(parent)
{
    setMouseTracking(true);
    m_mouseInScreen = -1;
    this->setMinimumSize(QSize(300, 200));
}

QScreenDistribution::~QScreenDistribution()
{
}

QSize QScreenDistribution::sizeHint() const
{
    return QSize(400, 200);
}

void QScreenDistribution::mouseMoveEvent(QMouseEvent *event)
{
    bool outside = true;
    int i = 0;
    // Per cada icona de finestra, comprobar si el ratolí hi és dins
    while (outside && i < m_screens.size())
    {
        if (m_screens[i].contains(event->pos()))
        {
            outside = false;
            setCursor(Qt::PointingHandCursor);
            if (m_mouseInScreen != i)
            {
                m_mouseInScreen = i;
                repaint();
            }
        }
        i++;
    }
    // Si no se n'hi ha trobat cap, llavors està a fora
    if (outside)
    {
        unsetCursor();
        if (m_mouseInScreen != -1)
        {
            m_mouseInScreen = -1;
            repaint();
        }
    }
}

void QScreenDistribution::mousePressEvent(QMouseEvent *event)
{
    bool outside = true;
    int i = 0;
    // Per cada icona de finestra, comprobar si s'ha fet un clic dins
    while (outside && i < m_screens.size())
    {
        if (m_screens[i].contains(event->pos()))
        {
            outside = false;
            emit screenClicked(i);
            event->ignore();
        }
        i++;
    }
}

void QScreenDistribution::paintEvent(QPaintEvent *event)
{
    m_screens.clear();

    // Fills the list of rects (screens) to be painted.
    computeSizesAndPositions();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);


    int borderSize = 8;
    int outerRadius = 5;
    int circleRadius = m_screenNumberPixelSize * 1.25;

    QBrush borderBrush = QBrush(QColor("#18222a"));

    QLinearGradient screenGradient(0, 0, 0, 1);
    screenGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    screenGradient.setColorAt(0, QColor("#545e67"));
    screenGradient.setColorAt(1, QColor("#333639"));
    QBrush screenBrush = QBrush(screenGradient);

    QLinearGradient circleGradient(0, 0, 0, 1);
    circleGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    circleGradient.setColorAt(0, QColor("#b7bcc0"));
    circleGradient.setColorAt(1, QColor("#9c9d9f"));
    QBrush circleBrush = QBrush(circleGradient);

    QLinearGradient circleActiveGradient(0, 0, 0, 1);
    circleActiveGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    circleActiveGradient.setColorAt(0, QColor("#2093e6"));
    circleActiveGradient.setColorAt(1, QColor("#1e6ea5"));
    QBrush circleActiveBrush = QBrush(circleActiveGradient);

    QPen whitePen = QPen(QColor("#f3f3f3"), 2, Qt::SolidLine);
    QPen blackPen = QPen(QColor("#2a2c2f"), 2, Qt::SolidLine);
    painter.setPen(blackPen);
    QFont numberFont;
    numberFont.setPixelSize(m_screenNumberPixelSize);

    for (int i = 0; i < m_screens.count(); i++)
    {
        // Paint the border
        painter.setPen(Qt::NoPen);
        painter.setBrush(borderBrush);
        painter.drawRoundedRect(m_screens.at(i), outerRadius, outerRadius);

        // Paint the screen
        painter.setPen(Qt::NoPen);
        painter.setBrush(screenBrush);
        QRect screenRect = m_screens.at(i);
        screenRect = QRect(screenRect.left() + borderSize,
                           screenRect.top() + borderSize,
                           screenRect.width() - borderSize*2,
                           screenRect.height() - borderSize*2);
        painter.drawRect(screenRect);

        // Paint the circle
        painter.setPen(Qt::NoPen);
        painter.setBrush(circleBrush);
        if (m_mouseInScreen ==  i) // Screen hovered by mouse
        {
            painter.setPen(whitePen);
            painter.setBrush(circleActiveBrush);
        }
        QRect circleRect = m_screens.at(i);
        circleRect = QRect(circleRect.left() + circleRect.width() / 2 - circleRadius / 2,
                           circleRect.top() + circleRect.height() / 2 - circleRadius / 2,
                           circleRadius,
                           circleRadius);
        painter.drawEllipse(circleRect);

        // Paint the number
        painter.setPen(blackPen);
        painter.setBrush(Qt::NoBrush);
        if (m_mouseInScreen ==  i) // Screen hovered by mouse
        {
            painter.setPen(whitePen);
        }
        painter.setFont(numberFont);
        painter.drawText(m_screens.at(i), Qt::AlignCenter, QString::number(i + 1));
    }
    event->accept();
}

void QScreenDistribution::computeSizesAndPositions()
{
    QDesktopWidget *desktop = QApplication::desktop();

    int minimumX = 0;
    int minimumY = 0;
    int maximumX = 0;
    int maximumY = 0;

    // Per cada pantalla
    for (int i = 0; i < desktop->screenCount(); i++)
    {
        // Agafem les seves coordenades
        QPoint topLeft = desktop->screenGeometry(i).topLeft();
        QPoint bottomRight = desktop->screenGeometry(i).bottomRight();

        m_screens.append(QRect(topLeft, bottomRight));

        // I calculem el tamany màxim que ocupa tot el conjunt de pantalles
        if (topLeft.x() < minimumX)
        {
            minimumX = topLeft.x();
        }
        if (bottomRight.x() > maximumX)
        {
            maximumX = bottomRight.x();
        }
        if (topLeft.y() < minimumY)
        {
            minimumY = topLeft.y();
        }
        if (bottomRight.y() > maximumY)
        {
            maximumY = bottomRight.y();
        }
    }

    int totalWidth = maximumX - minimumX;
    int totalHeight = maximumY - minimumY;

    // Calculem la proporció en que s'ha de dividir per que no es deformi el dibuix
    float widthDivisor = (float)totalWidth / ((float)width() - WidgetMargin * 2);
    float heightDivisor = (float)totalHeight / ((float)height() - WidgetMargin * 2);

    float divisor = (widthDivisor < heightDivisor) ? heightDivisor : widthDivisor;
    int offsetX = 0;
    int offsetY = 0;

    if (widthDivisor > heightDivisor)
    {
        float centering = (height() - WidgetMargin * 2 - (totalHeight / divisor)) / 2;
        offsetX = -1 * minimumX / divisor + WidgetMargin;
        offsetY = -1 * minimumY / divisor + WidgetMargin + centering;
    }
    else
    {
        float centering = (width() - WidgetMargin * 2 - (totalWidth / divisor)) / 2;
        offsetX = -1 * minimumX / divisor + WidgetMargin + centering;
        offsetY = -1 * minimumY / divisor + WidgetMargin;
    }

    // Adaptem les posicións a les posicions de dibuix escalades i centrades
    int minimumScreenHeight = 0;
    int minimumScreenWidth = 0;
    for (int i = 0; i < m_screens.count(); i++)
    {
        // Requadre
        QRect screen;
        screen.setLeft(m_screens.at(i).left() / divisor + offsetX + 3);
        screen.setTop(m_screens.at(i).top() / divisor + offsetY + 3);
        screen.setRight(m_screens.at(i).right() / divisor + offsetX - 3);
        screen.setBottom(m_screens.at(i).bottom() / divisor + offsetY - 3);
        m_screens.replace(i, screen);

        if (i == 0 || screen.height() < minimumScreenHeight)
        {
           minimumScreenHeight = screen.height();
        }
        if (i == 0 || screen.height() < minimumScreenWidth)
        {
            minimumScreenWidth = screen.width();
        }
    }
    //The number must be half of the screen smallest dimension (width or height)
    m_screenNumberPixelSize = minimumScreenHeight < minimumScreenWidth ?  minimumScreenHeight : minimumScreenWidth;
    m_screenNumberPixelSize /= 2;
}

} // End namespace udg
