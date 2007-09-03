/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "tablemenu.h"
#include "logging.h"

namespace udg {

TableMenu::TableMenu()
 : QWidget()
{
    setWindowFlags(Qt::Popup);
    m_columns = 0;
    m_rows = 0;
    m_itemList = new QList<ItemMenu *>();
    QGridLayout * verticalLayout = new QGridLayout( this );
    m_gridLayout = new QGridLayout( );
    m_information = new QLabel( );
    m_information->setAlignment( Qt::AlignHCenter );
    verticalLayout->addLayout( m_gridLayout,0,0 );
    verticalLayout->addWidget( m_information,1,0 );
    ItemMenu * firstItem = new ItemMenu( this );
    firstItem->setFrameShape( QFrame::StyledPanel );
    firstItem->setMinimumSize( 30, 30 );
    firstItem->setMaximumSize( 30, 30 );
    firstItem->setData( ( QVariant * ) new QString( tr("%1,%2").arg( m_rows ).arg( m_columns ) ) );
    firstItem->setFixed( true );
    m_itemList->insert( 0, firstItem );
    m_gridLayout->addWidget( firstItem, m_columns, m_rows );
    m_oldSelected = firstItem;
    m_information->setText("1x1");
    connect( firstItem , SIGNAL( isActive( ItemMenu * ) ) , this , SLOT( verifySelected( ItemMenu * ) ) );
    connect( firstItem , SIGNAL( isSelected( ItemMenu * ) ) , this , SLOT( emitSelected( ItemMenu * ) ) );
}

TableMenu::~TableMenu()
{
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
        newItem->setData( (QVariant *) new QString( tr("%1,%2").arg( numRow ).arg( m_columns ) ) );
        newItem->setMinimumSize( 30, 30 );
        newItem->setMaximumSize( 30, 30 );
        newItem->setFixed( true );
        m_gridLayout->addWidget( newItem, numRow, m_columns );
        m_itemList->insert( ((m_columns+1)*numRow + m_columns), newItem );
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
        newItem->setData( (QVariant * ) new QString( tr("%1,%2").arg( m_rows ).arg( numColumn ) ) );
        newItem->setMinimumSize( 30, 30 );
        newItem->setMaximumSize( 30, 30 );
        newItem->setFixed( true );
        m_gridLayout->addWidget( newItem, m_rows, numColumn );
        m_itemList->insert( (m_rows*(m_columns+1) + numColumn), newItem );
        connect( newItem , SIGNAL( isActive( ItemMenu * ) ) , this , SLOT( verifySelected( ItemMenu * ) ) );
        connect( newItem , SIGNAL( isSelected( ItemMenu * ) ) , this , SLOT( emitSelected( ItemMenu * ) ) );
    }
}

void TableMenu::verifySelected( ItemMenu * selected )
{

    ItemMenu * item;

    QStringList values = ((QString *) selected->getData())->split( "," );
    int rows = values.value( 0 ).toInt();
    int columns = values.value( 1 ).toInt();

    QStringList oldValues = ((QString *) m_oldSelected->getData())->split( "," );
    int oldRows = oldValues.value( 0 ).toInt();
    int oldColumns = oldValues.value( 1 ).toInt();

    if( rows == m_rows ) addRow(); // Hem d'afegir una fila a la graella

    if( oldRows < rows )
    {
        // Marcar com a seleccionades les anteriors
        int numColumn;
        for( numColumn = 0; numColumn < columns; numColumn++ )
        {
            item = m_itemList->value( rows*(m_columns+1) + numColumn );
            item->setSelected( true );
        }
    }
    else if( oldRows > rows )// Hem de desmarcar una fila d'elements
    {
        int numColumn;
        for( numColumn = 0; numColumn < m_columns; numColumn++ )
        {
            item = m_itemList->value( oldRows*(m_columns+1) + numColumn );
            item->setSelected( false );
        }
    }

    if( columns == m_columns ) addColumn(); // Hem d'afegir una columna a la graella

    if( oldColumns < columns )
    {
        // Marcar com a seleccionades les anteriors
        int numRow;
        for( numRow = 0; numRow < rows; numRow++ )
        {
            item = m_itemList->value( numRow*(m_columns+1) + columns );
            item->setSelected( true );
        }
    }
    else if( oldColumns > columns )// Hem de desmarcar una columna d'elements
    {
        int numRow;
        for( numRow = 0; numRow < m_rows; numRow++ )
        {
            item = m_itemList->value( numRow*(m_columns+1) + oldColumns );
            item->setSelected( false );
        }
    }

    m_information->setText( QString( tr("%1x%2").arg(columns+1).arg(rows+1) ) );
    m_oldSelected = selected;
}


bool TableMenu::event( QEvent * event )
{
    if ( event->type() == QEvent::Leave )
    {
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

    QStringList values = ((QString *) selected->getData())->split( "," );
    int rows = values.value( 0 ).toInt();
    int columns = values.value( 1 ).toInt();

    emit selectedGrid( rows+1, columns+1 );
    hide();

}

}