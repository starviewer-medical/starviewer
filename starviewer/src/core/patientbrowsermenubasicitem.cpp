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

    setStyleSheet( "border: 1px solid gray; border-radius: 2;" );
}

Series *  PatientBrowserMenuBasicItem::getSerie()
{
    return m_serie;
}

bool PatientBrowserMenuBasicItem::event( QEvent * event )
{
    if ( event->type() == QEvent::Enter )
    {
        if( this->font().bold() )
        {
            setStyleSheet( "border: 1px solid gray; border-radius: 2; background-color: rgba(85, 160, 255, 128); font-weight: bold;" );
        }
        else
        {
            setStyleSheet( "border: 1px solid gray; border-radius: 2; background-color: rgba(85, 160, 255, 128);" );
        }
        emit isActive( m_serie );
    }
    else if ( event->type() == QEvent::MouseButtonPress )
    {
        emit selectedSerie( m_serie );
        return true;
    }
    else if ( event->type() == QEvent::Leave )
    {
        if( this->font().bold() )
        {
            setStyleSheet( "border: 1px solid gray; border-radius: 2; font-weight: bold" );
        }
        else
        {
            setStyleSheet( "border: 1px solid gray; border-radius: 2;" );
        }
    }
    else if ( event->type() == QEvent::Hide || event->type() == QEvent::Close )
    {
        emit isNotActive();
    }

    return QLabel::event( event );
}

void PatientBrowserMenuBasicItem::setFontBold()
{
    setStyleSheet( "border: 1px solid gray; border-radius: 2;font-weight: bold" );
}

}
