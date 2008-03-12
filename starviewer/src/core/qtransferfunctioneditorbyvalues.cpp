/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "qtransferfunctioneditorbyvalues.h"

#include <cmath>

#include <QBoxLayout>
#include <QScrollArea>
#include <QScrollBar>

#include "qtransferfunctionintervaleditor.h"


namespace udg {


QTransferFunctionEditorByValues::QTransferFunctionEditorByValues( QWidget * parent )
    : QTransferFunctionEditor( parent )
{
    setupUi( this );


    // Creem una scroll area per si hi ha molts intervals (no es pot crear des del Qt Designer)

    QScrollArea * scrollArea = new QScrollArea( this );
    qobject_cast< QBoxLayout * >( this->layout() )->insertWidget( 1, scrollArea );

    m_intervalEditorsWidget = new QWidget( scrollArea );
    QBoxLayout * layout = new QVBoxLayout( m_intervalEditorsWidget );
    m_intervalEditorsLayout = new QVBoxLayout();
    layout->addLayout( m_intervalEditorsLayout );
    layout->addStretch();
    layout->setMargin( 0 );

    scrollArea->setWidget( m_intervalEditorsWidget );
    scrollArea->setWidgetResizable( true );
    scrollArea->setFrameShape( QFrame::NoFrame );


    // Creem el primer interval

    QTransferFunctionIntervalEditor * first = new QTransferFunctionIntervalEditor( m_intervalEditorsWidget );
    first->setIsFirst( true );
    first->setIsLast( true );
    first->setObjectName( "interval0" );
    m_intervalEditorsLayout->addWidget( first );

    connect( first, SIGNAL( startChanged(int) ), SLOT( markAsChanged() ) );
    connect( first, SIGNAL( endChanged(int) ), SLOT( markAsChanged() ) );
    connect( first, SIGNAL( colorChanged(const QColor&) ), SLOT( markAsChanged() ) );


    // Mida mínima de la scroll area

    QScrollBar scrollBar( Qt::Vertical );   // necessitem una scroll bar auxiliar per a saber-ne l'amplada
    scrollBar.show();
    scrollArea->setMinimumWidth( first->minimumSizeHint().width() + scrollBar.width() );
    scrollBar.hide();


    m_numberOfIntervals = 1;

    m_changed = true;

    connect( m_addPushButton, SIGNAL( clicked() ), SLOT( addInterval() ) );
    connect( m_removePushButton, SIGNAL( clicked() ), SLOT( removeInterval() ) );
    connect( m_nameLineEdit, SIGNAL( textChanged(const QString&) ), SLOT( markAsChanged() ) );
}


QTransferFunctionEditorByValues::~QTransferFunctionEditorByValues()
{
}


void QTransferFunctionEditorByValues::setMaximum( unsigned short maximum )
{
    QTransferFunctionEditor::setMaximum( maximum );

    QList<QTransferFunctionIntervalEditor *> intervalList =
        m_intervalEditorsWidget->findChildren<QTransferFunctionIntervalEditor *>();
    QTransferFunctionIntervalEditor * interval;
    foreach ( interval, intervalList )
    {
        interval->setMaximum( m_maximum );
    }

    m_changed = true;
}


void QTransferFunctionEditorByValues::setTransferFunction( const TransferFunction & transferFunction )
{
    // si no hi ha hagut canvis i ens passen una funció igual llavors no cal fer res
    if ( !m_changed && m_transferFunction == transferFunction ) return;

    m_nameLineEdit->setText( transferFunction.name() );

    while ( m_numberOfIntervals > 1 ) removeInterval();

    QTransferFunctionIntervalEditor * current =
            m_intervalEditorsWidget->findChild< QTransferFunctionIntervalEditor * >( "interval0" );
    // sempre tindrem a punt el següent (per evitar restriccions amb els valors)
    QTransferFunctionIntervalEditor * next = addIntervalAndReturnIt();

    QList< double > points = transferFunction.getPoints();
    bool lastIsInterval = false;

    for ( unsigned short i = 0; i < points.size(); i++ )
    {
        double x = points.at( i );

        if ( i == 0 ) current->setIsInterval( false );  // cas especial: primer

        if ( i == 0 || transferFunction.get( x ) != current->color() )
        {
            if ( i > 0 )
            {
                current = next;
                if ( i < points.size() - 1 ) next = addIntervalAndReturnIt();   // si és l'últim no en creem cap més
            }

            current->setStart( static_cast< int >( qRound( x ) ) );
            current->setColor( transferFunction.get( x ) );
            lastIsInterval = false;
        }
        else
        {
            current->setIsInterval( true );
            current->setEnd( static_cast< int >( qRound( x ) ) );
            lastIsInterval = true;
        }
    }

    if ( lastIsInterval ) removeInterval();

    m_changed = true;
    getTransferFunction();  // actualitzem m_transferFunction
}


const TransferFunction & QTransferFunctionEditorByValues::getTransferFunction() const
{
    if ( m_changed )
    {
        m_transferFunction.clear();

        m_transferFunction.setName( m_nameLineEdit->text() );

        QList< QTransferFunctionIntervalEditor * > intervalList =
                m_intervalEditorsWidget->findChildren< QTransferFunctionIntervalEditor * >();
        QTransferFunctionIntervalEditor * interval;
        foreach ( interval, intervalList )
        {
            m_transferFunction.addPoint( interval->start(), interval->color() );
            if ( interval->isInterval() ) m_transferFunction.addPoint( interval->end(), interval->color() );
        }

        m_changed = false;
    }

    return m_transferFunction;
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


QTransferFunctionIntervalEditor * QTransferFunctionEditorByValues::addIntervalAndReturnIt()
{
    if ( m_numberOfIntervals == m_maximum + 1u ) return 0;

    QTransferFunctionIntervalEditor * last =
            m_intervalEditorsWidget->findChild< QTransferFunctionIntervalEditor * >(
            QString( "interval%1" ).arg( m_numberOfIntervals - 1 ) );
    QTransferFunctionIntervalEditor * afterLast
            = new QTransferFunctionIntervalEditor( m_intervalEditorsWidget );
    afterLast->setMaximum( m_maximum );

    connect( last, SIGNAL( endChanged(int) ), afterLast, SLOT( setPreviousEnd(int) ) );
    connect( afterLast, SIGNAL( startChanged(int) ), last, SLOT( setNextStart(int) ) );

    connect( afterLast, SIGNAL( startChanged(int) ), SLOT( markAsChanged() ) );
    connect( afterLast, SIGNAL( endChanged(int) ), SLOT( markAsChanged() ) );
    connect( afterLast, SIGNAL( colorChanged(const QColor&) ), SLOT( markAsChanged() ) );

    last->setIsLast( false );

    afterLast->setIsLast( true );
    afterLast->setObjectName( QString( "interval%1" ).arg( m_numberOfIntervals ) );

    m_intervalEditorsLayout->addWidget( afterLast );

    m_numberOfIntervals++;

    markAsChanged();

    return afterLast;
}


void QTransferFunctionEditorByValues::markAsChanged()
{
    m_changed = true;
}


}
