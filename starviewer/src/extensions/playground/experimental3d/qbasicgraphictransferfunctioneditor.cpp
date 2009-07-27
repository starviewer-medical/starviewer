#include "qbasicgraphictransferfunctioneditor.h"

#include <QColorDialog>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPolygonF>
#include <QResizeEvent>
#include <QToolTip>

namespace udg {

const double QBasicGraphicTransferFunctionEditor::PointSize = 4.0;

QBasicGraphicTransferFunctionEditor::QBasicGraphicTransferFunctionEditor( QWidget *parent )
 : QWidget( parent ), m_minimum( -50000.0 ), m_maximum( 50000.0 ), m_dragging( false ), m_selecting( false )
{
    setFocusPolicy( Qt::WheelFocus );

    // escacs
    QPixmap pixmap( 20, 20 );
    QPainter painter( &pixmap );
    painter.fillRect( 0, 0, 10, 10, Qt::lightGray );
    painter.fillRect( 10, 10, 10, 10, Qt::lightGray );
    painter.fillRect( 0, 10, 10, 10, Qt::darkGray );
    painter.fillRect( 10, 0, 10, 10, Qt::darkGray );
    painter.end();
    QPalette palette = this->palette();
    palette.setBrush( backgroundRole(), pixmap );
    setAutoFillBackground( true );
    setPalette( palette );

    updateColorGradient();
}

QBasicGraphicTransferFunctionEditor::~QBasicGraphicTransferFunctionEditor()
{
}

double QBasicGraphicTransferFunctionEditor::minimum() const
{
    return m_minimum;
}

double QBasicGraphicTransferFunctionEditor::maximum() const
{
    return m_maximum;
}

void QBasicGraphicTransferFunctionEditor::setRange( double minimum, double maximum )
{
    Q_ASSERT( minimum < maximum );

    if ( m_minimum != minimum || m_maximum != maximum )
    {
        m_minimum = minimum;
        m_maximum = maximum;

        updateColorGradient();
        update();

        emit rangeChanged( m_minimum, m_maximum );
    }
}

const TransferFunction& QBasicGraphicTransferFunctionEditor::transferFunction() const
{
    return m_transferFunction;
}

void QBasicGraphicTransferFunctionEditor::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction.normalize();

    updateColorGradient();
    update();
}

void QBasicGraphicTransferFunctionEditor::setTransferFunctionName( const QString &name )
{
    m_transferFunction.setName( name );
}

void QBasicGraphicTransferFunctionEditor::adjustRangeToFunction()
{
    QList<double> points = m_transferFunction.getPoints();

    if ( points.size() < 2 ) return;

    setRange( points.first(), points.last() );
}

bool QBasicGraphicTransferFunctionEditor::event( QEvent *event )
{
    if ( event->type() == QEvent::ToolTip )
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent*>( event );
        bool found;
        double x = nearestX( helpEvent->pos(), found );

        if ( found )
        {
            double y = m_transferFunction.getOpacity( x );
            QToolTip::showText( helpEvent->globalPos(), QString( "(%1, %2)" ).arg( x ).arg( y ) );
        }
        else QToolTip::hideText();
    }

    return QWidget::event(event);
}

void QBasicGraphicTransferFunctionEditor::keyPressEvent( QKeyEvent *event )
{
    switch ( event->key() )
    {
        case Qt::Key_Space: adjustRangeToFunction(); break;
        case Qt::Key_Escape: clearSelection(); break;
        case Qt::Key_A: if ( event->modifiers().testFlag( Qt::ControlModifier ) ) selectAll(); break;
    }
}

