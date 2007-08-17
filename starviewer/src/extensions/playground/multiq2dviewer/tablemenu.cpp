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
    m_gridLayout = new QGridLayout( this );
    ItemMenu * firstItem = new ItemMenu( this );
    firstItem->setFrameShape( QFrame::StyledPanel );
    firstItem->setMinimumSize( 30, 30 );
    firstItem->setMaximumSize( 30, 30 );
    firstItem->setData( (QVariant *) new QString( tr("%1,%2").arg( m_rows ).arg( m_columns ) ) );
    m_gridLayout->addWidget( firstItem, m_columns, m_rows );

    connect( firstItem , SIGNAL( isActive( ItemMenu * ) ) , this , SLOT( verifySelected( ItemMenu * ) ) );
}

TableMenu::~TableMenu()
{
}

void TableMenu::addColumn()
{
    ItemMenu * newItem;
    int numRow = 0;

    m_columns++;
    
    for( numRow = 0; numRow < m_rows; numRow++ )
    {
        newItem = new ItemMenu( this );
        newItem->setFrameShape( QFrame::StyledPanel );
        newItem->setData( (QVariant *) new QString( tr("%1,%2").arg( numRow ).arg( m_columns ) ) );
        newItem->setMinimumSize( 30, 30 );
        newItem->setMaximumSize( 30, 30 );
        m_gridLayout->addWidget( newItem, numRow, m_columns );
        connect( newItem , SIGNAL( isActive( ItemMenu * ) ) , this , SLOT( verifySelected( ItemMenu * ) ) );
    }
}

void TableMenu::addRow()
{
    ItemMenu * newItem;
    int numColumn = 0;

    m_rows++;
    
    for( numColumn = 0; numColumn < m_columns; numColumn++ )
    {
        newItem = new ItemMenu( this );
        newItem->setFrameShape( QFrame::StyledPanel );
        newItem->setData( (QVariant * ) new QString( tr("%1,%2").arg( m_rows ).arg( numColumn ) ) );
        newItem->setMinimumSize( 30, 30 );
        newItem->setMaximumSize( 30, 30 );
        m_gridLayout->addWidget( newItem, m_rows, numColumn );
        connect( newItem , SIGNAL( isActive( ItemMenu * ) ) , this , SLOT( verifySelected( ItemMenu * ) ) );
    }
}

void TableMenu::verifySelected( ItemMenu * selected )
{

    QStringList values = ((QString *) selected->getData())->split( "," );

    int rows = values.value( 0 ).toInt();
    int columns = values.value( 1 ).toInt();

    DEBUG_LOG( QString( tr("Elem seleccionat %1,%2.").arg(rows).arg(columns) ) );

    if( columns == m_columns ) addColumn();

    if( rows == m_rows ) addRow();
    
}

}
