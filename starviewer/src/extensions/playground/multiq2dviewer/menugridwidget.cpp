/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "menugridwidget.h"

#include "gridicon.h"
#include "QGridLayout"
#include "QFrame"
#include "QPalette"
#include "QMouseEvent"

#include "logging.h"

namespace udg {

MenuGridWidget::MenuGridWidget( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    setWindowFlags(Qt::Popup);

    m_predefinedGridsList << "1x1" << "1x2" << "2x2" << "2x3" << "3x3" << "3x4" << "4x4";
    m_maxColumns = 4;
    createPredefinedGrids( m_predefinedGridsList );

}

MenuGridWidget::~MenuGridWidget()
{
}

void MenuGridWidget::createPredefinedGrids( QStringList listPredefinedGridsList )
{
    int rows;
    int columns;
    int numberPredefined;
    QStringList values;
    ItemMenu * icon;
    int positionRow = 0;
    int positionColumn = 0;

    QGridLayout * gridLayout = new QGridLayout( m_predefinedGrids );
    gridLayout->setSpacing( 6 );
    gridLayout->setMargin( 6 );

    for( numberPredefined = 0; numberPredefined < listPredefinedGridsList.size(); numberPredefined++ )
    {
        values = listPredefinedGridsList.at( numberPredefined ).split( "x" );
        rows = values.value( 0 ).toInt();
        columns = values.value( 1 ).toInt();
        icon = createIcon( rows, columns );
        gridLayout->addWidget( icon, positionRow, positionColumn );

        positionColumn ++;

        if( positionColumn == m_maxColumns )
        {
            positionColumn = 0;
            positionRow ++;
        }

    }
}

ItemMenu * MenuGridWidget::createIcon( int rows, int columns )
{
    ItemMenu * icon = new ItemMenu( this );
    icon->setData( (QVariant *) new QString( tr("%1,%2").arg( rows ).arg( columns ) ) );
    int numberRows;
    int numberColumns;
    GridIcon* newIcon;

    QGridLayout * gridLayout = new QGridLayout( icon );
    gridLayout->setSpacing( 0 );
    gridLayout->setMargin( 0 );

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

void MenuGridWidget::emitSelected( ItemMenu * selected )
{

    QStringList values = ((QString *) selected->getData())->split( "," );
    int rows = values.value( 0 ).toInt();
    int columns = values.value( 1 ).toInt();

    DEBUG_LOG( QString( tr("Graella seleccionada: %1x%2").arg(rows).arg(columns) ) );

    emit selectedGrid( rows, columns );
    hide();

}


}
