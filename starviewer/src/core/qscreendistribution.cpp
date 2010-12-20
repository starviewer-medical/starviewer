/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qscreendistribution.h"

#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>
#include <QSize>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QWidgetAction>

namespace udg {

QScreenDistribution::QScreenDistribution(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    m_mouseInScreen = -1;
    m_marging = 50;
    this->setMinimumSize(QSize(300,200));
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
        setCursor(Qt::ArrowCursor);
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
    QList<QPoint> centers;
    m_screens.clear();

    // Calcul de les posicions on s'haurà de pintar les diferents icones de finestres. Es passen
    // les dades com a punters, així es poden emplenar els vectors dins el mètode.
    computeSizesAndPositions(&m_screens, &centers);

    QPainter painter(this);
    QPen pen = QPen(Qt::blue, 2, Qt::SolidLine);
    painter.setPen(pen);

    for (int i = 0; i < m_screens.count(); i++)
    {
        int topLeftX = m_screens.at(i).left();
        int topLeftY = m_screens.at(i).top();
        int bottomRightX = m_screens.at(i).right();
        int bottomRightY = m_screens.at(i).bottom();
        QPoint center = centers.at(i);
        int centerX = center.x();
        int centerY = center.y();
        
        // Pintar el requadre de la pantalla numero i
        painter.drawLine(topLeftX, topLeftY, bottomRightX, topLeftY);
        painter.drawLine(bottomRightX, topLeftY, bottomRightX, bottomRightY);
        painter.drawLine(bottomRightX, bottomRightY, topLeftX, bottomRightY);
        painter.drawLine(topLeftX, bottomRightY, topLeftX, topLeftY);
        
        if (m_mouseInScreen == i)
        {
            // Pintar el cuadre com a seleccionat, amb relleu
            // La part de dalt i esquerra de blau clar
            pen = QPen(QColor(60,60,255), 2, Qt::SolidLine);
            painter.setPen(pen);
            painter.drawLine(topLeftX+1, topLeftY+1, bottomRightX-1, topLeftY+1);
            painter.drawLine(topLeftX, topLeftY, bottomRightX, topLeftY);
            painter.drawLine(topLeftX+1, bottomRightY-1, topLeftX+1, topLeftY+1);
            painter.drawLine(topLeftX, bottomRightY, topLeftX, topLeftY);

            // La part de baix i la dreta de blau més fosc
            pen = QPen(QColor(0,0,128), 2, Qt::SolidLine);
            painter.setPen(pen);
            painter.drawLine(bottomRightX-1, topLeftY+1, bottomRightX-1, bottomRightY-1);
            painter.drawLine(bottomRightX, topLeftY, bottomRightX, bottomRightY);
            painter.drawLine(bottomRightX-1, bottomRightY-1, topLeftX+1, bottomRightY-1);
            painter.drawLine(bottomRightX, bottomRightY, topLeftX, bottomRightY);
            
            pen = QPen(Qt::blue, 2, Qt::SolidLine);
            painter.setPen(pen);
        }
        
        //Pintar el numero
        QPoint point(centerX - 20, centerY - 7);
        QSize size(40, 40);
        QRectF rectangle(point, size); 
        painter.drawText(rectangle, Qt::AlignHCenter, QString::number(i + 1));
    }
    event->accept();
}

void QScreenDistribution::computeSizesAndPositions(QList<QRect> *screens, QList<QPoint> *centers)
{
    QDesktopWidget *desktop = QApplication::desktop();   

    int mainScreen = 0;
    int MinimumX = 0;
    int MinimumY = 0;
    int MaximumX = 0;
    int MaximumY = 0;

    // Per cada pantalla
    for (int i = 0; i < desktop->screenCount(); i++)
    {
        // Agafem les seves coordenades
        QPoint topLeft = desktop->screenGeometry(i).topLeft();
        QPoint bottomRight = desktop->screenGeometry(i).bottomRight();

        screens->append(QRect(topLeft, bottomRight));
        centers->append((topLeft + bottomRight) / 2);

        // I calculem el tamany màxim que ocupa tot el conjunt de pantalles
        if (topLeft.x() < MinimumX)
        {
            MinimumX = topLeft.x();
        }
        if (bottomRight.x() > MaximumX)
        {
            MaximumX = bottomRight.x();
        }
        if (topLeft.y() < MinimumY)
        {
            MinimumY = topLeft.y();
        }
        if (bottomRight.y() > MaximumY)
        {
            MaximumY = bottomRight.y();
        }

        if (i == desktop->primaryScreen())
        {
            mainScreen = i;
        }
    }

    int totalWidth = MaximumX - MinimumX;
    int totalHeight = MaximumY - MinimumY;    

    // Calculem la proporció en que s'ha de dividir per que no es deformi el dibuix
    float widthDivisor = (float)totalWidth / ((float)width() - m_marging * 2);
    float heightDivisor = (float)totalHeight / ((float)height() - m_marging * 2);

    float divisor = (widthDivisor < heightDivisor) ? heightDivisor : widthDivisor;
    int offsetX = 0;
    int offsetY = 0;

    if (widthDivisor > heightDivisor)
    {
        float centering = (height() - m_marging * 2 - (totalHeight / divisor)) / 2;
        offsetX = -1 * MinimumX / divisor + m_marging;
        offsetY = -1 * MinimumY / divisor + m_marging + centering;
    }
    else
    {
        float centering = (width() - m_marging * 2 - (totalWidth / divisor)) / 2;
        offsetX = -1 * MinimumX / divisor + m_marging + centering;
        offsetY = -1 * MinimumY / divisor + m_marging;
    }

    // Adaptem les posicións a les posicions de dibuix escalades i centrades
    for (int i = 0; i < screens->count(); i++)
    {
        // Requadre
        int left = screens->at(i).left() / divisor + offsetX;
        int top = screens->at(i).top() / divisor + offsetY;
        int right = screens->at(i).right() / divisor + offsetX;
        int bottom = screens->at(i).bottom() / divisor + offsetY;
        screens->replace(i, QRect(QPoint(left, top), QPoint(right, bottom)));
        
        // Center
        int x = centers->at(i).x() / divisor + offsetX;
        int y = centers->at(i).y() / divisor + offsetY;
        centers->replace(i, QPoint(x, y));
    }
}

} // end namespace udg