void QBasicGraphicTransferFunctionEditor::mousePressEvent( QMouseEvent *event )
{
    bool found;
    double nearestPointX = nearestX( event->pos(), found );

    if ( event->button() == Qt::LeftButton )
    {
        Qt::KeyboardModifiers modifiers = event->modifiers();

        if ( modifiers.testFlag( Qt::ControlModifier ) && modifiers.testFlag( Qt::ShiftModifier ) )
        {
            m_selecting = true;
            m_selectionRectangle = QRect( event->pos(), event->pos() );
        }
        else
        {
            m_dragging = true;

            if ( found )
            {
                m_currentX = nearestPointX;
                if ( !m_selectedPoints.contains( m_currentX ) ) clearSelection();
            }
            else
            {
                clearSelection();
                QPointF functionPoint = pixelToFunctionPoint( event->pos() );
                addPoint( functionPoint.x(), functionPoint.y() );
                m_currentX = functionPoint.x();
            }

            m_transferFunctionCopy = m_transferFunction;
            m_selectedPointsCopy = m_selectedPoints;
        }
    }
    else if ( event->button() == Qt::RightButton )
    {
        clearSelection();
        if ( found ) removePoint( nearestPointX );
    }
    else if ( event->button() == Qt::MidButton )
    {
        if ( found )
        {
            if ( !m_selectedPoints.contains( m_currentX ) ) clearSelection();
            QColor color = QColorDialog::getColor( m_transferFunction.getColor( nearestPointX ), this );
            if ( color.isValid() ) changePointColor( nearestPointX, color );
        }
        else
        {
            clearSelection();

            QColor color = QColorDialog::getColor( Qt::black, this );

            if ( color.isValid() )
            {
                QPointF functionPoint = pixelToFunctionPoint( event->pos() );
                addPoint( functionPoint.x(), functionPoint.y() );
                changePointColor( functionPoint.x(), color );
            }
        }
    }
}

void QBasicGraphicTransferFunctionEditor::mouseMoveEvent( QMouseEvent *event )
{
    Qt::KeyboardModifiers modifiers = event->modifiers();

    if ( m_selecting )
    {
        if ( modifiers.testFlag( Qt::ControlModifier ) && modifiers.testFlag( Qt::ShiftModifier ) )
        {
            m_selectionRectangle.setBottomRight( event->pos() );
            update();
        }
        else
        {
            m_selecting = false;
            update();
        }
    }
    else if ( m_dragging )
    {
        QPointF functionPoint = pixelToFunctionPoint( event->pos() );
        double x = functionPoint.x(), y = functionPoint.y();

        if ( modifiers.testFlag( Qt::ShiftModifier ) ) x = m_currentX;
        if ( modifiers.testFlag( Qt::ControlModifier ) ) y = m_transferFunctionCopy.getOpacity( m_currentX );
        if ( modifiers.testFlag( Qt::MetaModifier ) ) x = qRound( x );
        if ( modifiers.testFlag( Qt::AltModifier ) ) y = qRound( 10.0 * y ) / 10.0;

        // vigilem que no es surti de rang
        if ( x < m_minimum ) x = m_minimum;
        if ( x > m_maximum ) x = m_maximum;
        if ( y < 0.0 ) y = 0.0;
        if ( y > 1.0 ) y = 1.0;

        changeCurrentPoint( x, y );

        QPoint pixel = functionPointToGraphicPoint( QPointF( x, y ) ).toPoint();
        QToolTip::showText( mapToGlobal( pixel ), QString( "(%1, %2)" ).arg( x ).arg( y ) );
    }
}

void QBasicGraphicTransferFunctionEditor::mouseReleaseEvent( QMouseEvent *event )
{
    Q_UNUSED( event );

    m_dragging = false;

    if ( m_selecting )
    {
        m_selecting = false;

        QRect selectionRectangle = m_selectionRectangle.intersected( this->rect() );
        QPointF functionTopLeft = pixelToFunctionPoint( selectionRectangle.topLeft() );
        QPointF functionBottomRight = pixelToFunctionPoint( QPoint( selectionRectangle.left() + selectionRectangle.width(),
                                                                    selectionRectangle.top() + selectionRectangle.height() ) );
        QList<double> pointsInInterval = m_transferFunction.getPointsInInterval( functionTopLeft.x(), functionBottomRight.x() );
        int nPoints = pointsInInterval.size();

        for ( int i = 0; i < nPoints; i++ )
        {
            double x = pointsInInterval.at( i );
            double y = m_transferFunction.getOpacity( x );

            if ( y >= functionBottomRight.y() && y <= functionTopLeft.y() && !m_selectedPoints.contains( x ) ) m_selectedPoints << x;
        }

        update();
    }

    QToolTip::hideText();
}

