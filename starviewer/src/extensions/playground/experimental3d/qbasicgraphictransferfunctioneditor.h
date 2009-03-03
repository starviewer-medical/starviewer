#ifndef UDGQBASICGRAPHICTRANSFERFUNCTIONEDITOR_H
#define UDGQBASICGRAPHICTRANSFERFUNCTIONEDITOR_H


#include <QWidget>

#include <QLinearGradient>

#include "transferfunction.h"


namespace udg {


/**
 * Editor de funcions de transferència gràfic en condicions.
 *
 * \todo - Desfer.
 *       - Millorar la documentació.
 */
class QBasicGraphicTransferFunctionEditor : public QWidget {

    Q_OBJECT

public:

    QBasicGraphicTransferFunctionEditor( QWidget *parent = 0 );
    ~QBasicGraphicTransferFunctionEditor();

    /// Assigna el rang de valors de la funció de transferència.
    void setRange( double minimum, double maximum );
    /// Retorna la funció de transferència.
    const TransferFunction& transferFunction() const;
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );

public slots:

    /// Assigna el nom de la funció de transferència.
    void setTransferFunctionName( const QString &name );
    /// Ajusta el mínim i el mínim i el màxim de manera que coincideixin amb els extrems de la funció.
    void adjustRangeToFunction();

signals:

    /// S'emet quan canvia el rang.
    void rangeChanged( double minimum, double maximum );

protected:

    virtual bool event( QEvent *event );
    virtual void keyPressEvent( QKeyEvent *event );
    virtual void mousePressEvent( QMouseEvent *event );
    virtual void mouseMoveEvent( QMouseEvent *event );
    virtual void mouseReleaseEvent( QMouseEvent *event );
    virtual void wheelEvent( QWheelEvent *event );
    virtual void paintEvent( QPaintEvent *event );
    virtual void resizeEvent( QResizeEvent *event );

private:

    static const double POINT_SIZE;

    void updateColorGradient();
    void drawBackground();
    void drawFunction();
    void drawSelectionRectangle();

    QPointF pixelToFunctionPoint( const QPoint &pixel ) const;
    QPointF functionPointToGraphicPoint( const QPointF &functionPoint ) const;

    double nearestX( const QPoint &pixel, bool &found ) const;

    void addPoint( double x, double y );
    void removePoint( double x );
    void changePointColor( double x, QColor &color );
    void changeCurrentPoint( double x, double y );

    void clearSelection();
    void selectAll();

private:

    /// Valor de propietat mínim.
    double m_minimum;
    /// Valor de propietat màxim.
    double m_maximum;
    /// Funció de transferència representada.
    TransferFunction m_transferFunction;

    /// Gradient per representar els colors de la funció de transferència (horitzontal).
    QLinearGradient m_colorGradient;

    bool m_dragging;
    double m_currentX;
    TransferFunction m_transferFunctionCopy;    // còpia de la funció de transferència necessària per arrossegar punts

    bool m_selecting;
    QRect m_selectionRectangle;
    QList<double> m_selectedPoints;
    QList<double> m_selectedPointsCopy;         // còpia dels punts seleccionats necessària per arrossegar punts

};


}


#endif
