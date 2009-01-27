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
    void paintEvent(QPaintEvent *);

    void setMaxX(double a);
    void setMinX(double a);
    void setMaxY(double a);
    void setMinY(double a);
    void setTitle( QString s );
    void setData( QVector<double> v );
signals:


private:

    /// Crea les connexions de signals i slots.
    void createConnections();

private slots:

private:

    QVector<QPolygonF> m_data;
    bool m_hold;
    double m_minX, m_maxX;
    double m_minY, m_maxY;

};


}


#endif