void QBasicGraphicTransferFunctionEditor::wheelEvent( QWheelEvent *event )
{
    int zoom = event->delta() / 8;

    if ( 2 * zoom >= m_maximum - m_minimum ) return;

    m_minimum += zoom;
    m_maximum -= zoom;

    updateColorGradient();
    update();

    emit rangeChanged( m_minimum, m_maximum );
}

void QBasicGraphicTransferFunctionEditor::paintEvent( QPaintEvent *event )
{
    Q_UNUSED( event );

    drawBackground();
    drawFunction();
    if ( m_selecting ) drawSelectionRectangle();
}

void QBasicGraphicTransferFunctionEditor::resizeEvent( QResizeEvent *event )
{
    Q_UNUSED( event );

    updateColorGradient();
    // l'update() ja es crida automàticament
}

void QBasicGraphicTransferFunctionEditor::updateColorGradient()
{
    m_colorGradient = QLinearGradient( 0, 0, width(), 0 );

    double shift = -m_minimum;
    double scale = 1.0 / ( m_maximum - m_minimum );
    QList<double> colors = m_transferFunction.getColorPoints();
    int nColors = colors.size();

    for ( int i = 0; i < nColors; i++ )
    {
        double x = colors.at( i );
        double x01 = ( x + shift ) * scale;

        if ( x01 < 0.0 )
        {
            x01 = 0.0;
            x = x01 / scale - shift;
        }

        if ( x01 > 1.0 )
        {
            x01 = 1.0;
            x = x01 / scale - shift;
        }

        QColor color = m_transferFunction.getColor( x );
        color.setAlpha( 255 );
        m_colorGradient.setColorAt( x01, color );
    }
}

void QBasicGraphicTransferFunctionEditor::drawBackground()
{
    QImage background( size(), QImage::Format_ARGB32_Premultiplied );
    background.fill( 0 );

    QPainter backgroundPainter( &background );
    backgroundPainter.fillRect( rect(), m_colorGradient );
    backgroundPainter.setCompositionMode( QPainter::CompositionMode_DestinationAtop );

    QLinearGradient alphaGradient( 0, 0, 0, height() );
    alphaGradient.setColorAt( 0.0, QColor::fromRgbF( 0.0, 0.0, 0.0, 1.0 ) );
    alphaGradient.setColorAt( 1.0, QColor::fromRgbF( 0.0, 0.0, 0.0, 0.0 ) );

    backgroundPainter.fillRect( rect(), alphaGradient );

    QPainter painter( this );
    painter.drawImage( 0, 0, background );
}

void QBasicGraphicTransferFunctionEditor::drawFunction()
{
    QPolygonF function;
    QList<double> points = m_transferFunction.getPoints();
    int nPoints = points.size();

    for ( int i = 0; i < nPoints; i++ )
    {
        double x = points.at( i );
        function << functionPointToGraphicPoint( QPointF( x, m_transferFunction.getOpacity( x ) ) );
    }

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setPen( QColor( 251, 251, 251, 216 ) );
    painter.setBrush( QColor( 248, 248, 248, 202 ) );

    painter.drawPolyline( function );

    for ( int i = 0; i < nPoints; i++ )
    {
        QPointF point = function.at( i );

        if ( m_selectedPoints.contains( points.at( i ) ) ) painter.setBrush( QColor( 99, 39, 77, 189 ) );
        else painter.setBrush( QColor( 248, 248, 248, 202 ) );

        QRectF rectangle( point.x() - PointSize, point.y() - PointSize, 2.0 * PointSize, 2.0 * PointSize );
        painter.drawEllipse( rectangle );
    }
}

void QBasicGraphicTransferFunctionEditor::drawSelectionRectangle()
{
    QPainter painter( this );
    painter.setPen( QColor( 223, 137, 136, 118 ) );
    painter.drawRect( m_selectionRectangle.normalized() );
}

