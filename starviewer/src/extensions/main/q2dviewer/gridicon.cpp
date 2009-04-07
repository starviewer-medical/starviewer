/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "gridicon.h"
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

#include "logging.h"

namespace udg {

GridIcon::GridIcon( QWidget *parent, QString iconType )
 : QFrame( parent )
{
    QGridLayout * gridLayout = new QGridLayout( this );
    gridLayout->setSpacing(0);
    gridLayout->setMargin(1);
    m_label = new QLabel( this );
    m_label->setFrameShape(QFrame::StyledPanel);
    QPixmap * pixmap = new QPixmap();
    
    if( iconType != 0 )
    {
        if( iconType == "COR" )
            pixmap->load( QString::fromUtf8(":/images/coronal.png") );
        else if( iconType == "SAG" )
            pixmap->load( QString::fromUtf8(":/images/sagital.png") );
        else if( iconType == "ECO" )
            pixmap->load( QString::fromUtf8(":/images/eco.png") );
        else if( iconType == "MLO_R" )
            pixmap->load( QString::fromUtf8(":/images/MLO_R.png") );
        else if( iconType == "MLO_L" )
            pixmap->load( QString::fromUtf8(":/images/MLO_L.png") );
        else if( iconType == "CC_R" )
            pixmap->load( QString::fromUtf8(":/images/CC_R.png") );
        else if( iconType == "CC_L" )
            pixmap->load( QString::fromUtf8(":/images/CC_L.png") );
    }
    else
    {
        pixmap->load( QString::fromUtf8(":/images/axial.png") );
    }

    m_label->setPixmap( *pixmap );
    gridLayout->addWidget(m_label, 0, 0, 1, 1);
}

GridIcon::~GridIcon()
{
}

void GridIcon::resizeEvent ( QResizeEvent * event )
{
    QFrame::resizeEvent( event );

    m_label->setPixmap( m_label->pixmap()->scaled( event->size().width()-2,event->size().height()-2, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
    m_label->setAlignment( Qt::AlignCenter );
}

}
