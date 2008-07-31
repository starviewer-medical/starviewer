/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "widgetlayoutviewer.h"

#include "logging.h"

namespace udg {

WidgetLayoutViewer::WidgetLayoutViewer(QWidget *parent) : QWidget(parent), m_selectedViewer(0)
{
    this->setViewerSelected( this->getNewQ2DViewerWidget() );
    initLayouts();
}

WidgetLayoutViewer::~WidgetLayoutViewer()
{

}

void WidgetLayoutViewer::initLayouts()
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

    m_vectorViewers.push_back( m_selectedViewer );
    this->setLayout(m_gridLayout);
    m_gridLayout->update();

    m_selectedViewer->setSelected( true );
}

Q2DViewerWidget* WidgetLayoutViewer::getViewerSelected()
{
    return m_selectedViewer;
}


Q2DViewerWidget* WidgetLayoutViewer::getNewQ2DViewerWidget()
{
    Q2DViewerWidget *newViewer = new Q2DViewerWidget( this );
    connect( newViewer, SIGNAL( selected( Q2DViewerWidget *) ), SLOT( setViewerSelected( Q2DViewerWidget *) ) );
    // per defecte no li posem cap annotació
    newViewer->getViewer()->removeAnnotation( Q2DViewer::AllAnnotation );

    emit viewerAdded( newViewer );
    return newViewer;
}

void WidgetLayoutViewer::addColumns( int columns )
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

void WidgetLayoutViewer::addRows( int rows )
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

void WidgetLayoutViewer::removeColumns( int columns )
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

void WidgetLayoutViewer::removeRows( int rows )
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

void WidgetLayoutViewer::setGrid( int rows, int columns )
{

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

void WidgetLayoutViewer::setViewerSelected( Q2DViewerWidget *viewer )
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

void WidgetLayoutViewer::showRows( int rows )
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

void WidgetLayoutViewer::hideRows( int rows )
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

void WidgetLayoutViewer::showColumns( int columns )
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

void WidgetLayoutViewer::hideColumns( int columns )
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

int WidgetLayoutViewer::getNumberOfViewers()
{
    return m_vectorViewers.size();
}

Q2DViewerWidget * WidgetLayoutViewer::getViewerWidget( int number )
{
    return m_vectorViewers.value( number );
}

}