double QBasicGraphicTransferFunctionEditor::nearestX( const QPoint &pixel, bool &found ) const
{
    found = false;

    QPointF functionPoint = pixelToFunctionPoint( pixel );
    double radiusX = ( m_maximum - m_minimum ) / ( width() - 1 ) * PointSize;

    QList<double> nearPoints = m_transferFunction.getPointsNear( functionPoint.x(), radiusX );
    double nearestPointX = 0.0;
    double nearestLength = 2.0 * PointSize;
    int nPoints = nearPoints.size();

    for ( int i = 0; i < nPoints; i++ )
    {
        double fx = nearPoints.at( i );
        double fy = m_transferFunction.getOpacity( fx );
        QPointF graphicPoint = functionPointToGraphicPoint( QPointF( fx, fy ) );
        double length = QLineF( pixel, graphicPoint ).length();

        if ( length < nearestLength )
        {
            nearestPointX = fx;
            nearestLength = length;
            found = true;
        }
    }

    return nearestPointX;
}

QPointF QBasicGraphicTransferFunctionEditor::pixelToFunctionPoint( const QPoint &pixel ) const
{
    double scaleX = ( m_maximum - m_minimum ) / ( width() - 1 );
    double shiftX = m_minimum;
    double scaleY = 1.0 / -( height() - 1 );
    double shiftY = 1.0;

    return QPointF( scaleX * pixel.x() + shiftX, scaleY * pixel.y() + shiftY );
}

QPointF QBasicGraphicTransferFunctionEditor::functionPointToGraphicPoint( const QPointF &functionPoint ) const
{
    double shiftX = -m_minimum;
    double scaleX = ( width() - 1 ) / ( m_maximum - m_minimum );
    double shiftY = -1.0;
    double scaleY = -( height() - 1 );

    return QPointF( ( functionPoint.x() + shiftX ) * scaleX, ( functionPoint.y() + shiftY ) * scaleY );
}

void QBasicGraphicTransferFunctionEditor::addPoint( double x, double y )
{
    QColor color = Qt::black;
    color.setAlphaF( y );
    m_transferFunction.addPoint( x, color );

    updateColorGradient();
    update();
}

void QBasicGraphicTransferFunctionEditor::removePoint( double x )
{
    m_transferFunction.removePoint( x );

    updateColorGradient();
    update();
}

void QBasicGraphicTransferFunctionEditor::changePointColor( double x, QColor &color )
{
    color.setAlphaF( m_transferFunction.getOpacity( x ) );
    m_transferFunction.addPoint( x, color );

    updateColorGradient();
    update();
}

void QBasicGraphicTransferFunctionEditor::changeCurrentPoint( double x, double y )
{
    if ( !m_dragging ) return;

    m_transferFunction = m_transferFunctionCopy;

    if ( m_selectedPoints.isEmpty() )
    {
        QColor color = m_transferFunction.getColor( m_currentX );
        m_transferFunction.removePoint( m_currentX );
        color.setAlphaF( y );
        m_transferFunction.addPoint( x, color );
    }
    else
    {
        m_selectedPoints = m_selectedPointsCopy;

        double currentY = m_transferFunction.getOpacity( m_currentX );
        double deltaX = x - m_currentX;
        double deltaY = y - currentY;
        int nSelected = m_selectedPoints.size();

        for ( int i = 0; i < nSelected; i++ )
        {
            double sx = m_selectedPoints.at( i );
            QColor rgba = m_transferFunction.get( sx );
            m_transferFunction.removePoint( sx );
            rgba.setAlphaF( qBound( 0.0, rgba.alphaF() + deltaY, 1.0 ) );
            m_transferFunction.addPoint( sx + deltaX, rgba );
            m_selectedPoints[i] = sx + deltaX;
        }
    }

    updateColorGradient();
    update();
}

void QBasicGraphicTransferFunctionEditor::clearSelection()
{
    m_selectedPoints.clear();
    update();
}

void QBasicGraphicTransferFunctionEditor::selectAll()
{
    m_selectedPoints.clear();
    m_selectedPoints << m_transferFunction.getPoints();
    update();
}

}
