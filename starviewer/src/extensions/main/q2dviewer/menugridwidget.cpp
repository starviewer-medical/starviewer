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
#include "hangingprotocolsrepository.h"
#include "hangingprotocol.h"
#include "hangingprotocoldisplayset.h"
#include <QFrame>
#include <QPalette>
#include <QMouseEvent>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QSpacerItem>

namespace udg {

MenuGridWidget::MenuGridWidget( QWidget *parent )
 : QWidget( parent )
{
    setWindowFlags(Qt::Popup);
    m_maxColumns = 5;
    m_maxElements = 16;

    m_gridLayout = new QGridLayout( this );
    m_gridLayout->setSpacing( 6 );
    m_gridLayout->setMargin( 6 );

    m_itemList = new QList<ItemMenu *>();

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

    m_predefinedGridBox = new QGroupBox( this );
    m_predefinedGridBox->setAlignment(Qt::AlignRight);
    m_predefinedGridBox->setTitle( "Predefined grids " );
    m_gridLayout->addWidget( m_predefinedGridBox, 0, 0, 1, 1 );
    QGridLayout * gridLayoutPredefined = new QGridLayout( m_predefinedGridBox );
    QSpacerItem * spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayoutPredefined->addItem(spacerItem, 0, m_maxColumns, 1, 1);

    dropContent();

	int numberOfHangingProtocols = HangingProtocolsRepository::getRepository()->getNumberOfItems();
    int numberOfItems = listPredefinedGridsList.size() + numberOfHangingProtocols;

	if( numberOfItems >= m_maxColumns ) width = 70 * m_maxColumns + ( m_gridLayout->margin()*2 );
    else
    {
        width = 70 * numberOfItems + ( m_gridLayout->margin()*2 );
    }

	height = 86 *  ( ceil ( numberOfItems/(m_maxColumns*1.0 ) ) );

    m_predefinedGridBox->resize( width, height );
    this->resize( width+6, height+6 );

    for( numberPredefined = 0; numberPredefined < listPredefinedGridsList.size(); numberPredefined++ )
    {
        values = listPredefinedGridsList.at( numberPredefined ).split( "x" );
        rows = values.value( 0 ).toInt();
        columns = values.value( 1 ).toInt();
        icon = createIcon( rows, columns );
        gridLayoutPredefined->addWidget( icon, positionRow, positionColumn );
        m_itemList->push_back( icon );
        positionColumn ++;

        if( positionColumn == m_maxColumns )
        {
            positionColumn = 0;
            positionRow++;
        }
    }

    // Creació de hanging protocols
	int hangingProtocolNumber;
	Identifier id;
	HangingProtocol * hangingProtocol;
    m_hangingProtocolBox = new QGroupBox( this );
    m_hangingProtocolBox->setTitle( "Hanging Protocols");
    m_hangingProtocolBox->setAlignment(Qt::AlignRight);
    m_gridLayout->addWidget( m_hangingProtocolBox, 1, 0, 1, 1 );
    QGridLayout * gridLayoutHanging = new QGridLayout( m_hangingProtocolBox );
    positionRow = 0;
    positionColumn = 0;

	for( hangingProtocolNumber = 0; hangingProtocolNumber < numberOfHangingProtocols; hangingProtocolNumber++)
	{	
		id.setValue( hangingProtocolNumber );
        hangingProtocol = HangingProtocolsRepository::getRepository()->getItem( id );
		icon = createIcon( hangingProtocol, hangingProtocolNumber );
		
		gridLayoutHanging->addWidget( icon, positionRow, positionColumn );
        m_itemList->push_back( icon );
        positionColumn ++;

        if( positionColumn == m_maxColumns )
        {
            positionColumn = 0;
            positionRow++;
        }
	}
    if ( numberOfHangingProtocols == 0 ) 
           m_hangingProtocolBox->hide();
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

ItemMenu * MenuGridWidget::createIcon( HangingProtocol * hangingProtocol, int hangingProtocolNumber )
{
    HangingProtocolDisplaySet * displaySet;
	int displaySetNumber;
	ItemMenu * icon = new ItemMenu( this );
	icon->setData( QString( tr( "%1" ).arg( hangingProtocolNumber ) ) );
	QStringList listOfPositions;
	double x1;
	double x2;
	double y1;
	double y2;
	GridIcon* newIcon;

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
		newIcon = new GridIcon( icon );

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

    QGridLayout * gridLayout = new QGridLayout();
    gridLayout->setSpacing( 0 );
    gridLayout->setMargin( 2 );

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
            gridLayout->addWidget( newIcon, numberRows, numberColumns );
        }
    }

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

    for( i = 0; i < m_itemList->size(); i++ )
    {
        item = m_itemList->value( i );
        m_gridLayout->removeWidget( item );
        delete item;
    }
    m_itemList->clear();
}


}
