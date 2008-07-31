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
#include <QFrame>
#include <QPalette>
#include <QMouseEvent>
#include <QGridLayout>
#include <QLabel>

namespace udg {

MenuGridWidget::MenuGridWidget( QWidget *parent )
 : QWidget( parent )
{
    m_predefinedGrids = new QWidget( this );
    m_predefinedGrids->setGeometry(QRect(0, 10, 271, 181));

    setWindowFlags(Qt::Popup);
    m_maxColumns = 4;
    m_maxElements = 16;

    m_gridLayout = new QGridLayout( m_predefinedGrids );
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

    dropContent();

    if( listPredefinedGridsList.size() >= m_maxColumns ) width = 70 * m_maxColumns;
    else
    {
        width = 70 * listPredefinedGridsList.size();
    }

    height = 100 *  ( ceil ( listPredefinedGridsList.size()/( m_maxColumns*1.0 ) ) );

    m_predefinedGrids->resize( width, height );
    this->resize( width, height+10 );

    for( numberPredefined = 0; numberPredefined < listPredefinedGridsList.size(); numberPredefined++ )
    {
        values = listPredefinedGridsList.at( numberPredefined ).split( "x" );
        rows = values.value( 0 ).toInt();
        columns = values.value( 1 ).toInt();
        icon = createIcon( rows, columns );
        m_gridLayout->addWidget( icon, positionRow, positionColumn );
        m_itemList->push_back( icon );
        positionColumn ++;

        if( positionColumn == m_maxColumns )
        {
            positionColumn = 0;
            positionRow ++;
        }
    }
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

ItemMenu * MenuGridWidget::createIcon( int rows, int columns )
{
    ItemMenu * icon = new ItemMenu( this );
    icon->setData( new QString( tr( "%1,%2" ).arg( rows ).arg( columns ) ) );
    icon->setGeometry ( 0, 0, 32, 32 );
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

    QStringList values = ( selected->getData() )->split( "," );
    int rows = values.value( 0 ).toInt();
    int columns = values.value( 1 ).toInt();

    emit selectedGrid( rows, columns );
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
