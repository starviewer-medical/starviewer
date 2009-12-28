/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "menugridwidget.h"

#include "gridicon.h"
#include "itemmenu.h"
#include "logging.h"
#include "math.h"
#include "hangingprotocol.h"
#include "hangingprotocoldisplayset.h"
#include <QFrame>
#include <QPalette>
#include <QMouseEvent>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QSpacerItem>
#include <QMovie>
#include <QLabel>

namespace udg {

MenuGridWidget::MenuGridWidget( QWidget *parent )
 : QWidget( parent ), m_searchingWidget(0)
{
    setWindowFlags(Qt::Popup);
    m_maxColumns = 5;
    m_maxElements = 16;

    m_gridLayout = new QGridLayout( this );
    m_gridLayout->setSpacing( 6 );
    m_gridLayout->setMargin( 6 );

    m_nextHangingProtocolRow = 0;
    m_nextHangingProtocolColumn = 0;
    m_gridLayoutHanging = 0;

    m_loadingIsShowed = false;
    // Creem el widget amb l'animació de "searching"
    createSearchingWidget();
//     m_predefinedGridsList << "1x1" << "1x2" << "2x2" << "2x3" << "3x3" << "3x4" << "4x4" << "4x5";
//     createPredefinedGrids( m_predefinedGridsList );

}

MenuGridWidget::~MenuGridWidget()
{
}

void MenuGridWidget::createPredefinedGrids( QStringList listPredefinedGridsList )
{
    int width;
    int height;
    int rows;
    int columns;
    int numberPredefined;
    QStringList values;
    ItemMenu * icon;
    int positionRow = 0;
    int positionColumn = 0;

    m_predefinedGridWidget = new QWidget( this );
    QGridLayout * gridLayoutPredefined = new QGridLayout();
    gridLayoutPredefined->setSpacing( 6 );
    gridLayoutPredefined->setMargin( 6 );
    QSpacerItem * spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum); 
 	gridLayoutPredefined->addItem(spacerItem, 0, m_maxColumns, 1, 1);

    QFrame * line_predefined = new QFrame(this);
    line_predefined->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    line_predefined->setFrameShape(QFrame::HLine);
    line_predefined->setFrameShadow(QFrame::Sunken);
    QLabel * label_predefined = new QLabel(this);
    label_predefined->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    label_predefined->setText("Grid layouts");
    QHBoxLayout * hboxLayout_predefined = new QHBoxLayout();
    hboxLayout_predefined->setMargin( 0 );
    hboxLayout_predefined->setSpacing( 6 );
    hboxLayout_predefined->addWidget(line_predefined);
    hboxLayout_predefined->addWidget(label_predefined);
    
    m_gridLayout->addLayout( hboxLayout_predefined, 0, 0, 1, 1 );
    m_gridLayout->addLayout( gridLayoutPredefined, 1, 0, 1, 1);
    dropContent();

	int numberOfHangingProtocols = m_hangingItems.size();
    int numberOfPredefinedItems = listPredefinedGridsList.size();

	if( numberOfPredefinedItems >= m_maxColumns )
	{
		width = 70 * m_maxColumns + ( m_gridLayout->margin()*2 );
	}
    else
    {
        width = 70 * numberOfPredefinedItems + ( m_gridLayout->margin()*2 );
    }

	height = 86 *  ( ceil ( numberOfPredefinedItems/(m_maxColumns*1.0 ) ) );

    m_predefinedGridWidget->resize( width, height );
	this->resize( width+6, height+6 );

    for( numberPredefined = 0; numberPredefined < listPredefinedGridsList.size(); numberPredefined++ )
    {
        values = listPredefinedGridsList.at( numberPredefined ).split( "x" );
        rows = values.value( 0 ).toInt();
        columns = values.value( 1 ).toInt();
        icon = createIcon( rows, columns );
        gridLayoutPredefined->addWidget( icon, positionRow, positionColumn );
        m_itemList.push_back( icon );
        positionColumn ++;

        if( positionColumn == m_maxColumns )
        {
            positionColumn = 0;
            positionRow++;
        }
    }

    // Creació de menu per hanging protocols
	if( numberOfHangingProtocols > 0 )
	{
		int hangingProtocolNumber;
		HangingProtocol * hangingProtocol;
		positionRow = 0;
		positionColumn = 0;

		m_hangingProtocolWidget = new QWidget( this );
		m_gridLayoutHanging = new QGridLayout();
		m_gridLayoutHanging->setSpacing( 6 );
		m_gridLayoutHanging->setMargin( 6 );
		QSpacerItem * spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum); 
		m_gridLayoutHanging->addItem(spacerItem2, 0, m_maxColumns, 1, 1);

