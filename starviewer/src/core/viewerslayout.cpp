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
	
	m_numberOfVisibleViewers = 0;

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

    m_isRegular = true;

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

void ViewersLayout::restoreLayouts()
{
	int i;
	int numberOfViewers;
	int column = 1;
	int row = 0;

	numberOfViewers = m_vectorViewers.size();

	/// Creem els que calguin per ser regular si es que no hi havia cap grid predefinit
	if( (m_totalColumns == 1) && (m_totalRows == 1) && numberOfViewers > 1)
	{
		m_totalColumns = ceil( sqrt( (double)numberOfViewers) );
		m_totalRows = m_totalColumns;
		
		if( (m_totalRows * m_totalColumns) < numberOfViewers )
		{
			for( i = 0; i < (numberOfViewers - (m_totalRows * m_totalColumns) ); i++ )
			{
				m_vectorViewers.push_back( getNewQ2DViewerWidget() );
			}
		}
	}

	m_viewersLayout->addWidget( m_vectorViewers.value(0), 0, 0 );

	// S'amaguen tots i es deixa el principal, es tornen a posar amb layout
	for( i = 1; i < m_vectorViewers.size(); i++)
	{
		m_vectorViewers.value(i)->hide();
		m_viewersLayout->addWidget( m_vectorViewers.value(i), row, column );

		column++;
		if( column >= m_totalColumns)
		{
			row++;
			column = 0;
		}
	}

	setViewerSelected ( m_vectorViewers.value( 0 ) );
	m_rows = 1;
	m_columns = 1;
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

    // Mirem si les tenim amagades i mostrem totes les necessaries
    int windowsToShow = 0;
    int windowsToCreate = 0;
    int windowsToHide = 0;
	
	if( !m_isRegular )
	{
		restoreLayouts();
	}

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
    m_isRegular = true;
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

	m_numberOfVisibleViewers = 0;

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

		m_numberOfVisibleViewers++;

        emit viewerAdded( newViewer );
    }

    m_positionsList = positionsList;
    m_isRegular = false;
}

Q2DViewerWidget * ViewersLayout::addViewer( QString position )
{

    Q2DViewerWidget *newViewer;
    QStringList listOfPositions;
    double x1;
    double y1;
    double x2;
    double y2;
    int screen_x = this->width();
    int screen_y = this->height();

	m_rows = 0;
    m_columns = 0;
    if( m_gridLayout ) removeLayouts();

	if ( m_numberOfVisibleViewers < m_vectorViewers.size() )
	{
		newViewer = m_vectorViewers.value( m_numberOfVisibleViewers );
	}
	else
	{
		newViewer = getNewQ2DViewerWidget();
		m_vectorViewers.push_back( newViewer );
	}
	
    listOfPositions = position.split("\\");
    x1 = listOfPositions.value( 0 ).toDouble();
    y1 = listOfPositions.value( 1 ).toDouble();
    x2 = listOfPositions.value( 2 ).toDouble();
    y2 = listOfPositions.value( 3 ).toDouble();
    newViewer->setGeometry( x1*screen_x, (1-y1)*screen_y, ((x2-x1)*screen_x), (y1-y2)*screen_y );

	m_numberOfVisibleViewers++;

    emit viewerAdded( newViewer );
    
    m_positionsList << position;
    m_isRegular = false;

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

    if( !m_isRegular )
    {
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
        }
    }
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
