/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientbrowsermenuextendeditem.h"

#include <QVBoxLayout>
#include <QPixmap>
#include <logging.h>

namespace udg {

PatientBrowserMenuExtendedItem::PatientBrowserMenuExtendedItem( QFrame *parent )
: QFrame(parent)
{
    createInitialWidget();
}

PatientBrowserMenuExtendedItem::PatientBrowserMenuExtendedItem( Series * serie, QFrame *parent )
: QFrame(parent)
{
    setSerie( serie );
    createInitialWidget();
}

PatientBrowserMenuExtendedItem::~PatientBrowserMenuExtendedItem()
{
}

void PatientBrowserMenuExtendedItem::createInitialWidget()
{
    setWindowFlags(Qt::ToolTip);

    QVBoxLayout * verticalLayout = new QVBoxLayout( this );
    m_icon = new QLabel( this );
    verticalLayout->addWidget( m_icon );

    m_text = new QLabel( this );
    verticalLayout->addWidget( m_text );

    hide();
}

void PatientBrowserMenuExtendedItem::setSerie( Series * serie)
{
    m_serie = serie;
    
    m_icon->setPixmap( QPixmap( ":/images/axial.png" ) );
    m_text->setText( QString( tr("%1 \n%2 \n%3")
                    .arg( m_serie->getDescription().trimmed() )
                    .arg( serie->getModality().trimmed() )
                    .arg( serie->getProtocolName().trimmed() )
                    ) );
    m_text->show();
    m_icon->show();
}

void PatientBrowserMenuExtendedItem::showSerie( int y , Series * serie )
{
    setSerie( serie );
    emit setPosition( this, y );
}

}
