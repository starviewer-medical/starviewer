#ifndef UDGQGRAPHICPLOTWIDGET_H
#define UDGQGRAPHICPLOTWIDGET_H

#include "ui_qgraphicplotwidgetbase.h"

#include <QVector>


namespace udg {


/**
 * Widget que permet dibuxar dades en forma de gràfica.
 */
class QGraphicPlotWidget : public QWidget, private Ui::QGraphicPlotWidgetBase {

    Q_OBJECT

public:

    QGraphicPlotWidget( QWidget *parent = 0 );
    ~QGraphicPlotWidget();

    void paintPoints( );
    void resizeData( );
    void paintEvent(QPaintEvent *);

    void setMaxX(double a);
    void setMinX(double a);
    void setMaxY(double a);
    void setMinY(double a);
    void setTitle( QString s );
    void setData( QVector<double> v, int channel = 0 );
    void setPaintingFeatures( QBrush color = Qt::yellow, qreal width = 2, int channel = 0, Qt::PenStyle style = Qt::SolidLine, Qt::PenCapStyle cap = Qt::SquareCap, Qt::PenJoinStyle join = Qt::BevelJoin );
    void setHold(bool b){m_hold=b;}
    void setAutoLimits(bool b = true) {m_autoXLimits=b;m_autoYLimits=b;}
    void setAutoXLimits(bool b = true) {m_autoXLimits=b;}
    void setAutoYLimits(bool b = true) {m_autoYLimits=b;}
    //bool eventFilter(QObject *object, QEvent *event);
signals:


private:

    /// Crea les connexions de signals i slots.
    void createConnections();

    void movePoint(int index, const QPointF &point, bool emitUpdate);

private slots:

private:

    QVector<QPolygonF> m_dataPlot;
    QVector<QPen> m_dataPlotFeatures;
    QVector<QVector<double> > m_dataSource;
    bool m_hold;
    double m_minX, m_maxX;
    double m_minY, m_maxY;
    double m_width, m_height;
    bool m_autoXLimits;
    bool m_autoYLimits;

};


}


#endif
