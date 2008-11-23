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

#include "logging.h"

namespace udg {

PatientBrowserMenuExtendedItem::PatientBrowserMenuExtendedItem(QWidget *parent)
: QFrame(parent)
{
    m_series = 0;
    createInitialWidget();
}

PatientBrowserMenuExtendedItem::PatientBrowserMenuExtendedItem(Series * series, QWidget *parent)
: QFrame(parent)
{
    createInitialWidget();
    setSeries( series );
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
    m_text->setText(tr("No Series Selected"));
    m_text->setAlignment( Qt::AlignCenter );
    verticalLayout->addWidget( m_text );
}

void PatientBrowserMenuExtendedItem::setSeries( Series *series )
{
    m_series = series;

    m_icon->setPixmap( series->getThumbnail() );
    m_text->setText( QString( tr("%1 \n%2 \n%3\n%4 Images") )
                    .arg( series->getDescription().trimmed() )
                    .arg( series->getModality().trimmed() )
                    .arg( series->getProtocolName().trimmed() )
                    .arg( series->getNumberOfImages() )
                    );
}

QPixmap PatientBrowserMenuExtendedItem::makeEmptyThumbnail()
{
    QPixmap pixmap(100,100);
    pixmap.fill(Qt::black);

    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.drawText(0, 0, 100, 100, Qt::AlignCenter | Qt::TextWordWrap, tr("No Series Selected"));

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
