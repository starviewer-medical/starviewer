/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "patientbrowsermenu.h"

#include "patient.h"
#include "patientbrowsermenuextendedinfo.h"
#include "patientbrowsermenuextendeditem.h"
#include "patientbrowsermenulist.h"
#include "series.h"
#include "image.h"
#include "volume.h"
#include "volumerepository.h"
#include "screenmanager.h"

#include "patientbrowsermenugroup.h"

#include <QPainter>
#include <QVBoxLayout>

namespace udg {

PatientBrowserMenu::PatientBrowserMenu(QWidget *parent)
 : QWidget(parent), m_patientBrowserList(0), m_patientAdditionalInfo(0)
{
    // Inicialment no sabem en quina pantalla es pot desplegar el menú
    m_currentScreenID = m_leftScreenID = m_rightScreenID = -1;

    m_showFusionOptions = false;
}

PatientBrowserMenu::~PatientBrowserMenu()
{
}

void PatientBrowserMenu::setPatient(Patient *patient)
{
    createWidgets();
    QString caption;
    QString label;
    
    foreach (Study *study, patient->getStudies())
    {
        // Extreiem el caption de l'estudi
        caption = tr("Study %1 %2 [%3] %4")
            .arg(study->getDate().toString(Qt::ISODate))
            .arg(study->getTimeAsString())
            .arg(study->getModalitiesAsSingleString())
            .arg(study->getDescription());

        // Per cada sèrie de l'estudi extreurem el seu label i l'identificador
        QList<QPair<QString, QString> > itemsList;
        QList<QPair<QString, QString> > fusionItemsList;
        foreach (Series *series, study->getViewableSeries())
        {
            label = tr(" Series %1: %2 %3 %4 %5")
                        .arg(series->getSeriesNumber().trimmed())
                        .arg(series->getProtocolName().trimmed())
                        .arg(series->getDescription().trimmed())
                        .arg(series->getBodyPartExamined())
                        .arg(series->getViewPosition());

            int volumeNumber = 1;
            foreach (Volume *volume, series->getVolumesList())
            {
                QPair<QString, QString> itemPair;
                // Label
                if (series->getNumberOfVolumes() > 1)
                {
                    itemPair.first = label + " (" + QString::number(volumeNumber) + ")";
                }
                else
                {
                    itemPair.first = label;
                }

                volumeNumber++;
                // Identifier
                itemPair.second = QString::number(volume->getIdentifier().getValue());
                // Afegim el parell a la llista
                itemsList << itemPair;

                // Look for fusion pairs
                if (m_showFusionOptions)
                {
                    if (series->getModality() == "CT" && !series->isCTLocalizer())
                    {
                        AnatomicalPlane acquisitionPlane = volume->getAcquisitionPlane();

                        if (acquisitionPlane != AnatomicalPlane::NotAvailable)
                        {
                            int zIndex = volume->getCorrespondingOrthogonalPlane(acquisitionPlane).getZIndex();
                            double margin = series->getImages().first()->getSliceThickness() * 5;

                            double range1[2];
                            range1[0] = volume->getImages().first()->getImagePositionPatient()[zIndex];
                            range1[1] = volume->getImages().last()->getImagePositionPatient()[zIndex];
                            
                            foreach (Series * secondSeries, study->getViewableSeries())
                            {
                                if ((secondSeries->getModality() == "PT" || secondSeries->getModality() == "NM") && series->getFrameOfReferenceUID() == secondSeries->getFrameOfReferenceUID())
                                {
                                    foreach (Volume *secondVolume, secondSeries->getVolumesList())
                                    {
                                        if (secondVolume->getAcquisitionPlane() == acquisitionPlane)
                                        {
                                            double range2[2];
                                            range2[0] = secondVolume->getImages().first()->getImagePositionPatient()[zIndex];
                                            range2[1] = secondVolume->getImages().last()->getImagePositionPatient()[zIndex];
                                            
                                            if ((range1[0] + margin > range2[0] && range1[1] - margin < range2[1]) || (range2[0] + margin > range1[0] && range2[1] - margin < range1[1]))
                                            {
                                                QPair<QString, QString> itemPair;
                                                // Label
                                                itemPair.first = QString("%1 + %2").arg(series->getProtocolName().trimmed() + series->getDescription().trimmed()).arg(secondSeries->getProtocolName().trimmed() + secondSeries->getDescription().trimmed());
                                                // Identifier
                                                itemPair.second = QString("%1+%2").arg(volume->getIdentifier().getValue()).arg(secondVolume->getIdentifier().getValue());
                                                // Afegim el parell a la llista
                                                fusionItemsList << itemPair;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // Afegim les sèries agrupades per estudi
        m_patientBrowserList->addItemsGroup(caption, itemsList << fusionItemsList);
    }

    connect(m_patientBrowserList, SIGNAL(isActive(QString)), SLOT(updateActiveItemView(QString)));
    connect(m_patientBrowserList, SIGNAL(selectedItem(QString)), SLOT(processSelectedItem(QString)));
}

void PatientBrowserMenu::setShowFusionOptions(bool show)
{
    m_showFusionOptions = show;
}

void PatientBrowserMenu::updateActiveItemView(const QString &identifier)
{
    if (identifier.contains("+"))
    {
        QList<PatientBrowserMenuExtendedItem*> items;
        foreach(const QString &stringID, identifier.split("+"))
        {
            Identifier id(stringID.toInt());
            Volume *volume = VolumeRepository::getRepository()->getVolume(id);
            if (volume)
            {
                // Actualitzem les dades de l'item amb informació adicional
                PatientBrowserMenuExtendedItem *item = new PatientBrowserMenuExtendedItem(m_patientAdditionalInfo);
                item->setPixmap(volume->getThumbnail());
                Series *series = volume->getImage(0)->getParentSeries();
                item->setText(QString(tr("%1\n%2\n%3\n%4 Images"))
                              .arg(series->getDescription().trimmed())
                              .arg(series->getModality().trimmed())
                              .arg(series->getProtocolName().trimmed())
                              .arg(volume->getNumberOfFrames())
                              );
                items << item;
            }
        }
        m_patientAdditionalInfo->setItems(items);
    }
    else
    {
        Identifier id(identifier.toInt());
        Volume *volume = VolumeRepository::getRepository()->getVolume(id);
        if (volume)
        {
            // Actualitzem les dades de l'item amb informació adicional
            PatientBrowserMenuExtendedItem *item = new PatientBrowserMenuExtendedItem(m_patientAdditionalInfo);
            item->setPixmap(volume->getThumbnail());
            Series *series = volume->getImage(0)->getParentSeries();
            item->setText(QString(tr("%1\n%2\n%3\n%4 Images"))
                                            .arg(series->getDescription().trimmed())
                                            .arg(series->getModality().trimmed())
                                            .arg(series->getProtocolName().trimmed())
                                            .arg(volume->getNumberOfFrames())
                                             );
            m_patientAdditionalInfo->setItems(QList<PatientBrowserMenuExtendedItem*>() << item);
        }
    }
    placeAdditionalInfoWidget();
}

void PatientBrowserMenu::popup(const QPoint &point, const QString &identifier)
{
    // Marquem l'ítem actual de la llista
    m_patientBrowserList->markItem(identifier);

    // Determinem quines són les pantalles que tenim
    ScreenManager screenManager;
    m_currentScreenID = screenManager.getScreenID(point);
    ScreenLayout screenLayout = screenManager.getScreenLayout();
    m_leftScreenID = screenLayout.getScreenOnTheLeftOf(m_currentScreenID);
    m_rightScreenID = screenLayout.getScreenOnTheRightOf(m_currentScreenID);

    QRect currentScreenGeometry = screenLayout.getScreen(m_currentScreenID).getAvailableGeometry();

    m_patientBrowserList->setMaximumWidth(currentScreenGeometry.width() - 300);
    m_patientBrowserList->setMaximumHeight(currentScreenGeometry.height() - currentScreenGeometry.height() / 5);

    // Calculem l'alineament del menú
    bool rightAligned = shouldAlignMenuToTheRight(currentScreenGeometry);
    
    // Calculem quanta part de la llista queda fora segons la geometria de la pantalla on se'ns ha demanat obrir el menú
    // i així poder determinar la posició del widget per tal que no surti fora de la pantalla i es pugui veure el seu contingut.
    QSize outside;
    computeListOutsideSize(point, outside, rightAligned);

    // TODO Què fem si la pantalla adicional té una alçada més petita que l'actual? Recalculem tot el menú perquè tingui l'alçada menor?
    // De moment això no es tindrà en compte
    
    // Si els valors són positius caldrà moure la posició original com a mínim tot el que ens sortim de les fronteres
    // TODO En teoria aquest Margin no seria necessari si fèssim servir frameGeometry() en comptes de sizeHint()
    const int Margin = 5;
    int menuXPosition = point.x();
    int menuYPosition = point.y();
    if (outside.width() > 0)
    {
        menuXPosition -= outside.width() + Margin;
    }
    if (outside.height() > 0)
    {
        menuYPosition -= outside.height() + Margin;
    }

    // Movem la finestra del menu al punt que toca
    m_patientBrowserList->move(menuXPosition, menuYPosition);

    // Col·loquem el widget amb la informació adicional a la dreta o l'esquerra del principal segons l'espai disponible
    placeAdditionalInfoWidget();
    m_patientAdditionalInfo->show();
    // TODO: HACK si no mostrem l'"Aditional info" abans que el "browser list" després no processa els events
    // correctament i quan seleccionem una sèrie no arriba el signal enlloc i no es pot seleccionar cap sèrie
    // relacionat amb el ticket #555 Això només passa amb qt 4.3, amb qt 4.2 no tenim aquest
    // problema. Amb qt 4.2 podem fer show en l'ordre que volguem. El que fem per evitar flickering és mostrar-lo sota mateix
    // del "browser list" i així no es nota tant
    // Això, a partir de Qt 4.7 sembla que només passa a Mac. Semblaria que això és el símptoma d'un altre problema: que es mostri com un popup?
    m_patientBrowserList->show();

    // TODO No es té en compte si després d'haver mogut de lloc el widget aquest ja es veu correctament,
    // ja que es podria donar el cas que el widget no hi cabés a tota la pantalla
    // Caldria millorar el comportament en certs aspectes, com per exemple, redistribuir les files i columnes
    // del llistat si aquest no hi cap a la pantalla, per exemple.
}

bool PatientBrowserMenu::shouldAlignMenuToTheRight(const QRect &currentScreenGeometry)
{
    bool rightAligned = true;
    // Cal comprovar si la llista resultant és més ample
    if (m_patientBrowserList->sizeHint().width() > currentScreenGeometry.width())
    {
        if (m_leftScreenID == -1)
        {
            if (m_rightScreenID == -1)
            {
                // No podem expandir més, no tenim més pantalles.
                // TODO Explorar top i bottom?
            }
            else
            {
                // Tenim pantalla per expandir-nos a la dreta
                rightAligned = false;
            }
        }
        else
        {
            if (m_rightScreenID == -1)
            {
                // Tenim pantalla per expandir-nos a l'esquerra
                rightAligned = true;
            }
            else
            {
                // Tenim pantalles a esquerra i dreta. Cal escollir quina és la més adequada.
                // Opció 1) La pantalla amb la mateixa alçada o major per mantenir aspecte
                ScreenLayout screenLayout = ScreenManager().getScreenLayout();
                QRect leftScreenGeometry = screenLayout.getScreen(m_leftScreenID).getGeometry();
                QRect rightScreenGeometry = screenLayout.getScreen(m_rightScreenID).getGeometry();

                if (leftScreenGeometry.height() >= rightScreenGeometry.height())
                {
                    rightAligned = true;
                }
                else
                {
                    rightAligned = false;
                }
            }
        }
    }

    return rightAligned;
}

void PatientBrowserMenu::computeListOutsideSize(const QPoint &popupPoint, QSize &out, bool rightAligned)
{
    // Obtenim la geometria de la pantalla on se'ns ha demanat obrir el menú
    ScreenLayout screenLayout = ScreenManager().getScreenLayout();
    QRect currentScreenGeometry = screenLayout.getScreen(m_currentScreenID).getAvailableGeometry();
    QPoint currentScreenGlobalOriginPoint = currentScreenGeometry.topLeft();

    // Calculem les mides dels widgets per saber on els hem de col·locar
    // TODO Aquestes mesures no són les més exactes. Les adequades haurien de basar-se en els valors de QWidget::frameGeometry()
    // tal com s'explica a http://doc.trolltech.com/4.7/application-windows.html#window-geometry
    // El problema és que cal que abans haguem fet un show() per saber les mides de veritat. Una opció també seria implementar
    // aquest posicionament quan es produeix un resize d'aquests widgets. Aquesta és una de les raons per la qual veiem el menú
    // adicional amb una petita ombra quan es queda a la dreta del principal.
    int mainMenuApproximateWidth = m_patientBrowserList->sizeHint().width();
    int wholeMenuApproximateHeight = qMax(m_patientBrowserList->sizeHint().height(), m_patientAdditionalInfo->sizeHint().height());

    // Calculem les fronteres per on ens podria sortir el menú (lateral dret/esquerre)
    if (rightAligned)
    {
        int globalRight = currentScreenGlobalOriginPoint.x() + currentScreenGeometry.width();
        int widgetRight = mainMenuApproximateWidth + popupPoint.x();
        out.setWidth(widgetRight - globalRight);
    }
    else
    {
        int globalLeft = currentScreenGlobalOriginPoint.x();
        int widgetLeft = popupPoint.x();
        out.setWidth(widgetLeft - globalLeft);
    }
    
    // Calculem les fronteres per on ens podria sortir el menú (alçada)
    int globalBottom = currentScreenGlobalOriginPoint.y() + currentScreenGeometry.height();
    int widgetBottom = wholeMenuApproximateHeight + popupPoint.y();
    out.setHeight(widgetBottom - globalBottom);
}

void PatientBrowserMenu::placeAdditionalInfoWidget()
{
    ScreenLayout screenLayout = ScreenManager().getScreenLayout();
    QRect currentScreenGeometry = screenLayout.getScreen(m_currentScreenID).getAvailableGeometry();
    QPoint currentScreenGlobalOriginPoint = currentScreenGeometry.topLeft();

    // TODO Aquesta mesura no és la més exacta. L'adequada hauria de basar-se en els valors de QWidget::frameGeometry()
    int mainMenuApproximateWidth = m_patientBrowserList->sizeHint().width();
    int menuXPosition = m_patientBrowserList->pos().x();
    int menuYPosition = m_patientBrowserList->pos().y();
    // Cal fer un adjust size abans perquè les mides del widget quedin actualitzades correctament
    m_patientAdditionalInfo->adjustSize();
    int additionalInfoWidgetApproximateWidth = m_patientAdditionalInfo->frameGeometry().width();

    int menuXShift = 0;
    if (menuXPosition + mainMenuApproximateWidth + additionalInfoWidgetApproximateWidth > currentScreenGlobalOriginPoint.x() + currentScreenGeometry.width())
    {
        // En cas que la combinació de menús en sí, ja ocupi tota la pantalla comprobem en quina pantalla és millor col·locar el menú addicional
        if (mainMenuApproximateWidth + additionalInfoWidgetApproximateWidth >= currentScreenGlobalOriginPoint.x() + currentScreenGeometry.width())
        {
            if (m_leftScreenID != -1)
            {
                // A l'esquerra
                menuXShift = -additionalInfoWidgetApproximateWidth;
            }
            else if (m_rightScreenID != -1)
            {
                // A la dreta
                menuXShift = mainMenuApproximateWidth;
            }
            else
            {
                // No tenim pantalles ni a esquerre ni a dreta, li indiquem a l'esquerra
                menuXShift = -additionalInfoWidgetApproximateWidth;
            }
        }
        else
        {
            // Tenim espai a l'esquerra, dins de la mateixa pantalla
            menuXShift = -additionalInfoWidgetApproximateWidth;
        }
    }
    else
    {
        // Tenim espai a la dreta, dins de la mateixa pantalla
        menuXShift = mainMenuApproximateWidth;
    }

    m_patientAdditionalInfo->move(menuXPosition + menuXShift, menuYPosition);
}

void PatientBrowserMenu::processSelectedItem(const QString &identifier)
{
    m_patientAdditionalInfo->hide();
    m_patientBrowserList->hide();

    if (m_patientBrowserList->getMarkedItem() != identifier)
    {
        if (!identifier.contains("+"))
        {
            Identifier id(identifier.toInt());
            emit selectedVolume(VolumeRepository::getRepository()->getVolume(id));
        }
        else
        {
            QList<Volume*> volumes;
            foreach (const QString &id, identifier.split("+"))
            {
                volumes << VolumeRepository::getRepository()->getVolume(Identifier(id.toInt()));
            }
            emit selectedVolumes(volumes);
        }
    }
}

void PatientBrowserMenu::createWidgets()
{
    if (m_patientAdditionalInfo)
    {
        delete m_patientAdditionalInfo;
    }

    if (m_patientBrowserList)
    {
        delete m_patientBrowserList;
    }

    m_patientAdditionalInfo = new PatientBrowserMenuExtendedInfo(this);
    m_patientBrowserList = new PatientBrowserMenuList(this);

    m_patientAdditionalInfo->setWindowFlags(Qt::Popup);
    m_patientBrowserList->setWindowFlags(Qt::Popup);

    connect(m_patientAdditionalInfo, SIGNAL(closed()), m_patientBrowserList, SLOT(close()));
    connect(m_patientBrowserList, SIGNAL(closed()), m_patientAdditionalInfo, SLOT(close()));
}

}
