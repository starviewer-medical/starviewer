/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "viewerslayout.h"

#include "logging.h"

//Qt's
#include <QDesktopWidget>
#include <QSize>
#include <QVBoxLayout>

namespace udg {

ViewersLayout::ViewersLayout(QWidget *parent) : QWidget(parent), m_selectedViewer(0)
{
    this->setViewerSelected( this->getNewQ2DViewerWidget() );
    m_vectorViewers.push_back( m_selectedViewer );
    m_selectedViewer->setSelected( true );

    initLayouts();
}

ViewersLayout::~ViewersLayout()
{

}

void ViewersLayout::initLayouts()
{
    m_rows = 1;
    m_columns = 1;
    m_totalRows = 1;
    m_totalColumns = 1;

    m_gridLayout = new QGridLayout();
    m_gridLayout->setSpacing(0);
    m_gridLayout->setMargin(0);

    m_viewersLayout = new QGridLayout();
    m_viewersLayout->setSpacing(0);
    m_viewersLayout->setMargin(0);

    m_viewersLayout->addWidget( m_selectedViewer, 0, 0 );
    m_gridLayout->addLayout( m_viewersLayout, 0, 0 );

    this->setLayout(m_gridLayout);
    m_gridLayout->update();

}

void ViewersLayout::removeLayouts()
{
    int numberOfViewers = m_vectorViewers.size();
    int i;

    for( i = 0; i < numberOfViewers; i++)
    {
        m_viewersLayout->removeWidget( m_vectorViewers.value( i ) );
    }
}

Q2DViewerWidget* ViewersLayout::getViewerSelected()
{
    return m_selectedViewer;
}

Q2DViewerWidget* ViewersLayout::getNewQ2DViewerWidget()
{
    Q2DViewerWidget *newViewer = new Q2DViewerWidget( this );
    connect( newViewer, SIGNAL( selected( Q2DViewerWidget *) ), SLOT( setViewerSelected( Q2DViewerWidget *) ) );
    // per defecte no li posem cap annotació
    newViewer->getViewer()->removeAnnotation( Q2DViewer::AllAnnotation );

    emit viewerAdded( newViewer );
    return newViewer;
}

void ViewersLayout::addColumns( int columns )
{
    int posViewer = m_columns;
    Q2DViewerWidget *newViewer;

    int rows;
    while( columns > 0 )
    {
        rows = 0;
        m_columns += 1;
        m_totalColumns += 1;
        while( rows < m_viewersLayout->rowCount() )
        {
            newViewer = getNewQ2DViewerWidget();
            m_viewersLayout->addWidget( newViewer, rows, m_totalColumns-1);
            m_vectorViewers.insert( posViewer,newViewer );
            posViewer += m_columns;
            if( rows >= m_rows )
            {
                newViewer->hide();
                emit viewerRemoved( newViewer );
            }
            rows++;
        }
        posViewer = m_columns;
        columns--;
    }
}

void ViewersLayout::addRows( int rows )
{
    Q2DViewerWidget *newViewer;
    int column;

    while( rows > 0 )
    {
        m_rows += 1;
        m_totalRows += 1;
        //Afegim tants widgets com columnes
        for(column = 0; column < m_totalColumns; column++)
        {
            newViewer = getNewQ2DViewerWidget();
            m_viewersLayout->addWidget( newViewer, m_rows-1, column);
            m_vectorViewers.push_back( newViewer );
            if( column >= m_columns)
            {
                newViewer->hide();
                emit viewerRemoved( newViewer );
            }
        }
        rows--;
    }
}

void ViewersLayout::removeColumns( int columns )
{
    int posViewer = m_columns-1;
    Q2DViewerWidget *oldViewer;

    int rows;
    while( columns > 0 && m_columns > 1 )
    {
        rows = 0;
        // Eliminem un widget de cada fila per tenir una columna menys
        while (rows < m_viewersLayout->rowCount() )
        {
            oldViewer = m_vectorViewers.value(posViewer);
            m_viewersLayout->removeWidget( oldViewer );
            m_vectorViewers.remove( posViewer );
            if ( m_selectedViewer == oldViewer )
                setViewerSelected( m_vectorViewers.value( 0 ) );
            delete oldViewer;
            posViewer += (m_columns-1);
            rows++;
        }
        m_columns--;
        posViewer = m_columns-1;
        columns--;
    }
}

void ViewersLayout::removeRows( int rows )
{
    int i;
    int posViewer = m_vectorViewers.count()-1;
    Q2DViewerWidget *oldViewer;

    while( rows > 0 && m_rows > 1 )
    {
        //Eliminem tants widgets com columnes
        for(i = 0; i < m_columns; i++)
        {
            oldViewer = m_vectorViewers.value(posViewer);
            m_vectorViewers.remove(posViewer);
            m_viewersLayout->removeWidget( oldViewer );
            // TODO eliminar els viewers que treiem del toolManager???
            if ( m_selectedViewer == oldViewer )
                setViewerSelected( m_vectorViewers.value( 0 ) );
            delete oldViewer;
            posViewer -= 1;
        }
        m_rows--;
        rows--;
    }
}

void ViewersLayout::setGrid( int rows, int columns )
{

    this->setLayout( NULL );

    // Mirem si les tenim amagades i mostrem totes les necessaries
    int windowsToShow = 0;
    int windowsToCreate = 0;
    int windowsToHide = 0;

    if( rows > m_rows )
    {
        int hideWindows = m_totalRows - m_rows;

        if( hideWindows < (rows - m_rows) )
             windowsToShow = hideWindows;
        else
            windowsToShow = rows-m_rows;

        showRows( windowsToShow );

        if( rows > m_totalRows )
            windowsToCreate = rows - m_totalRows;

        addRows( windowsToCreate );
    }
    else if( rows < m_rows )
    {
        hideRows( m_rows - rows );
    }

    windowsToShow = 0;
    windowsToCreate = 0;
    windowsToHide = 0;

    if( columns > m_columns )
    {
        int hideWindows = m_totalColumns - m_columns;

        if( hideWindows < (columns - m_columns) )
            windowsToShow = hideWindows;
        else
            windowsToShow = columns-m_columns;

        showColumns( windowsToShow );

        if( columns > m_totalColumns )
            windowsToCreate = columns - m_totalColumns;

        addColumns( windowsToCreate );
    }
    else if( columns < m_columns )
    {
        hideColumns( m_columns - columns );
    }
}

void ViewersLayout::setGrid( QList<QString> positionsList )
{
    int i;
    Q2DViewerWidget *newViewer;
    QStringList listOfPositions;
    QString position;
    int numberOfElements = positionsList.size();
    double x1;
    double y1;
    double x2;
    double y2;
    int screen_x = this->width();
    int screen_y = this->height();

    if( m_gridLayout ) removeLayouts();

    for( i = 0; i < numberOfElements; i++ )
    {
        newViewer = m_vectorViewers.value( i );

        if( newViewer == 0)
        {
            newViewer = getNewQ2DViewerWidget();
            m_vectorViewers.push_back( newViewer );
        }
        position = positionsList.value( i );
        listOfPositions = position.split("\\");
        x1 = listOfPositions.value( 0 ).toDouble();
        y1 = listOfPositions.value( 1 ).toDouble();
        x2 = listOfPositions.value( 2 ).toDouble();
        y2 = listOfPositions.value( 3 ).toDouble();
        newViewer->setGeometry( x1*screen_x, (1-y1)*screen_y, ((x2-x1)*screen_x), (y1-y2)*screen_y );

        emit viewerAdded( newViewer );
    }

    DEBUG_LOG( tr("MIDES: \n SCREEN_X: %1 \n SCREEN_Y: %2 \n [THIS.X, THIS.Y]: [%3,%4] \n, WIDTH: %5, HEIGHT: %6").arg(screen_x).arg( screen_y).arg(this->x()).arg(this->y()).arg(this->width()).arg(this->height()) );

    m_positionsList = positionsList;
}

Q2DViewerWidget * ViewersLayout::addViewer( QString position )
{
    int i;
    Q2DViewerWidget *newViewer;
    QStringList listOfPositions;
    double x1;
    double y1;
    double x2;
    double y2;
    int screen_x = this->width();
    int screen_y = this->height();

    if( m_gridLayout ) removeLayouts();

    newViewer = getNewQ2DViewerWidget();
    m_vectorViewers.push_back( newViewer );

    listOfPositions = position.split("\\");
    x1 = listOfPositions.value( 0 ).toDouble();
    y1 = listOfPositions.value( 1 ).toDouble();
    x2 = listOfPositions.value( 2 ).toDouble();
    y2 = listOfPositions.value( 3 ).toDouble();

    newViewer->setGeometry( x1*screen_x, (1-y1)*screen_y, ((x2-x1)*screen_x), (y1-y2)*screen_y );

    emit viewerAdded( newViewer );

    DEBUG_LOG( tr("MIDES: \n SCREEN_X: %1 \n SCREEN_Y: %2 \n [THIS.X, THIS.Y]: [%3,%4] \n, WIDTH: %5, HEIGHT: %6").arg(screen_x).arg( screen_y).arg(this->x()).arg(this->y()).arg(this->width()).arg(this->height()) );

    m_positionsList << position;

    return newViewer;
}

void ViewersLayout::resizeEvent ( QResizeEvent * event )
{
    int i;
    double x1;
    double y1;
    double x2;
    double y2;
    int numberOfElements = m_vectorViewers.size();
    Q2DViewerWidget * viewer;
    QStringList listOfPositions;
    QString position;

    QWidget::resizeEvent(event);

    int screen_x = this->width();
    int screen_y = this->height();

    for( i = 0; i < numberOfElements; i++ )
    {
        viewer = m_vectorViewers.value( i );
        position = m_positionsList.value( i );
        listOfPositions = position.split("\\");
        x1 = listOfPositions.value( 0 ).toDouble();
        y1 = listOfPositions.value( 1 ).toDouble();
        x2 = listOfPositions.value( 2 ).toDouble();
        y2 = listOfPositions.value( 3 ).toDouble();
        viewer->setGeometry( x1*screen_x, (1-y1)*screen_y, (x2-x1)*screen_x, (y1-y2)*screen_y );

        DEBUG_LOG( tr("NOUS VALORS: \n X: %1 \n Y: %2 \n WIDTH: %3, HEIGHT: %4 AMB x1 = %5, y1 = %6, x2 = %7 Y2 = %8 ").arg(x1*screen_x).arg( (1-y1)*screen_y).arg(((x2-x1)*screen_x)).arg((y1-y2)*screen_y).arg(x1).arg(y1).arg(x2).arg(y2) );
    }

     DEBUG_LOG( tr("MIDES al resize: \n SCREEN_X: %1 \n SCREEN_Y: %2 \n [THIS.X, THIS.Y]: [%3,%4] \n, WIDTH: %5, HEIGHT: %6").arg(screen_x).arg( screen_y).arg(this->x()).arg(this->y()).arg(this->width()).arg(this->height()) );

}

void ViewersLayout::setViewerSelected( Q2DViewerWidget *viewer )
{
    if( !viewer )
    {
        DEBUG_LOG("El Viewer donat és NUL!");
        return;
    }

    if ( viewer != m_selectedViewer )
    {
        if( m_selectedViewer )
        {
            m_selectedViewer->setSelected( false );
        }
        m_selectedViewer = viewer;
        m_selectedViewer->setSelected( true );
        emit viewerSelectedChanged( m_selectedViewer );
    }
}

void ViewersLayout::showRows( int rows )
{
    Q2DViewerWidget *viewer;
    int numColumn;

    while( rows > 0 )
    {
        for( numColumn = 0; numColumn < m_columns; numColumn++ )
        {
            viewer = m_vectorViewers.value( ( m_totalColumns*m_rows ) + numColumn );
            viewer->show();
        }
        m_rows++;
        rows--;
    }
}

void ViewersLayout::hideRows( int rows )
{
    Q2DViewerWidget *viewer;
    int numColumn;

    while( rows > 0 )
    {
        m_rows--;
        for( numColumn = 0; numColumn < m_columns; numColumn++ )
        {
            viewer = m_vectorViewers.value( ( ( m_totalColumns*m_rows ) + numColumn ) );
            viewer->hide();
            emit viewerRemoved( viewer );
            if ( m_selectedViewer == viewer )
                setViewerSelected( m_vectorViewers.value( 0 ) );
        }
        rows--;
    }
}

void ViewersLayout::showColumns( int columns )
{
    Q2DViewerWidget *viewer;
    int numRow;

    while( columns > 0 )
    {
        for( numRow = 0; numRow < m_rows; numRow++ )
        {
            viewer = m_vectorViewers.value( ( m_totalColumns*numRow ) + m_columns );
            viewer->show();
        }
        m_columns++;
        columns--;
    }
}

void ViewersLayout::hideColumns( int columns )
{
    Q2DViewerWidget *viewer;
    int numRow;

    while( columns > 0 )
    {
        m_columns--;
        for( numRow = 0; numRow < m_rows; numRow++ )
        {
            viewer = m_vectorViewers.value( ( m_totalColumns*numRow ) + m_columns );
            viewer->hide();
            emit viewerRemoved( viewer );
            if ( m_selectedViewer == viewer )
                setViewerSelected( m_vectorViewers.value( 0 ) );
        }
        columns--;
    }
}

int ViewersLayout::getNumberOfViewers()
{
    return m_vectorViewers.size();
}

Q2DViewerWidget * ViewersLayout::getViewerWidget( int number )
{
    return m_vectorViewers.value( number );
}

}
