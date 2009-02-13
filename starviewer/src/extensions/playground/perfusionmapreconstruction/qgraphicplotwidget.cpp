#include "qgraphicplotwidget.h"

#include "logging.h"

#include <QPainter>
#include <QResizeEvent>


namespace udg {


QGraphicPlotWidget::QGraphicPlotWidget( QWidget *parent )
 : QWidget( parent ), m_hold(false), m_minX(0.0), m_maxX(1.0), m_minY(0.0), m_maxY(1.0), m_autoXLimits(true), m_autoYLimits(false)
{
    setupUi( this );

    parent->installEventFilter(this);

    createConnections();
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
    if(m_width != m_plotView->size().width() || m_height != m_plotView->size().height())
    {
        this->resizeData();
    }
    int i;
    //DEBUG_LOG(QString("Paint Points!! : %1 %2").arg(m_dataPlot.size()).arg(m_title->text()));
    for (i=0;i<m_dataPlot.size();i++)
    {
        QPainter paint(this);
        paint.setRenderHint(QPainter::Antialiasing);
        paint.translate(m_plotView->x(),m_plotView->y());
        //DEBUG_LOG(QString("Paint Featrues!! : %1 %2").arg(m_dataPlotFeatures[i].color().green()).arg(m_dataPlotFeatures[i].width()));
        paint.setPen(m_dataPlotFeatures[i]);
        paint.drawPolyline(m_dataPlot[i]);
    }
}

void QGraphicPlotWidget::resizeData( )
{
    //DEBUG_LOG("Resize Data");
    int t,i;
    QPolygonF p;
    double px,py;

    m_width = m_plotView->size().width();
    m_height = m_plotView->size().height();

    for (i=0;i<m_dataSource.size();i++)
    {
        for (t=0;t<m_dataSource[i].size();t++)
        {
            if((t >= m_minX) && (t <= m_maxX))
            {
                px = ((double)t-m_minX)*m_width/(m_maxX-m_minX);
                
                if(m_dataSource[i][t] > m_maxY)
                {
                    py = m_maxY;
                }
                else if(m_dataSource[i][t] < m_minY)
                {
                    py = m_minY;
                }
                else
                {
                    py = (double) m_height - (((m_dataSource[i][t]-m_minY) * (double)m_height) / (m_maxY-m_minY));
                }
                p << QPointF( px , py);
            }
            //DEBUG_LOG(QString("%1 -> [%2,%3], pos = [%4,%5], size = [%6,%7]" ).arg(t).arg(px).arg(py).arg(m_maxY).arg(m_minY).arg(m_plotView->size().width()).arg(m_plotView->size().height()));
        }
        m_dataPlot[i]=p;
    }
}

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

void QGraphicPlotWidget::setData( QVector<double> v, int channel )
{
    int t,k;
    QPolygonF p;
    double px,py;
    int old_size;

    //Suposem que tots els plots tenen el mateix nombre de dades!!
    if(m_autoXLimits)
    {
        m_minX=0;
        m_maxX=v.size()-1;
        this->setMinX(m_minX);
        this->setMaxX(m_maxX);
    }

    double maxY,minY;
    if(m_autoYLimits)
    {
        minY = v[0];
        maxY = v[0];
        //mirem per la resta de plots menys en el canal que afegirem
        for (k=0;k<m_dataSource.size();k++)
        {
            if(k!=channel)
            {
                for (t=0;t<m_dataSource[k].size();t++)
                {
                    if(m_dataSource[k][t] > maxY)
                    {
                        maxY = m_dataSource[k][t];
                    }
                    if(m_dataSource[k][t] < minY)
                    {
                        minY = m_dataSource[k][t];
                    }
                }
            }
        }
        //i mirem pel que afegirem
        for (t=0;t<v.size();t++)
        {
            if(v[t] > maxY)
            {
                maxY = v[t];
            }
            if(v[t] < minY)
            {
                minY = v[t];
            }
        }
        this->setMinY(minY);
        this->setMaxY(maxY);
    }

    m_width = m_plotView->size().width();
    m_height = m_plotView->size().height();

    for (t=0;t<v.size();t++)
    {
        if((t >= m_minX) && (t <= m_maxX))
        {
            px = ((double)t-m_minX)*m_width/(m_maxX-m_minX);
            
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
                py = (double) m_height - (((v[t]-m_minY) * (double)m_height) / (m_maxY-m_minY));
            }
            p << QPointF( px , py);
        }
        //DEBUG_LOG(QString("%1 -> [%2,%3], pos = [%4,%5], size = [%6,%7]" ).arg(t).arg(px).arg(py).arg(m_maxY).arg(m_minY).arg(m_plotView->size().width()).arg(m_plotView->size().height()));
    }
    if(m_hold)
    {
        m_dataPlot.push_back(p);
        m_dataSource.push_back(v);
        m_dataPlotFeatures.push_back(QPen(Qt::yellow,1.5));
    }
    else
    {
        //Si el canal on hem de guardar la dad no existeix ampliem la mida
        if(m_dataPlot.size() <= channel)
        {
            m_dataPlot.resize(channel+1);
            m_dataSource.resize(channel+1);
            old_size = m_dataPlotFeatures.size();
            m_dataPlotFeatures.resize(channel+1);
            //Les dataPlotFeatures les inicialitzem al valor per defecte nostre
            for(int i=old_size;i<m_dataPlotFeatures.size();i++)
            {
                m_dataPlotFeatures[i]=QPen(Qt::yellow,1.5);
            }
        }
        m_dataPlot[channel]=p;
        m_dataSource[channel]=v;
    }

    this->update();
}

void QGraphicPlotWidget::setPaintingFeatures( QBrush color, qreal width, int channel, Qt::PenStyle style, Qt::PenCapStyle cap, Qt::PenJoinStyle join )
{
    if(m_dataPlotFeatures.size() > channel)
    {
        m_dataPlotFeatures[channel] = QPen( color, width, style, cap, join);
    }
}

}