		QFrame * line_hanging = new QFrame(this);
		line_hanging->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
		line_hanging->setFrameShape(QFrame::HLine);
		line_hanging->setFrameShadow(QFrame::Sunken);
		QLabel * label_hanging = new QLabel(this);
		label_hanging->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
		label_hanging->setText("Hanging protocols");
		QHBoxLayout * hboxLayout_hanging = new QHBoxLayout();
		hboxLayout_hanging->setMargin( 0 );
		hboxLayout_hanging->setSpacing( 6 );
		hboxLayout_hanging->addWidget(line_hanging);
		hboxLayout_hanging->addWidget(label_hanging);
	    
		m_gridLayout->addLayout( hboxLayout_hanging, 2, 0, 1, 1 );
		m_gridLayout->addLayout( m_gridLayoutHanging, 3, 0, 1, 1);

		for( hangingProtocolNumber = 0; hangingProtocolNumber < numberOfHangingProtocols; hangingProtocolNumber++)
		{	
			hangingProtocol = m_hangingItems.value( hangingProtocolNumber );
			icon = createIcon( hangingProtocol );
			
			m_gridLayoutHanging->addWidget( icon, positionRow, positionColumn );
			m_itemList.push_back( icon );
			positionColumn ++;

			if( positionColumn == m_maxColumns )
			{
				positionColumn = 0;
				positionRow++;
			}
		}
	}
    m_nextHangingProtocolRow = positionRow;
    m_nextHangingProtocolColumn = positionColumn;

    if( m_putLoadingItem )
        addSearchingItem();
}

void MenuGridWidget::createPredefinedGrids( int numSeries )
{
    int row = 1;
    int column = 1;
    bool opt = true;
    int numberElements = 1;

    m_predefinedGridsList.clear();

    while( row*column < numSeries && numberElements < m_maxElements )
    {
        m_predefinedGridsList << QString( tr( "%1x%2" ).arg( row ).arg( column ) );

        // Primer augmentem una columna i despres una fila
        if( opt )
        {
            column++;
            opt = false;
        }
        else
        {
            row++;
            opt = true;
        }
        numberElements++;
    }
    // Afegim la ultima opcio on algun widget pot quedar buit
    m_predefinedGridsList << QString( tr( "%1x%2" ).arg( row ).arg( column ) );

    createPredefinedGrids( m_predefinedGridsList );
}

ItemMenu * MenuGridWidget::createIcon( const HangingProtocol * hangingProtocol )
{
    HangingProtocolDisplaySet * displaySet;
	int displaySetNumber;
	ItemMenu * icon = new ItemMenu( this );
	icon->setData( QString( tr( "%1" ).arg( hangingProtocol->getIdentifier() ) ) );
	QStringList listOfPositions;
	double x1;
	double x2;
	double y1;
	double y2;
	GridIcon* newIcon;
    QString iconType;

    icon->setGeometry( 0, 0, 64, 80 ); 
	icon->setMaximumWidth( 64 );
	icon->setMinimumWidth( 64 );
	icon->setMinimumHeight( 80 );
	icon->setMaximumHeight( 80 );
    icon->setSizePolicy( QSizePolicy( QSizePolicy::Fixed,QSizePolicy::Fixed ) );

	QLabel * sizeText = new QLabel( icon );
    sizeText->setText( hangingProtocol->getName() );
    sizeText->setAlignment( Qt::AlignHCenter );
    sizeText->setGeometry( 0, 64, 64, 80 );

	for( displaySetNumber = 1; displaySetNumber <= hangingProtocol->getNumberOfDisplaySets(); displaySetNumber++ )
	{
		displaySet = hangingProtocol->getDisplaySet( displaySetNumber );
        iconType = displaySet->getIconType();

        if( iconType.isEmpty() )
            iconType = hangingProtocol->getIconType();

        newIcon = new GridIcon( icon, iconType );

		listOfPositions = displaySet->getPosition().split("\\");
		x1 = listOfPositions.value( 0 ).toDouble();
		y1 = listOfPositions.value( 1 ).toDouble();
		x2 = listOfPositions.value( 2 ).toDouble();
		y2 = listOfPositions.value( 3 ).toDouble();

		newIcon->setGeometry( x1*64, (1-y1)*64, ((x2-x1)*64), (y1-y2)*64 );
		newIcon->show();
	}

	icon->show();
    connect( icon , SIGNAL( isSelected( ItemMenu * ) ) , this , SLOT( emitSelected( ItemMenu * ) ) );
    return icon;
}

