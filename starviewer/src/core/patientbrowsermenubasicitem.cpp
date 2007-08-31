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

    setText( tr(" Serie %1: %2 %3 %4 %5")
        .arg( serie->getSeriesNumber().trimmed() )
        .arg( serie->getProtocolName().trimmed() )
        .arg( serie->getDescription().trimmed() )
        .arg( serie->getBodyPartExamined() )
        .arg( serie->getViewPosition() )
        );
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

//         QPalette palette = this->palette();
//         QBrush selected( QColor( 85, 160, 255, 128 ) );
//         selected.setStyle( Qt::SolidPattern );
//         palette.setBrush( QPalette::Active, QPalette::Window, selected );
//         setPalette( palette );

        emit isActive(m_serie);
    }
    else if ( event->type() == QEvent::MouseButtonPress )
    {
        emit selectedSerie( m_serie );
    }
    else if ( event->type() == QEvent::Leave )
    {
        setStyleSheet("");

//         QPalette palette = this->palette();
//         QBrush selected( QColor(239, 243, 247, 255) );
//         selected.setStyle( Qt::SolidPattern );
//         palette.setBrush( QPalette::Active, QPalette::Window, selected );
//         setPalette( palette );
    }
    else if ( event->type() == QEvent::Hide || event->type() == QEvent::Close )
    {
        emit isNotActive();
    }

    return QLabel::event(event);
}
}
