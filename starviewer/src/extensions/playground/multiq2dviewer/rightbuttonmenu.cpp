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

namespace udg {

RightButtonMenu::RightButtonMenu( QWidget *parent)
{
    setupUi( this );
    QMenu rightMenu(this);

    QPalette palette = rightMenu.palette();
    QBrush black(QColor(0, 0, 0, 255));
    black.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::WindowText, black);
    palette.setBrush(QPalette::Disabled, QPalette::Text, black);
    palette.setBrush(QPalette::Disabled, QPalette::Button, black);
    palette.setBrush(QPalette::Disabled, QPalette::ButtonText, black);
    rightMenu.setPalette(palette);

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
        studyWidget->show();
        verticalLayout->addWidget(studyWidget,numberStudy,0);
    }

}

void RightButtonMenu::contextMenuEvent(QContextMenuEvent *event)
{
    rightMenu.exec(event->globalPos());
}


QWidget * RightButtonMenu::createStudyWidget( Study * study, QWidget * parent )
{
    QWidget * studyWidget = new QWidget( parent );
    QWidget * serieWidget;
    QList<Series*> series;
    int numberSerie;

//     QWidget * infoStudyWidget = new QWidget();
//     QPalette palette( studyWidget->palette() );
//     QBrush studyBackground(QColor(197, 197, 197, 255));
//     studyBackground.setStyle(Qt::SolidPattern);
//     palette.setBrush(QPalette::Active, QPalette::Window, studyBackground);
//     infoStudyWidget->setPalette(palette);
//     infoStudyWidget->show();
    
    QHBoxLayout * horizontalLayout = new QHBoxLayout( );
//     infoStudyWidget->setLayout( horizontalLayout );

    QLabel * studyText = new QLabel( studyWidget );
    studyText->setText("Study");
    studyText->show();

    QLabel * dateText = new QLabel( studyWidget );
    dateText->setText( tr("Date: %1").arg( study->getDate().toString( "dd/MM/yyyy") ) );
    dateText->show();

    QLabel * descriptionText = new QLabel( studyWidget );
    descriptionText->setText( tr("Description: %1").arg( study->getDescription() ) );
    descriptionText->show();

    horizontalLayout->addWidget( studyText );
    horizontalLayout->addWidget( dateText );
    horizontalLayout->addWidget( descriptionText );

    QAction * studyAction = new QAction(
        tr("Study: Date: %1 Description: %2")
        .arg( study->getDate().toString( "dd/MM/yyyy") )
        .arg( study->getDescription() )
        , this);
    rightMenu.addAction(studyAction);
    studyAction->setEnabled( false );
    studyAction->setFont( QFont() );

    QGridLayout * gridLayout = new QGridLayout( studyWidget );
    QGridLayout * gridLayoutWidgets = new QGridLayout( );
    
    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);
    gridLayout->addLayout(gridLayoutWidgets, 1, 0, 1, 1);

    series = study->getSeries();

    for( numberSerie = 0; numberSerie < study->getNumberOfSeries(); numberSerie++ )
    {
        serieWidget = createSerieWidget( series.value( numberSerie ), studyWidget );
        serieWidget->show();
        gridLayoutWidgets->addWidget( serieWidget, 0, numberSerie );
    }

    return studyWidget;
}

RightMenuItem* RightButtonMenu::createSerieWidget( Series * serie, QWidget * parent )
{
    RightMenuItem * serieWidget = new RightMenuItem( parent );

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

    QAction * serieAction = new QAction(
        tr("Serie: \nModality: %1 \nDescription: %2")
        .arg( serie->getModality() )
        .arg( serie->getDescription() )
        , this);
    rightMenu.addAction(serieAction);

    connect( serieWidget , SIGNAL( isActive( int, QWidget * ) ) , this , SLOT( showInformation( int, QWidget * ) ) );
    return serieWidget;

}

void RightButtonMenu::showInformation( int y, QWidget * moreInformation )
{
    moreInformation->setGeometry( this->x() + this->width(), y , 100, 100 );

}

void RightButtonMenu::updateAdditionalWidget( Series * serie )
{

}


}
