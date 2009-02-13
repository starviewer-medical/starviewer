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


const double QBasicGraphicTransferFunctionEditor::POINT_SIZE = 4.0;


QBasicGraphicTransferFunctionEditor::QBasicGraphicTransferFunctionEditor( QWidget *parent )
 : QWidget( parent ), m_minimum( 0.0 ), m_maximum( 4095.0 ), m_dragging( false )
{
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
    m_transferFunction = transferFunction;

    updateColorGradient();
    update();
}


void QBasicGraphicTransferFunctionEditor::setTransferFunctionName( const QString &name )
{
    m_transferFunction.setName( name );
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


void QBasicGraphicTransferFunctionEditor::mousePressEvent( QMouseEvent *event )
{
    bool found;
    double nearestPointX = nearestX( event->pos(), found );

    if ( event->button() == Qt::LeftButton )
    {
        m_dragging = true;

        if ( found ) m_currentX = nearestPointX;
        else
        {
            QPointF functionPoint = pixelToFunctionPoint( event->pos() );
            addPoint( functionPoint.x(), functionPoint.y() );
            m_currentX = functionPoint.x();
        }

        m_transferFunctionCopy = m_transferFunction;
    }
    else if ( event->button() == Qt::RightButton )
    {
        if ( found ) removePoint( nearestPointX );
    }
    else if ( event->button() == Qt::MidButton )
    {
        if ( found )
        {
            QColor color = QColorDialog::getColor( m_transferFunction.getColor( nearestPointX ), this );
            if ( color.isValid() ) changePointColor( nearestPointX, color );
        }
        else
        {
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
    QPointF functionPoint = pixelToFunctionPoint( event->pos() );
    double x = functionPoint.x(), y = functionPoint.y();

    // comprovem les tecles modificadores activades per restringir el moviment en x o en y o arrodonir
    Qt::KeyboardModifiers modifiers = event->modifiers();
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


void QBasicGraphicTransferFunctionEditor::mouseReleaseEvent( QMouseEvent *event )
{
    Q_UNUSED( event );

    m_dragging = false;

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

    foreach ( QPointF point, function )
    {
        QRectF rectangle( point.x() - POINT_SIZE, point.y() - POINT_SIZE, 2.0 * POINT_SIZE, 2.0 * POINT_SIZE );
        painter.drawEllipse( rectangle );
    }
}


double QBasicGraphicTransferFunctionEditor::nearestX( const QPoint &pixel, bool &found ) const
{
    found = false;

    QPointF functionPoint = pixelToFunctionPoint( pixel );
    double radiusX = ( m_maximum - m_minimum ) / ( width() - 1 ) * POINT_SIZE;

    QList<double> nearPoints = m_transferFunction.getPointsNear( functionPoint.x(), radiusX );
    double nearestPointX = 0.0;
    double nearestLength = 2.0 * POINT_SIZE;
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

    QColor color = m_transferFunction.getColor( m_currentX );
    m_transferFunction.removePoint( m_currentX );
    color.setAlphaF( y );
    m_transferFunction.addPoint( x, color );

    updateColorGradient();
    update();
}


}
