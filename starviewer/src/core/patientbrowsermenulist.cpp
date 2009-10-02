/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientbrowsermenulist.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

#include "patientbrowsermenubasicitem.h"

#include <cmath>

namespace udg {

PatientBrowserMenuList::PatientBrowserMenuList( QWidget * parent ) : QWidget(parent)
{
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setMargin(0);
}

PatientBrowserMenuList::~PatientBrowserMenuList()
{
}

void PatientBrowserMenuList::addItemsGroup( const QString &caption, const QList< QPair<QString,QString> > &itemsList )
{
    QWidget *groupWidget = new QWidget(this);

    QLabel *captionLabel = new QLabel(groupWidget);
    captionLabel->setText(caption);
    captionLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    captionLabel->setFrameShape(QFrame::StyledPanel);

    // Li donem l'style sheet a la caption que titula el grup d'ítems
    QString backgroundColor = captionLabel->palette().color( captionLabel->backgroundRole() ).name();
    captionLabel->setStyleSheet("border: 2px solid #3E73B9;"
                             "border-radius: 5;"
                             "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                             "stop: 0 " + backgroundColor +
                             ", stop: 0.1 #91C1FF, stop: 0.4 #8EBCF9, stop: 0.5 #86B2EC, stop: 1 #759CCF);");

    QVBoxLayout *groupLayout = new QVBoxLayout(groupWidget);
    QGridLayout *gridLayoutWidgets = new QGridLayout();

    groupLayout->addWidget(captionLabel);
    groupLayout->addLayout(gridLayoutWidgets);

    //comptem el nombre de series que seran visibles
    int numberOfItems = itemsList.count();

    int maxColumns = 2;
    if ( numberOfItems >= 20 )
        maxColumns = 3;

    int row = 0;
    int column = 0;
    int itemsPerColumn = ceil ( (double) numberOfItems / maxColumns );

    typedef QPair<QString,QString> MyPair;
    foreach( MyPair itemPair, itemsList )
    {
        gridLayoutWidgets->addWidget( createBasicItem(itemPair.first,itemPair.second), row, column );
        row++;
        if( row >= itemsPerColumn )
        {
            row = 0;
            column++;
        }
    }

    m_verticalLayout->addWidget(groupWidget);
}

PatientBrowserMenuBasicItem *PatientBrowserMenuList::createBasicItem( const QString &label, const QString &identifier )
{
    PatientBrowserMenuBasicItem *seriebasicWidget = new PatientBrowserMenuBasicItem(this);

    seriebasicWidget->setText( label );
    seriebasicWidget->setIdentifier( identifier );
    seriebasicWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    connect( seriebasicWidget, SIGNAL( selectedItem(QString) ), SIGNAL( selectedItem(QString) ) );
    connect( seriebasicWidget, SIGNAL( isActive(QString) ), SIGNAL( isActive(QString) ) ) ;
    connect( seriebasicWidget, SIGNAL( isNotActive() ), SIGNAL( isNotActive() ) );

    m_itemsList.push_back( seriebasicWidget );

    return seriebasicWidget;
}

void PatientBrowserMenuList::markItem( const QString &identifier )
{
    int i = 0;
    bool found = false;

    while( i < m_itemsList.size() && !found )
    {
        if( m_itemsList.value(i)->getIdentifier() == identifier )
        {
            found = true;
            m_itemsList.value(i)->setFontBold();
            m_markedItem = identifier;
        }
        i++;
    }
}

QString PatientBrowserMenuList::getMarkedItem() const
{
    return m_markedItem;
}

bool PatientBrowserMenuList::event( QEvent * event )
{
    if ( event->type() == QEvent::Close ) // s'ha pulsat l'escape
    {
        emit close();
    }
    return QWidget::event( event );
}

}
