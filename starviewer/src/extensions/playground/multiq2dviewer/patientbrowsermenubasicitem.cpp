/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientbrowsermenubasicitem.h"

#include <QPalette>
#include <QListWidgetItem>
#include <QPixmap>
#include <QVBoxLayout>
#include <QMouseEvent>

#include "series.h"
#include "logging.h"

namespace udg {

PatientBrowserMenuBasicItem::PatientBrowserMenuBasicItem( QWidget *parent )
: QLabel(parent)
{
    setFrameShape(QFrame::StyledPanel);
}

void PatientBrowserMenuBasicItem::setSerie( Series * serie )
{
    m_serie = serie;

    setText( tr(" Serie %1 ").arg( serie->getProtocolName().trimmed() ) );
}

Series *  PatientBrowserMenuBasicItem::getSerie()
{
    return m_serie;
}

bool PatientBrowserMenuBasicItem::event( QEvent * event )
{

    if ( event->type() == QEvent::Enter )
    {
        setStyleSheet("background-color: rgba(85, 160, 255, 128);");
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
        setStyleSheet("");
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