ItemMenu * MenuGridWidget::createIcon( int rows, int columns )
{
    ItemMenu * icon = new ItemMenu( this );
    icon->setData( QString( tr( "%1,%2" ).arg( rows ).arg( columns ) ) );
    icon->setGeometry ( 0, 0, 64, 80 );
	icon->setMaximumWidth( 64 );
	icon->setMinimumWidth( 64 );
	icon->setMinimumHeight( 80 );
	icon->setMaximumHeight( 80 );
    icon->setSizePolicy( QSizePolicy( QSizePolicy::Fixed,QSizePolicy::Fixed ) );

    int numberRows;
    int numberColumns;
    GridIcon* newIcon;

    QLabel * sizeText = new QLabel();
    sizeText->setText( QString( tr( "%1x%2" ).arg( columns ).arg( rows ) ) );
    sizeText->setAlignment( Qt::AlignHCenter );
    sizeText->setGeometry( 0, 64, 64, 80 );

    QGridLayout * gridLayout = new QGridLayout();
    gridLayout->setSpacing( 0 );
    gridLayout->setMargin( 0 );

    QGridLayout * verticalLayout = new QGridLayout( icon );
    verticalLayout->setSpacing( 0 );
    verticalLayout->setMargin( 0 );
    verticalLayout->addLayout( gridLayout,0,0 );
    verticalLayout->addWidget( sizeText,1,0 );

    for( numberRows = 0; numberRows < rows; numberRows++ )
    {
        for( numberColumns = 0; numberColumns < columns; numberColumns++ )
        {
            newIcon = new GridIcon( icon );
            newIcon->show();
            gridLayout->addWidget( newIcon, numberRows, numberColumns );
        }
    }

    icon->show();
    connect( icon , SIGNAL( isSelected( ItemMenu * ) ) , this , SLOT( emitSelected( ItemMenu * ) ) );
    return icon;
}

void MenuGridWidget::setMaxColumns( int columns )
{
    m_maxColumns = columns;
}

void MenuGridWidget::setMaxElements( int elements )
{
    m_maxElements = elements;
}

void MenuGridWidget::emitSelected( ItemMenu * selected )
{

	if( selected->getData().contains(",") )// És un grid regular
	{
		QStringList values = ( selected->getData() ).split( "," );
		int rows = values.value( 0 ).toInt();
		int columns = values.value( 1 ).toInt();

		emit selectedGrid( rows, columns );
	}
	else // És un grid de hanging protocol
	{
		int i;
		i = selected->getData().toInt();
		emit selectedGrid( selected->getData().toInt() );
	}
    hide();

}

void MenuGridWidget::dropContent()
{
    int i;
    ItemMenu * item;

    for( i = 0; i < m_itemList.size(); i++ )
    {
        item = m_itemList.value( i );
        m_gridLayout->removeWidget( item );
        delete item;
    }
    m_itemList.clear();
}

void MenuGridWidget::setHangingItems( QList<HangingProtocol *> listOfCandidates )
{
	m_hangingItems.clear();
	m_hangingItems = listOfCandidates;
}

void MenuGridWidget::addHangingItems( QList<HangingProtocol *> items )
{
	m_hangingItems.append( items );
}

void MenuGridWidget::setSearchingItem( bool state )
{
    m_putLoadingItem = state;

	if( state == false )
	{
		if( m_gridLayoutHanging != 0 )
		{
            m_searchingWidget->setVisible( false );
			m_gridLayoutHanging->removeWidget( m_searchingWidget );
            m_loadingIsShowed = false;
		}
	}
}

void MenuGridWidget::addSearchingItem()
{
    // S'assumeix que el widget ha d'estar creat
    Q_ASSERT( m_searchingWidget );
    
    if( m_loadingIsShowed || (m_gridLayoutHanging == 0) )
        return;
        
    // Afegim el widget dins del layout del menú i el fem visible
    m_gridLayoutHanging->addWidget( m_searchingWidget, m_nextHangingProtocolColumn, m_nextHangingProtocolRow );
    m_searchingWidget->setVisible(true);
	
    m_loadingColumn = m_nextHangingProtocolColumn;
	m_loadingRow = m_nextHangingProtocolRow;
	
    m_loadingIsShowed = true;
}

void MenuGridWidget::createSearchingWidget()
{
    if( !m_searchingWidget )
    {
        m_searchingWidget = new QWidget( this );
        m_searchingWidget->setVisible( false );
        m_searchingWidget->setGeometry ( 0, 0, 64, 64 );
        m_searchingWidget->setMaximumWidth( 64 );
        m_searchingWidget->setMinimumWidth( 64 );
        m_searchingWidget->setMinimumHeight( 64 );
        m_searchingWidget->setMaximumHeight( 64 );
        m_searchingWidget->setSizePolicy( QSizePolicy( QSizePolicy::Fixed,QSizePolicy::Fixed ) );
        QVBoxLayout * verticalLayout = new QVBoxLayout( m_searchingWidget );

        // Construcció del label per l'animació
        QMovie * searchingMovie = new QMovie( m_searchingWidget );
        searchingMovie->setFileName(QString::fromUtf8(":/images/loader.gif"));
        QLabel * searchingLabelMovie = new QLabel( m_searchingWidget );
        searchingLabelMovie->setMovie( searchingMovie );
        searchingLabelMovie->setAlignment( Qt::AlignCenter );

        // Construcció del label pel text
        QLabel * searchingLabelText = new QLabel( m_searchingWidget );
        searchingLabelText->setText( "Searching..." );

        // Es col·loca dins al widget i a la graella per mostrar-ho
        verticalLayout->addWidget(searchingLabelMovie);
        verticalLayout->addWidget(searchingLabelText);

        searchingMovie->start();
    }
}

}
