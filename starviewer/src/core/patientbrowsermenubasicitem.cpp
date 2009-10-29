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

#include "logging.h"

namespace udg {

PatientBrowserMenuBasicItem::PatientBrowserMenuBasicItem( QWidget *parent )
: QLabel(parent)
{
    setFrameShape(QFrame::StyledPanel);
    setStyleSheet( "border: 1px solid gray; border-radius: 2;" );
}

void PatientBrowserMenuBasicItem::setIdentifier( const QString &identifier )
{
    m_identifier = identifier;
}

QString PatientBrowserMenuBasicItem::getIdentifier() const
{
    return m_identifier;
}

void PatientBrowserMenuBasicItem::setFontBold()
{
    setStyleSheet( "border: 1px solid gray; border-radius: 2;font-weight: bold" );
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
        emit isActive( m_identifier );
    }
    else if ( event->type() == QEvent::MouseButtonPress )
    {
        emit selectedItem( m_identifier );
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

}
