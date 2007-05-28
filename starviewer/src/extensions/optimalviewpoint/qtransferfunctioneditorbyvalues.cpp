/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qtransferfunctioneditorbyvalues.h"

#include <QBoxLayout>
#include <QScrollArea>
#include <math.h>
#include "qtransferfunctionintervaleditor.h"

namespace udg {

QTransferFunctionEditorByValues::QTransferFunctionEditorByValues( QWidget * parent )
 : QWidget( parent )
{
    setupUi( this );

    QScrollArea * scrollArea = new QScrollArea( this );
    qobject_cast< QBoxLayout * >( this->layout() )->insertWidget( 0, scrollArea );

    m_intervalEditorsWidget = new QWidget( scrollArea );
    QVBoxLayout * layout = new QVBoxLayout( m_intervalEditorsWidget );
    m_intervalEditorsLayout = new QVBoxLayout();
    layout->addLayout( m_intervalEditorsLayout );
    layout->addStretch();
    layout->setMargin( 0 );

    scrollArea->setWidget( m_intervalEditorsWidget );
    scrollArea->setWidgetResizable( true );
    scrollArea->setFrameShape( QFrame::NoFrame );

    QTransferFunctionIntervalEditor * first
            = new QTransferFunctionIntervalEditor( m_intervalEditorsWidget );
    first->setIsFirst( true );
    first->setIsLast( true );
    first->setObjectName( "interval0" );
    m_intervalEditorsLayout->addWidget( first );

    connect( first, SIGNAL( startChanged(int) ), this, SLOT( markAsChanged() ) );
    connect( first, SIGNAL( endChanged(int) ), this, SLOT( markAsChanged() ) );

    m_numberOfIntervals = 1;
    m_changed = true;

    connect( m_addPushButton, SIGNAL( clicked() ), this, SLOT( addInterval() ) );
    connect( m_removePushButton, SIGNAL( clicked() ), this, SLOT( removeInterval() ) );
}


QTransferFunctionEditorByValues::~QTransferFunctionEditorByValues()
{
}


const QGradientStops & QTransferFunctionEditorByValues::getTransferFunction() const
{
    //typedef QPair<qreal,QColor> QGradientStop;
    //typedef QVector<QGradientStop> QGradientStops;

    m_transferFunction.clear();

    QList< QTransferFunctionIntervalEditor * > intervalList =
            m_intervalEditorsWidget->findChildren< QTransferFunctionIntervalEditor * >();
    QTransferFunctionIntervalEditor * interval;
    foreach ( interval, intervalList )
        m_transferFunction << interval->getInterval();

    m_changed = false;

    return m_transferFunction;
}


void QTransferFunctionEditorByValues::setTransferFunction( const QGradientStops & transferFunction )
{
    if ( !m_changed && m_transferFunction == transferFunction ) return;

    //typedef QPair<qreal,QColor> QGradientStop;
    //typedef QVector<QGradientStop> QGradientStops;

    while ( m_numberOfIntervals > 1 ) removeInterval();

    QTransferFunctionIntervalEditor * current =
            m_intervalEditorsWidget->findChild< QTransferFunctionIntervalEditor * >( "interval0" );
    // sempre tindrem a punt el següent (per evitar restriccions amb els valors) i l'esborrarem al final
    QTransferFunctionIntervalEditor * next = addIntervalAndReturnIt();

    for ( unsigned short i = 0; i < transferFunction.size(); i++ )
    {
        const QGradientStop & stop = transferFunction[i];

        if ( i == 0 ) current->setIsInterval( false );  // cas especial: primer

        if ( i == 0 || stop.second != current->color() )
        {
            if ( i > 0 )
            {
                current = next;
                next = addIntervalAndReturnIt();
            }

            current->setStart( static_cast< int >( round( stop.first * 255.0 ) ) );
            current->setColor( stop.second );
        }
        else
        {
            current->setIsInterval( true );
            current->setEnd( static_cast< int >( round( stop.first * 255.0 ) ) );
        }
    }

    removeInterval();   // esborrem l'últim interval

    getTransferFunction();  // actualitzem m_transferFunction
}


void QTransferFunctionEditorByValues::addInterval()
{
    addIntervalAndReturnIt();
}


void QTransferFunctionEditorByValues::removeInterval()
{
    if ( m_numberOfIntervals == 1 ) return;

    m_numberOfIntervals--;

    QTransferFunctionIntervalEditor * last =
            m_intervalEditorsWidget->findChild< QTransferFunctionIntervalEditor * >(
            QString( "interval%1" ).arg( m_numberOfIntervals ) );
    last->setParent( 0 );
    delete last;

    QTransferFunctionIntervalEditor * beforeLast =
            m_intervalEditorsWidget->findChild< QTransferFunctionIntervalEditor * >(
            QString( "interval%1" ).arg( m_numberOfIntervals - 1 ) );
    beforeLast->setIsLast( true );

    markAsChanged();
}


void QTransferFunctionEditorByValues::markAsChanged()
{
    m_changed = true;
}


QTransferFunctionIntervalEditor * QTransferFunctionEditorByValues::addIntervalAndReturnIt()
{
    if ( m_numberOfIntervals == 256 ) return 0;

    QTransferFunctionIntervalEditor * last =
            m_intervalEditorsWidget->findChild< QTransferFunctionIntervalEditor * >(
            QString( "interval%1" ).arg( m_numberOfIntervals - 1 ) );
    QTransferFunctionIntervalEditor * afterLast
            = new QTransferFunctionIntervalEditor( m_intervalEditorsWidget );

    connect( last, SIGNAL( endChanged(int) ), afterLast, SLOT( setPreviousEnd(int) ) );
    connect( afterLast, SIGNAL( startChanged(int) ), last, SLOT( setNextStart(int) ) );

    connect( afterLast, SIGNAL( startChanged(int) ), this, SLOT( markAsChanged() ) );
    connect( afterLast, SIGNAL( endChanged(int) ), this, SLOT( markAsChanged() ) );

    last->setIsLast( false );

    afterLast->setIsLast( true );
    afterLast->setObjectName( QString( "interval%1" ).arg( m_numberOfIntervals ) );

    m_intervalEditorsLayout->addWidget( afterLast );

    m_numberOfIntervals++;

    markAsChanged();

    return afterLast;
}


}
