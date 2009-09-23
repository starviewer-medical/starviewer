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

#include <cmath>

namespace udg {

PatientBrowserMenuList::PatientBrowserMenuList( QWidget * parent ) : QWidget(parent)
{
}

PatientBrowserMenuList::~PatientBrowserMenuList()
{
}

void PatientBrowserMenuList::setPatient( Patient * patient )
{
    if ( this->layout() )
    {
        // Si ens canvien el pacient, ens carreguem els widgets de l'anterior
        delete this->layout();
    }

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setMargin(0);

    foreach (Study *study, patient->getStudies())
    {
        verticalLayout->addWidget( createStudyWidget(study, this) );
    }
}

QWidget * PatientBrowserMenuList::createStudyWidget( Study * study, QWidget * parent )
{
    QWidget * studyWidget = new QWidget( parent );

    QLabel *studyText = new QLabel(studyWidget);
    studyText->setText( tr("Study %1 : %2 [%3] %4")
            .arg( study->getDateAsString() )
            .arg( study->getTimeAsString() )
            .arg( study->getModalitiesAsSingleString() )
            .arg( study->getDescription() )
        );
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

    // les sèries que al final afegirem al llistat
    QList<Series*> seriesToAdd = study->getViewableSeries(); 
    //comptem el nombre de series que seran visibles
    int numberOfViewableSeries = seriesToAdd.count();

    int maxColumns = 2;
    if ( numberOfViewableSeries >= 20 )
        maxColumns = 3;

    int row = 0;
    int column = 0;
    int columnElements = ceil ( (double) numberOfViewableSeries / maxColumns );

    while (!seriesToAdd.isEmpty())
    {
        while ( row < columnElements && !seriesToAdd.isEmpty())
        {
            gridLayoutWidgets->addWidget( createSerieWidget(seriesToAdd.takeFirst(), studyWidget), row, column );
            ++row;
        }
        row = 0;
        ++column;
    }

    return studyWidget;
}

PatientBrowserMenuBasicItem *PatientBrowserMenuList::createSerieWidget( Series * serie, QWidget * parent )
{
    PatientBrowserMenuBasicItem *seriebasicWidget = new PatientBrowserMenuBasicItem( parent );
    
    seriebasicWidget->setText( tr(" Serie %1: %2 %3 %4 %5")
                        .arg( serie->getSeriesNumber().trimmed() )
                        .arg( serie->getProtocolName().trimmed() )
                        .arg( serie->getDescription().trimmed() )
                        .arg( serie->getBodyPartExamined() )
                        .arg( serie->getViewPosition() )
                        );
    seriebasicWidget->setIdentifier( serie->getInstanceUID() );

    seriebasicWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    connect( seriebasicWidget, SIGNAL( selectedItem(QString) ), SIGNAL( selectedItem(QString) ) );
    connect( seriebasicWidget, SIGNAL( isActive(QString) ), SIGNAL( isActive(QString) ) ) ;
    connect( seriebasicWidget, SIGNAL( isNotActive() ), SIGNAL( isNotActive() ) );

    m_itemsList.push_back( seriebasicWidget );

    return seriebasicWidget;
}

void PatientBrowserMenuList::setSelectedItem( const QString &identifier )
{
    int i = 0;
    bool found = false;

    while( i < m_itemsList.size() && !found )
    {
        if( m_itemsList.value(i)->getIdentifier() == identifier )
        {
            found = true;
            m_itemsList.value(i)->setFontBold();
        }
        i++;
    }
}

bool PatientBrowserMenuList::event( QEvent * event )
{
    if ( event->type() == QEvent::Close ) // s'ha pulsat l'escape
    {
        emit close();
    }
    return QWidget::event( event );
}

}
