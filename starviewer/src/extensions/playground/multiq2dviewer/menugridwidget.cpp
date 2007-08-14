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

namespace udg {

MenuGridWidget::MenuGridWidget( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    setWindowFlags(Qt::Popup);

    m_predefinedGridsList  << "1x1" << "2x2" << "1x2" << "2x3" << "3x3";
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
    return icon;
}

void MenuGridWidget::setMaxColumns( int columns )
{
    m_maxColumns = columns;
}

bool MenuGridWidget::event( QEvent * event )
{

    if ( event->type() == QEvent::MouseButtonPress )
    {
        int rows = 3;
        int columns = 3;

        QWidget * selectedOption = childAt( QPoint( ( (QMouseEvent *)event )->x(), ( (QMouseEvent *)event )->y() ) );

//         QPalette palette = selectedOption->palette();
//         QBrush selected( QColor(255, 0, 0, 255) );
//         selected.setStyle( Qt::SolidPattern );
//         palette.setBrush( QPalette::Active, QPalette::Window, selected );
//         selectedOption->setPalette( palette );

//         emit selectedGrid( rows, columns );
        hide();
        return true;
    }
    else
    {
        return QWidget::event( event );
    }


    

}


}
