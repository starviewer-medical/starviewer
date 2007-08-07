/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "rightbuttonmenu.h"

#include <QHashIterator>
#include <QLabel>
#include <QGridLayout>
#include <QHashIterator>
#include <QSplitter>
#include <logging.h>
#include <QContextMenuEvent>
#include <QDesktopWidget>
#include <QRect>

namespace udg {

RightButtonMenu::RightButtonMenu( QWidget *parent )
{
    setupUi( this );
    setWindowFlags(Qt::Popup);
}

// RightButtonMenu::setModel( QStandardItemModel * model )
// {
//     QStandardItem * patient = model->item( 0,0 );
//     
//     
// }

void RightButtonMenu::setPatient( Patient * patient )
{

    DEBUG_LOG( QString( "Inici mètode setPatient " ) );

    QWidget * studyWidget;
    Study * study;
    int numberStudy;
    QList< Study* > studies = patient->getStudies();

    this->setWindowTitle( patient->getFullName() );

    QVBoxLayout *verticalLayout = new QVBoxLayout;
    verticalLayout->setMargin(0);
    this->setLayout(verticalLayout);

    for( numberStudy = 0; numberStudy < patient->getNumberOfStudies(); numberStudy++ )
    {
        study = studies.value( numberStudy );
        studyWidget = createStudyWidget( study, this );
        verticalLayout->addWidget(studyWidget,numberStudy,0);
    }

}


QWidget * RightButtonMenu::createStudyWidget( Study * study, QWidget * parent )
{
    QWidget * studyWidget = new QWidget( parent );
    
    QHBoxLayout * horizontalLayout = new QHBoxLayout( );
    horizontalLayout->setSpacing( 0 );
    horizontalLayout->setMargin( 0 );

    QLabel * studyText = new QLabel( studyWidget );
    studyText->setText(" Study ");
    studyText->setAutoFillBackground( true );

    QPalette palette( studyText->palette() );
    QBrush studyBackground( QColor( 85, 160, 255, 255 ) );
    studyBackground.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Window, studyBackground);

    studyText->setPalette(palette);

    QLabel * dateText = new QLabel( studyWidget );
    dateText->setText( tr(" Date: %1 ").arg( study->getDateAsString() ) );
    dateText->setAutoFillBackground( true );
    dateText->setPalette(palette);
    dateText->show();

    QLabel * descriptionText = new QLabel( studyWidget );
    descriptionText->setText( tr(" Description: %1 ").arg( study->getDescription() ) );
    descriptionText->setAutoFillBackground( true );
    descriptionText->setPalette(palette);
    descriptionText->show();

    horizontalLayout->addWidget( studyText );
    horizontalLayout->addWidget( dateText );
    horizontalLayout->addWidget( descriptionText );

    QGridLayout * gridLayout = new QGridLayout( studyWidget );
    QGridLayout * gridLayoutWidgets = new QGridLayout( );
    
    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);
    gridLayout->addLayout(gridLayoutWidgets, 1, 0, 1, 1);

    QList<Series*> seriesToAdd = study->getSeries();
    int maxColumns = 4;
    int row = 0;

    while (!seriesToAdd.isEmpty())
    {
        int column = 0;
        while ( column < maxColumns && !seriesToAdd.isEmpty())
        {
            gridLayoutWidgets->addWidget( createSerieWidget( seriesToAdd.takeFirst(), studyWidget ), row, column );
            ++column;
        }
        ++row;
    }

    return studyWidget;
}

RightMenuItem* RightButtonMenu::createSerieWidget( Series * serie, QWidget * parent )
{
    RightMenuItem * serieWidget = new RightMenuItem( parent );
    serieWidget->setSerie( serie );

    QLabel * serieText = new QLabel( serieWidget );
    serieText->setText("Serie ");
    serieText->show();

    QLabel * modalityText = new QLabel( serieWidget );
    modalityText->setText( tr("Modality: %1").arg( serie->getModality() ) );
    modalityText->show();

    QLabel * descriptionText = new QLabel( serieWidget );
    descriptionText->setText( tr("Description: %1").arg( serie->getDescription() ) );
    descriptionText->show();

    QVBoxLayout *verticalLayout = new QVBoxLayout;
    verticalLayout->setMargin(0);
    verticalLayout->addWidget( serieText, 0 );
    verticalLayout->addWidget( modalityText, 1 );
    verticalLayout->addWidget( descriptionText, 2 );
    serieWidget->setLayout(verticalLayout);

    connect( serieWidget , SIGNAL( isActive( int, QWidget * ) ) , this , SLOT( showInformation( int, QWidget * ) ) );

    connect( serieWidget , SIGNAL( selectedSerie( Series * ) ) , this , SLOT ( emitSelected( Series * ) ) );

    return serieWidget;

}

void RightButtonMenu::showInformation( int y, QWidget * moreInformation )
{

    int x;
    int screen_x= qApp->desktop()->availableGeometry().width();

    // Calcular si hi cap a la dreta, altrament el mostrarem a l'esquerre
    if( (this->x() + this->width() + moreInformation->width() ) > screen_x )
        x = this->x() - moreInformation->width();
    else
        x =  this->x() + this->width();

    moreInformation->move( x, this->y() );
}

void RightButtonMenu::setPosition( QPoint point )
{
    // Calcular si el menu hi cap a la pantalla
    int x = point.x();
    int y = point.y();
    
    int screen_x = qApp->desktop()->availableGeometry().width();
    int screen_y = qApp->desktop()->availableGeometry().height();

    if ( ( x + this->width() ) > screen_x )
    {
        x = screen_x - this->width() - 5;
    }

    if ( ( y + this->height() ) > screen_y )
    {
        y = screen_y - this->height() - 5;
    }

    // moure la finestra del menu al punt que toca
    move( QPoint(x,y) );
}

void RightButtonMenu::emitSelected( Series * serie )
{
    emit selectedSeries( serie );
    hide();
}

}
