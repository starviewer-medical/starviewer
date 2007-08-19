/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientbrowsermenulist.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

#include "patient.h"
#include "study.h"
#include "series.h"
#include "patientbrowsermenubasicitem.h"
#include "logging.h"

namespace udg {

PatientBrowserMenuList::PatientBrowserMenuList( QWidget * parent ) : QWidget(parent)
{
    setWindowFlags(Qt::Popup);
}


PatientBrowserMenuList::~PatientBrowserMenuList()
{
}

void PatientBrowserMenuList::setPatient( Patient * patient )
{

    DEBUG_LOG("Inici mètode setPatient");

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

    QLabel *studyText = new QLabel(studyWidget);
    studyText->setText(tr("Study %1 %2").arg( study->getDateAsString() ).arg( study->getDescription() ));
    studyText->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    studyText->setFrameShape(QFrame::StyledPanel);
    // TODO Hi ha un bug en les Qt 4.2 que fa que els border-radius no retallin el fons, per això fem la xapussa de fer el primer
    // gradient del color del fons i molt petit (dona una mica més sensació de rounded). Bug arreglat a les Qt 4.3
    // TODO En Qt 4.2 (sí en 4.3) no es suporta escollir un color de la palette, per tant, s'ha d'anar a buscar bucejant en la palette.
    QString backgroundColor = studyText->palette().color( studyText->backgroundRole() ).name();
    studyText->setStyleSheet("border: 2px solid #3E73B9;"
                             "border-radius: 5;"
                             "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                             "stop: 0 " + backgroundColor +
                             ", stop: 0.1 #91C1FF, stop: 0.4 #8EBCF9, stop: 0.5 #86B2EC, stop: 1 #759CCF);");

    QVBoxLayout * gridLayout = new QVBoxLayout( studyWidget );
    QGridLayout * gridLayoutWidgets = new QGridLayout( );

    gridLayout->addWidget(studyText);
    gridLayout->addLayout(gridLayoutWidgets);

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
