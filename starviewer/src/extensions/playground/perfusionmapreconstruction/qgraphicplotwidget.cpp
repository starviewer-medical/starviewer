#include "qgraphicplotwidget.h"

#include "logging.h"

#include <QPainter>


namespace udg {


QGraphicPlotWidget::QGraphicPlotWidget( QWidget *parent )
 : QWidget( parent ), m_hold(false), m_minX(0.0), m_maxX(1.0), m_minY(0.0), m_maxY(1.0)
{
    setupUi( this );

    createConnections();

    m_data = QVector<QPolygonF>(1);
}


QGraphicPlotWidget::~QGraphicPlotWidget()
{
}


void QGraphicPlotWidget::createConnections()
{
}

void QGraphicPlotWidget::paintPoints( )
{
    //DEBUG_LOG("Paint!!");
    QPainter paint(this);
    paint.setRenderHint(QPainter::Antialiasing);
    paint.setPen(QPen(Qt::yellow,2));
    paint.translate(m_plotView->x(),m_plotView->y());
    paint.drawPolyline(m_data[0]);
}

///Caldria implementar el resize!!!

void QGraphicPlotWidget::paintEvent(QPaintEvent *)
{
    //DEBUG_LOG("Paint Event Init!!");
    QPainter p(this);
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    p.translate(m_plotView->x(),m_plotView->y());
    p.drawRect(0, 0, m_plotView->width() - 1, m_plotView->height() - 1);
    
    this->paintPoints();
    //DEBUG_LOG("Paint Event End!!");
}

void QGraphicPlotWidget::setMaxX( double a )
{
    m_maxXLabel->setNum(a);
    m_maxX=a;
}

void QGraphicPlotWidget::setMinX( double a )
{
    m_minXLabel->setNum(a);
    m_minX=a;
}

void QGraphicPlotWidget::setMaxY( double a )
{
    QString s(QString::number(a,'e',3));
    m_maxYLabel->setText(s);
    m_maxY=a;
}

void QGraphicPlotWidget::setMinY( double a )
{
    QString s(QString::number(a,'e',3));
    m_minYLabel->setText(s);
    m_minY=a;
}

void QGraphicPlotWidget::setTitle( QString s )
{
    m_title->setText( s );
}

void QGraphicPlotWidget::setData( QVector<double> v )
{
    int t;
    QPolygonF p;
    double px,py;

    for (t=0;t<v.size();t++)
    {
        if((t >= m_minX) && (t <= m_maxX))
        {
            px = ((double)t-m_minX)*m_plotView->size().width()/(m_maxX-m_minX);
            
            if(v[t] > m_maxY)
            {
                py = m_maxY;
            }
            else if(v[t] < m_minY)
            {
                py = m_minY;
            }
            else
            {
                py = (double) m_plotView->size().height() - (((v[t]-m_minY) * (double)m_plotView->size().height()) / (m_maxY-m_minY));
            }
            p << QPointF( px , py);
        }
        DEBUG_LOG(QString("%1 -> [%2,%3], pos = [%4,%5], size = [%6,%7]" ).arg(t).arg(px).arg(py).arg(m_maxY).arg(m_minY).arg(m_plotView->size().width()).arg(m_plotView->size().height()));
    }
    if(m_hold)
    {
        m_data.push_back(p);
        //m_plotView->setPoints(p);
        
    }
    else
    {
        m_data[0]=p;
        //m_plotView->setPoints(p);
    }

    //this->update();
    this->repaint();
}


}
