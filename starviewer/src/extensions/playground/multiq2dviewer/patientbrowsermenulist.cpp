/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientbrowsermenulist.h"

#include <QLabel>
#include <QGridLayout>
#include <logging.h>

namespace udg {

PatientBrowserMenuList::PatientBrowserMenuList( QWidget * parent )
{
    setupUi( this );
    setWindowFlags(Qt::Popup);
}


PatientBrowserMenuList::~PatientBrowserMenuList()
{
}

void PatientBrowserMenuList::setPatient( Patient * patient )
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

QWidget * PatientBrowserMenuList::createStudyWidget( Study * study, QWidget * parent )
{
    QWidget * studyWidget = new QWidget( parent );
    
    QHBoxLayout * horizontalLayout = new QHBoxLayout( );
    horizontalLayout->setSpacing( 0 );
    horizontalLayout->setMargin( 0 );

    QLabel * studyText = new QLabel( studyWidget );
    studyText->setText( " Study " );
    studyText->setAutoFillBackground( true );
    studyText->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    QPalette palette( studyText->palette() );
    QBrush studyBackground( QColor( 85, 160, 255, 255 ) );
    studyBackground.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Window, studyBackground);
    QFont font = studyText->font();
    font.setBold( true );

    studyText->setPalette(palette);
    studyText->setFont( font );

    QLabel * dateText = new QLabel( studyWidget );
    dateText->setText( tr(" %1 ").arg( study->getDateAsString() ) );
    dateText->setAutoFillBackground( true );
    dateText->setPalette( palette );
    dateText->setFont( font );
    dateText->show();

    QLabel * descriptionText = new QLabel( studyWidget );
    descriptionText->setText( tr(" %1 ").arg( study->getDescription() ) );
    descriptionText->setAutoFillBackground( true );
    descriptionText->setPalette(palette);
    descriptionText->setFont( font );
    descriptionText->show();

    horizontalLayout->addWidget( studyText );
    horizontalLayout->addWidget( dateText );
    horizontalLayout->addWidget( descriptionText );

    QGridLayout * gridLayout = new QGridLayout( studyWidget );
    QGridLayout * gridLayoutWidgets = new QGridLayout( );
    
    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);
    gridLayout->addLayout(gridLayoutWidgets, 1, 0, 1, 1);

    QList<Series*> seriesToAdd = study->getSeries();
    int maxColumns = 2;
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

PatientBrowserMenuBasicItem* PatientBrowserMenuList::createSerieWidget( Series * serie, QWidget * parent )
{
    PatientBrowserMenuBasicItem * seriebasicWidget = new PatientBrowserMenuBasicItem( parent );
    seriebasicWidget->setSerie( serie );

    connect( seriebasicWidget , SIGNAL( selectedSerie( Series *) ) , this , SIGNAL( selectedSerie( Series * ) ) );
    connect( seriebasicWidget , SIGNAL( isActive( int, Series *) ) , this , SIGNAL( isActive( int, Series *) ) );
    connect( seriebasicWidget , SIGNAL( isNotActive() ) , this , SIGNAL( isNotActive() ) );

    return seriebasicWidget;
}

}
