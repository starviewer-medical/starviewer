/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientbrowsermenu.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QVBoxLayout>
#include <QMouseEvent>

#include "patient.h"
#include "patientbrowsermenuextendeditem.h"
#include "patientbrowsermenulist.h"
#include "series.h"
#include "image.h"
#include "volume.h"
#include "volumerepository.h"

namespace udg {

PatientBrowserMenu::PatientBrowserMenu(QWidget *parent) : QWidget(parent)
{
    m_patientAdditionalInfo = new PatientBrowserMenuExtendedItem(this);
    m_patientBrowserList = new PatientBrowserMenuList(this);

    m_patientAdditionalInfo->setWindowFlags( Qt::Popup );
    m_patientBrowserList->setWindowFlags( Qt::Popup );
    
    connect( m_patientAdditionalInfo, SIGNAL( close() ), m_patientBrowserList, SLOT( close() ) );
    connect( m_patientBrowserList, SIGNAL( close() ), m_patientAdditionalInfo, SLOT( close() ) );
}

PatientBrowserMenu::~PatientBrowserMenu()
{
}

void PatientBrowserMenu::setPatient(Patient * patient)
{
    m_patient = patient;
    QString caption;
    QString label;
    foreach( Study *study, m_patient->getStudies() )
    {
        // Extreiem el caption de l'estudi
        caption = tr("Study %1 : %2 [%3] %4")
            .arg( study->getDateAsString() )
            .arg( study->getTimeAsString() )
            .arg( study->getModalitiesAsSingleString() )
            .arg( study->getDescription() );

        // Per cada sèrie de l'estudi extreurem el seu label i l'identificador
        QList< QPair<QString,QString> > itemsList;
        foreach( Series *series, study->getViewableSeries() )
        {
            label = tr(" Serie %1: %2 %3 %4 %5")
                        .arg( series->getSeriesNumber().trimmed() )
                        .arg( series->getProtocolName().trimmed() )
                        .arg( series->getDescription().trimmed() )
                        .arg( series->getBodyPartExamined() )
                        .arg( series->getViewPosition() );
            
            if( series->getNumberOfVolumes() > 1 )
            {
                QString volumeID;
                int volumeNumber = 1;
                foreach( Volume *volume, series->getVolumesList() )
                {
                    QPair<QString,QString> itemPair;
                    // label
                    itemPair.first = label + " (" + QString::number(volumeNumber) + ")";
                    volumeNumber++;
                    // identifier
                    itemPair.second = QString::number(volume->getIdentifier().getValue());
                    // afegim el parell a la llista
                    itemsList << itemPair;
                }
            }
            else // només tenim un sol volum per la sèrie
            {
                Volume *volume = series->getFirstVolume();
                QPair<QString,QString> itemPair;
                // label
                itemPair.first = label;
                // identifier
                itemPair.second = QString::number(volume->getIdentifier().getValue());
                // afegim el parell a la llista
                itemsList << itemPair;
            }
        }
        // afegim les sèries agrupades per estudi
        m_patientBrowserList->addItemsGroup( caption, itemsList );
    }

    connect(m_patientBrowserList, SIGNAL( isActive(QString) ), SLOT( updateActiveItemView(QString) ));
    connect(m_patientBrowserList, SIGNAL( selectedItem(QString) ), SLOT ( emitSelected(QString) ));
}

void PatientBrowserMenu::updateActiveItemView(const QString &identifier)
{
    Identifier id( identifier.toInt() );
    Volume *volume = VolumeRepository::getRepository()->getVolume( id );
    if( volume )
    {
        // actualitzem les dades de l'item amb informació adicional
        m_patientAdditionalInfo->setPixmap( volume->getThumbnail() );
        Series *series = volume->getImage(0)->getParentSeries();
        m_patientAdditionalInfo->setText( QString( tr("%1 \n%2 \n%3\n%4 Images") )
                                        .arg( series->getDescription().trimmed() )
                                        .arg( series->getModality().trimmed() )
                                        .arg( series->getProtocolName().trimmed() )
                                        .arg( volume->getImages().count() )
                                        );
        // Actualitzem la posició del widget amb la informació adicional
        updatePosition();
    }
}

void PatientBrowserMenu::popup(const QPoint &point, const QString &identifier)
{
    // Calcular si el menu hi cap a la pantalla
    int x = point.x();
    int y = point.y();

    int screen_x, screen_y;

    if (qApp->desktop()->isVirtualDesktop())
    {
        screen_x = qApp->desktop()->geometry().width();
        screen_y = qApp->desktop()->geometry().height();
    }
    else
    {
        screen_x = qApp->desktop()->availableGeometry( point ).width();
        screen_y = qApp->desktop()->availableGeometry( point ).height();
    }

    m_patientBrowserList->markItem( identifier );
    QSize widgetIdealSize = m_patientBrowserList->sizeHint();

    if ( ( x + widgetIdealSize.width() ) > screen_x )
    {
        x = screen_x - widgetIdealSize.width() - 5;
    }

    if ( ( y + widgetIdealSize.height() ) > screen_y )
    {
        y = screen_y - widgetIdealSize.height() - 5;
    }

    // TODO: HACK si no mostrem l'"Aditional info" abans
    // que el "browser list" després no processa els events
    // correctament i quan seleccionem una sèrie no arriba
    // el signal enlloc i no es pot seleccionar cap sèrie
    // relacionat amb el ticket #555
    // Això només passa amb qt 4.3, amb qt 4.2 no tenim aquest 
    // problema. Amb qt 4.2 podem fer show en l'ordre que volguem
    // El que fem per evitar flickering és mostrar-lo sota mateix
    // del "browser list" i així no es nota tant
    m_patientAdditionalInfo->move( x, y );
    m_patientAdditionalInfo->show();
    // FI HACK

    //moure la finestra del menu al punt que toca
    m_patientBrowserList->move(x, y);
    m_patientBrowserList->show();
    
    QSize patientAdditionalInfoSize = m_patientAdditionalInfo->sizeHint();
    m_patientAdditionalInfo->resize( patientAdditionalInfoSize );

    // Calcular si hi cap a la dreta, altrament el mostrarem a l'esquerre del menu
    if( (m_patientBrowserList->x() + m_patientBrowserList->width() + patientAdditionalInfoSize.width() ) > screen_x )
    {
        x = ( m_patientBrowserList->x() ) -( m_patientAdditionalInfo->frameGeometry().width() );
    }
    else
    {
        x =  m_patientBrowserList->x() + m_patientBrowserList->width();
    }
    m_patientAdditionalInfo->move( x, m_patientBrowserList->y() );

    // TODO: HACK
    m_patientAdditionalInfo->show();
    // FI HACK
}

void PatientBrowserMenu::emitSelected(const QString &identifier)
{
    // HACK De moment això és un workaround per solucionar el ticket #824
    // és important que s'esborrin en aquest ordre, sinó es fa així el problema persisteix
    // ara també ens funciona bé perquè creem cada cop un PatientBrowserMenu.
    // Si en tinguéssim un d'estàtic segurament això ens ocasionaria que l'aplicació petaria més endavant
    delete m_patientAdditionalInfo;
    delete m_patientBrowserList;

    Identifier id( identifier.toInt() );
    emit selectedVolume( VolumeRepository::getRepository()->getVolume( id ) );
}

void PatientBrowserMenu::updatePosition()
{
    int x;
    int screen_x;
    //Passem el point per assegurar-nos que s'agafa la pantalla a on es visualitza el widget
    if (qApp->desktop()->isVirtualDesktop())
    {
        screen_x = qApp->desktop()->geometry().width();
    }
    else
    {
        screen_x = qApp->desktop()->availableGeometry( m_patientBrowserList->pos() ).width();
    }

    QSize patientAdditionalInfoSize = m_patientAdditionalInfo->sizeHint();
    m_patientAdditionalInfo->resize( patientAdditionalInfoSize );

    // Calcular si hi cap a la dreta, altrament el mostrarem a l'esquerre del menu
    if( (m_patientBrowserList->x() + m_patientBrowserList->width() + patientAdditionalInfoSize.width() ) > screen_x )
    {
        x = ( m_patientBrowserList->x() ) -( m_patientAdditionalInfo->frameGeometry().width() );
    }
    else
    {
        x =  m_patientBrowserList->x() + m_patientBrowserList->width();
    }
    m_patientAdditionalInfo->move( x, m_patientBrowserList->y() );
    m_patientAdditionalInfo->show();
    m_patientBrowserList->show();
}

}
