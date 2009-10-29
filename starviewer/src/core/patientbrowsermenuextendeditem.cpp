/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientbrowsermenuextendeditem.h"

#include <QVBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QEvent>

namespace udg {

const QString NoItemText( QObject::tr("No Item Selected") );

PatientBrowserMenuExtendedItem::PatientBrowserMenuExtendedItem(QWidget *parent)
: QFrame(parent)
{
    createInitialWidget();
}

PatientBrowserMenuExtendedItem::~PatientBrowserMenuExtendedItem()
{
}

void PatientBrowserMenuExtendedItem::createInitialWidget()
{
    QVBoxLayout * verticalLayout = new QVBoxLayout( this );
    m_icon = new QLabel( this );
    m_icon->setPixmap( this->makeEmptyThumbnail() );
    m_icon->setAlignment( Qt::AlignCenter );
    verticalLayout->addWidget( m_icon );

    m_text = new QLabel( this );
    m_text->setText(NoItemText);
    m_text->setAlignment( Qt::AlignCenter );
    verticalLayout->addWidget( m_text );
}

void PatientBrowserMenuExtendedItem::setPixmap( const QPixmap &pixmap )
{
    m_icon->setPixmap( pixmap );
}

void PatientBrowserMenuExtendedItem::setText( const QString &text )
{
    m_text->setText( text );
}

QPixmap PatientBrowserMenuExtendedItem::makeEmptyThumbnail()
{
    QPixmap pixmap(100,100);
    pixmap.fill(Qt::black);

    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.drawText(0, 0, 100, 100, Qt::AlignCenter | Qt::TextWordWrap, NoItemText );

    return pixmap;
}

bool PatientBrowserMenuExtendedItem::event( QEvent * event )
{
    if ( event->type() == QEvent::Close ) // s'ha pulsat l'escape
    {
        emit close();
    }
    return QFrame::event( event );
}

}
