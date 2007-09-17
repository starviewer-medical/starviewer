/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "gridicon.h"

namespace udg {

GridIcon::GridIcon( QWidget *parent )
 : QFrame( parent )
{
    QGridLayout * gridLayout = new QGridLayout( this );
    gridLayout->setSpacing(0);
    gridLayout->setMargin(1);
    label = new QLabel( this );
    label->setFrameShape(QFrame::StyledPanel);
    label->setPixmap(QPixmap(QString::fromUtf8(":/images/axial.png")));
    label->setScaledContents(true);
    label->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(label, 0, 0, 1, 1);
}

GridIcon::~GridIcon()
{
}


}
