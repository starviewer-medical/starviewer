/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "tablemenu.h"
#include "logging.h"
#include "itemmenu.h"
#include <QGridLayout>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>

namespace udg {

TableMenu::TableMenu()
 : QWidget()
{
    setWindowFlags(Qt::Popup);

    m_itemList = new QList<ItemMenu *>();
    QGridLayout * verticalLayout = new QGridLayout( this );
    m_gridLayout = new QGridLayout( );
    m_information = new QLabel( );
    m_information->setAlignment( Qt::AlignHCenter );
    verticalLayout->addLayout( m_gridLayout,0,0 );
    verticalLayout->addWidget( m_information,1,0 );

    initializeTable();

    setMouseTracking( true );
}

TableMenu::~TableMenu()
{
}

void TableMenu::initializeTable()
{
    m_columns = 0;
    m_rows = 0;

    ItemMenu * firstItem = new ItemMenu( this );
    firstItem->setFrameShape( QFrame::StyledPanel );
    firstItem->setMinimumSize( 30, 30 );
    firstItem->setMaximumSize( 30, 30 );
    firstItem->setData( new QString( tr("%1,%2").arg( m_rows ).arg( m_columns ) ) );
    firstItem->setFixed( true );
    m_itemList->insert( 0, firstItem );
    m_gridLayout->addWidget( firstItem, m_columns, m_rows );
    connect( firstItem , SIGNAL( isActive( ItemMenu * ) ) , this , SLOT( verifySelected( ItemMenu * ) ) );
    connect( firstItem , SIGNAL( isSelected( ItemMenu * ) ) , this , SLOT( emitSelected( ItemMenu * ) ) );

    m_information->setText("1x1");

    addColumn();
    addColumn();
    addColumn();
    addRow();
    addRow();
}

void TableMenu::addColumn()
{
    ItemMenu * newItem;
    int numRow = 0;

    m_columns++;

    for( numRow = 0; numRow <= m_rows; numRow++ )
    {
        newItem = new ItemMenu( this );
        newItem->setFrameShape( QFrame::StyledPanel );
        newItem->setData( new QString( tr("%1,%2").arg( numRow ).arg( m_columns ) ) );
        newItem->setMinimumSize( 30, 30 );
        newItem->setMaximumSize( 30, 30 );
        newItem->setFixed( true );
        m_gridLayout->addWidget( newItem, numRow, m_columns );
        m_itemList->insert( ((m_columns+1)*numRow + m_columns), newItem );
        newItem->show();//TODO Necessari al passar a qt 4.3
        connect( newItem , SIGNAL( isActive( ItemMenu * ) ) , this , SLOT( verifySelected( ItemMenu * ) ) );
        connect( newItem , SIGNAL( isSelected( ItemMenu * ) ) , this , SLOT( emitSelected( ItemMenu * ) ) );
    }
}

void TableMenu::addRow()
{
    ItemMenu * newItem;
    int numColumn = 0;

    m_rows++;

    for( numColumn = 0; numColumn <= m_columns; numColumn++ )
    {
        newItem = new ItemMenu( this );
        newItem->setFrameShape( QFrame::StyledPanel );
        newItem->setData( new QString( tr("%1,%2").arg( m_rows ).arg( numColumn ) ) );
        newItem->setMinimumSize( 30, 30 );
        newItem->setMaximumSize( 30, 30 );
        newItem->setFixed( true );
        m_gridLayout->addWidget( newItem, m_rows, numColumn );
        m_itemList->insert( (m_rows*(m_columns+1) + numColumn), newItem );
        newItem->show();//TODO Necessari al passar a qt 4.3
        connect( newItem , SIGNAL( isActive( ItemMenu * ) ) , this , SLOT( verifySelected( ItemMenu * ) ) );
        connect( newItem , SIGNAL( isSelected( ItemMenu * ) ) , this , SLOT( emitSelected( ItemMenu * ) ) );
    }
}

void TableMenu::verifySelected( ItemMenu * selected )
{
    ItemMenu * item;
    int numRow;
    int numColumn;

    QStringList values = ( selected->getData())->split( "," );
    int rows = values.value( 0 ).toInt();
    int columns = values.value( 1 ).toInt();

    if( rows == m_rows ) addRow(); // Hem d'afegir una fila a la graella
    if( columns == m_columns ) addColumn(); // Hem d'afegir una columna a la graella

    // Marquem i desmarquem les caselles segons la seleccio actual
    for( numRow = 0; numRow < m_rows; numRow++ )
    {
        for( numColumn = 0; numColumn < m_columns; numColumn++ )
        {
            item = m_itemList->value( numRow*(m_columns+1) + numColumn );

            if( numRow <= rows && numColumn <= columns )
            {
                item->setSelected( true );
            }
            else
            {
                item->setSelected( false );
            }
        }
    }
    m_information->setText( QString( tr("%1x%2").arg(columns+1).arg(rows+1) ) );
}


bool TableMenu::event( QEvent * event )
{
    if ( event->type() == QEvent::Leave )
    {
        dropTable();
        initializeTable();
        hide();
        return true;
    }
    else
    {
        return QWidget::event( event );
    }
}

void TableMenu::emitSelected( ItemMenu * selected )
{

    QStringList values = ( selected->getData())->split( "," );
    int rows = values.value( 0 ).toInt();
    int columns = values.value( 1 ).toInt();

    emit selectedGrid( rows+1, columns+1 );

    dropTable();
    initializeTable();
    hide();
}

void TableMenu::dropTable()
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

void TableMenu::mouseMoveEvent ( QMouseEvent * event )
{
    if( event->x() >= ( this->width() - 20 ) )
    {
        addColumn();
    }
}

}
