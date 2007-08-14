/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientbrowsermenubasicitem.h"
#include "QPalette"
#include "QListWidgetItem"
#include "QLabel"
#include "QPixmap"
#include "logging.h"
#include "QVBoxLayout"
#include "QMouseEvent"

namespace udg {

PatientBrowserMenuBasicItem::PatientBrowserMenuBasicItem( QWidget *parent )
: QFrame(parent)
{
    setupUi( this );
    setAutoFillBackground( true );
}

void PatientBrowserMenuBasicItem::setSerie( Series * serie )
{
    m_serie = serie;

    QLabel * serieText = new QLabel( this );
    serieText->setText( tr(" Serie %1 ").arg( serie->getProtocolName().trimmed() ) );
    serieText->show();
    QVBoxLayout *verticalLayout = new QVBoxLayout;
    verticalLayout->setMargin(0);
    verticalLayout->addWidget( serieText, 0 );
    this->setLayout( verticalLayout );
}

Series *  PatientBrowserMenuBasicItem::getSerie()
{
    return m_serie;
}

bool PatientBrowserMenuBasicItem::event( QEvent * event )
{

    if ( event->type() == QEvent::Enter )
    {
        QPalette palette = this->palette();
        QBrush selected( QColor( 85, 160, 255, 128 ) );
        selected.setStyle( Qt::SolidPattern );
        palette.setBrush( QPalette::Active, QPalette::Window, selected );
        setPalette( palette );
        emit isActive( this->geometry().y(), m_serie );
        return true;
    }
    else if ( event->type() == QEvent::MouseButtonPress )
    {
        emit selectedSerie( m_serie );
        return true;
    }
    else if ( event->type() == QEvent::Leave )
    {
        QPalette palette = this->palette();
        QBrush selected( QColor(239, 243, 247, 255) );
        selected.setStyle( Qt::SolidPattern );
        palette.setBrush( QPalette::Active, QPalette::Window, selected );
        setPalette( palette );
        return true;
    }
    else if ( event->type() == QEvent::Hide || event->type() == QEvent::Close )
    {
        emit isNotActive();
        return QWidget::event( event );
    }
    else
    {
        return QWidget::event( event );
    }

}
}
