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

#include <iostream>

#include <QWidget>
#include <QMessageBox>

#include "qdicomprintextension.h"
#include "qdicomprinterconfigurationwidget.h"
#include "dicomprintermanager.h"
#include "dicomprinter.h"
#include "dicomprintpage.h"
#include "dicomprintjob.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "volume.h"
#include "q2dviewer.h"
#include "starviewerapplication.h"
#include "toolmanager.h"
#include "voilutpresetstooldata.h"

// TODO: Ouch! SuperGuarrada (tm). Per poder fer sortir el menú i tenir accés al Patient principal. S'ha d'arreglar en quan es tregui les dependències
// de interface, pacs, etc.etc.!!
#include "../interface/qapplicationmainwindow.h"

namespace udg {

QDicomPrintExtension::QDicomPrintExtension(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    // Inicialitzem els settings

    m_noSupportedSeriesFrame->setVisible(false);
    fillSelectedDicomPrinterComboBox();

    m_qDicomPrinterConfigurationWidgetProof = new QDicomPrinterConfigurationWidget();

    m_qTimer = new QTimer();
    m_sentToPrintSuccessfullyFrame->setVisible(false);

    createConnections();
    configureInputValidator();
    initializeViewerTools();

    m_lastIDGroupedDICOMImagesToPrint = 0;
    m_thumbnailsPreviewWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_voiLutComboBox->setToolTip(tr("Choose a VOI LUT preset"));
}

void QDicomPrintExtension::createConnections()
{
    // TODO: no cal invocar un mètode per mostrar la configuració d'una impressora el mètode show dels widgets ja és un slot, llavors el mètode
    // configurationPrinter() es pot esborrar
    connect(m_configurationPrinterToolButton, SIGNAL(clicked()), m_qDicomPrinterConfigurationWidgetProof, SLOT(show()));
    connect(m_selectedPrinterComboBox, SIGNAL(currentIndexChanged(int)), SLOT(selectedDicomPrinterChanged(int)));
    connect(m_qDicomPrinterConfigurationWidgetProof, SIGNAL(printerSettingsChanged()), SLOT(fillSelectedDicomPrinterComboBox()));
    connect(m_selectionImageRadioButton, SIGNAL(clicked()), SLOT(imageSelectionModeChanged()));
    connect(m_currentImageRadioButton, SIGNAL(clicked()), SLOT(imageSelectionModeChanged()));
    // Sliders quan canvia de valor
    connect(m_intervalImagesSlider, SIGNAL(valueChanged(int)), SLOT(m_intervalImagesSliderValueChanged(int)));
    connect(m_fromImageSlider, SIGNAL(valueChanged(int)), SLOT(m_fromImageSliderValueChanged()));
    connect(m_toImageSlider, SIGNAL(valueChanged(int)), SLOT(m_toImageSliderValueChanged()));
    // LineEdit canvien de valor
    connect(m_intervalImagesLineEdit, SIGNAL(textEdited (const QString&)), SLOT(m_intervalImagesLineEditTextEdited(const QString&)));
    connect(m_fromImageLineEdit, SIGNAL(textEdited (const QString&)), SLOT(m_fromImageLineEditTextEdited(const QString&)));
    connect(m_toImageLineEdit, SIGNAL(textEdited (const QString&)), SLOT(m_toImageLineEditTextEdited(const QString&)));

    connect(m_qdicomPrinterBasicSettingsWidget, SIGNAL(basicDicomPrinterSettingChanged()), SLOT(updateNumberOfDicomPrintPagesToPrint()));
    connect(m_printButton, SIGNAL(clicked()), SLOT(print()));
    connect(m_addToPrintButton, SIGNAL(clicked()), SLOT(addSelectedImagesToGroupedDICOMImagesToPrint()));
    connect(m_clearDICOMImagesSelectionToPrintButton, SIGNAL(clicked()), SLOT(clearDICOMImagesGroupedToPrint()));
    connect(m_removeSelectedDICOMImagesSelectionToPrintButton, SIGNAL(clicked()), SLOT(removeGroupedDICOMImagesToPrintSelectedInThumbnailsPreview()));

    //connect(m_2DView, SIGNAL(eventReceived(unsigned long)), SLOT(strokeEventHandler(unsigned long)));
    connect(m_sliceViewSlider, SIGNAL(valueChanged(int)), m_2DView, SLOT(setSlice(int)));
    connect(m_2DView, SIGNAL(sliceChanged(int)), m_sliceViewSlider, SLOT(setValue(int)));
    connect(m_2DView, SIGNAL(volumeChanged(Volume*)), this, SLOT(updateInput()));

    connect(m_qTimer, SIGNAL(timeout()), SLOT(timeoutTimer()));
}

void QDicomPrintExtension::configureInputValidator()
{
    m_intervalImagesLineEdit->setValidator(new QIntValidator(0, 99999, m_intervalImagesLineEdit));
    m_fromImageLineEdit->setValidator(new QIntValidator(0, 99999, m_fromImageLineEdit));
    m_toImageLineEdit->setValidator(new QIntValidator(0, 99999, m_toImageLineEdit));
}

void QDicomPrintExtension::initializeViewerTools()
{
    // Creem el tool manager
    m_toolManager = new ToolManager(this);
    // Obtenim les accions de cada tool que volem
    m_toolManager->registerTool("SlicingMouseTool");
    m_toolManager->registerTool("WindowLevelTool");
    m_toolManager->registerTool("VoiLutPresetsTool");
    m_toolManager->registerTool("SlicingKeyboardTool");
    m_toolManager->registerTool("SlicingWheelTool");

    m_restoreToolButton->setDefaultAction(m_toolManager->registerActionTool("RestoreActionTool"));
    m_toolManager->enableRegisteredActionTools(m_2DView);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "VoiLutPresetsTool" << "SlicingKeyboardTool" << "SlicingMouseTool" << "SlicingWheelTool" << "WindowLevelTool";
    m_toolManager->triggerTools(defaultTools);
    m_toolManager->setupRegisteredTools(m_2DView);
}

void QDicomPrintExtension::setInput(Volume *input)
{
    m_2DView->setInput(input);
}

void QDicomPrintExtension::updateInput()
{
    m_sliceViewSlider->setMinimum(0);
    m_sliceViewSlider->setMaximum(m_2DView->getMaximumSlice());
    m_sliceViewSlider->setValue(0);

    m_intervalImagesSlider->setValue(1);

    resetAndUpdateSelectionImagesValue();

    updateVolumeSupport();

    VoiLutPresetsToolData *voiLutData = m_2DView->getVoiLutData();
    m_voiLutComboBox->setPresetsData(voiLutData);
}

void QDicomPrintExtension::fillSelectedDicomPrinterComboBox()
{
    DicomPrinterManager dicomPrinterManager;
    bool noDefaultPrinter = true;

    m_selectedPrinterComboBox->clear();

    foreach (DicomPrinter dicomPrinter, dicomPrinterManager.getDicomPrinterList())
    {
        QString comboBoxText = dicomPrinter.getAETitle();

        if (dicomPrinter.getDescription().length() > 0)
        {
            comboBoxText += " - " + dicomPrinter.getDescription();
        }

        m_selectedPrinterComboBox->addItem(comboBoxText, dicomPrinter.getID());

        if (dicomPrinter.getIsDefault())
        {
            // Si és impressora per defecte la deixem com a impressora seleccionada
            m_selectedPrinterComboBox->setCurrentIndex(m_selectedPrinterComboBox->findData(dicomPrinter.getID()));
            selectedDicomPrinterChanged(m_selectedPrinterComboBox->findData(dicomPrinter.getID()));
            noDefaultPrinter = false;
        }
    }

    if (m_selectedPrinterComboBox->count() > 0)
    {
        // Si no hi ha cap impressora per defecte fem que quedi seleccionada la primera de la llista.
        if (noDefaultPrinter)
        {
            m_selectedPrinterComboBox->setCurrentIndex(0);
            selectedDicomPrinterChanged(0);
        }
    }
    else
    {
        selectedDicomPrinterChanged(-1);
    }
}

void QDicomPrintExtension::print()
{
    if (getImagesToPrint().count() == 0)
    {
        QMessageBox::warning(this, ApplicationNameString, tr("No added images to print."));
        return;
    }

    DicomPrint dicomPrint;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int printedPages = dicomPrint.print(getSelectedDicomPrinter(), getDicomPrintJobToPrint());
    QApplication::restoreOverrideCursor();

    if (dicomPrint.getLastError() != DicomPrint::Ok)
    {
        // Si hem imprés una o més pàgines i hi ha error vol dir que han quedat algunes pàgines per imprimir
        showDicomPrintError(dicomPrint.getLastError(), printedPages > 0);
    }
    else
    {
        clearDICOMImagesGroupedToPrint();
        m_sentToPrintSuccessfullyFrame->setVisible(true);
        // Engeguem timer per a que d'aquí 20 segons s'amagui el frame indicant que s'han enviat a imprimir correctament les imatges
        m_qTimer->start(20000);
    }
}

void QDicomPrintExtension::timeoutTimer()
{
    m_sentToPrintSuccessfullyFrame->setVisible(false);
}

void QDicomPrintExtension::addSelectedImagesToGroupedDICOMImagesToPrint()
{
    GroupedDICOMImagesToPrint selectedGroupedDICOMImagesToPrint;

    m_lastIDGroupedDICOMImagesToPrint++;
    selectedGroupedDICOMImagesToPrint.ID = m_lastIDGroupedDICOMImagesToPrint;
    selectedGroupedDICOMImagesToPrint.imagesToPrint = getSelectedImagesToAddToPrint();
    selectedGroupedDICOMImagesToPrint.dicomPrintPresentationStateImage = getDICOMPrintPresentationStateImageForCurrentSelectedImages();
    m_groupedDICOMImagesToPrintList.append(selectedGroupedDICOMImagesToPrint);

    Image *firstSelectedImage = getSelectedImagesToAddToPrint().constFirst();
    m_thumbnailsPreviewWidget->append(QString::number(m_lastIDGroupedDICOMImagesToPrint), firstSelectedImage->getThumbnail(), getThumbnailPreviewDescriptionOfSelectedGroupedDICOMImagesToPrint());

    updateNumberOfDicomPrintPagesToPrint();
}

void QDicomPrintExtension::clearDICOMImagesGroupedToPrint()
{
    m_thumbnailsPreviewWidget->clear();
    m_groupedDICOMImagesToPrintList.clear();
    m_lastIDGroupedDICOMImagesToPrint = 0;

    updateNumberOfDicomPrintPagesToPrint();
}

void QDicomPrintExtension::removeGroupedDICOMImagesToPrintSelectedInThumbnailsPreview()
{
    foreach(QString thumbnailID, m_thumbnailsPreviewWidget->getSelectedThumbnailsID())
    {
        m_thumbnailsPreviewWidget->remove(thumbnailID);
        removeGroupedDICOMImagesToPrint(thumbnailID.toInt());
    }

    updateNumberOfDicomPrintPagesToPrint();
}

DicomPrintJob QDicomPrintExtension::getDicomPrintJobToPrint()
{
    DicomPrintJob dicomPrintJob;
    DicomPrinter selectedDicomPrinter = getSelectedDicomPrinter();

    dicomPrintJob.setDicomPrintPages(getDicomPrintPageListToPrint());
    dicomPrintJob.setFilmDestination(selectedDicomPrinter.getDefaultFilmDestination());
    dicomPrintJob.setLabel("");
    dicomPrintJob.setMediumType(selectedDicomPrinter.getDefaultMediumType());
    dicomPrintJob.setNumberOfCopies(m_numberOfCopiesSpinBox->value());
    dicomPrintJob.setPrintPriority(selectedDicomPrinter.getDefaultPrintPriority());

    return dicomPrintJob;
}

QList<DicomPrintPage> QDicomPrintExtension::getDicomPrintPageListToPrint()
{
    QList<QPair<Image*, DICOMPrintPresentationStateImage> > imagesToPrintWithPresentationState = getImagesToPrint();
    QList<DicomPrintPage> dicomPrintPageList;
    DicomPrinter dicomPrinter = getSelectedDicomPrinter();
    int numberOfImagesPerPage = dicomPrinter.getDefaultFilmLayoutColumns() * dicomPrinter.getDefaultFilmLayoutRows();
    int numberOfPage = 1;

    while (!imagesToPrintWithPresentationState.isEmpty())
    {
        int indexOfImagePerPage = 0;
        QList<QPair<Image*, DICOMPrintPresentationStateImage> > imagesPageList;
        DicomPrintPage dicomPrintPage = fillDicomPrintPagePrintSettings(dicomPrinter);

        // TODO:No tinc clar que això haig de ser responsabilitat de la Interfície emplenar les anotacions
        if (dicomPrinter.getSupportsAnnotationBox())
        {
            addAnnotationsToDicomPrintPage(&dicomPrintPage, imagesToPrintWithPresentationState.at(0).first);
        }

        dicomPrintPage.setPageNumber(numberOfPage);
        // Emplenen una dicomPrintPage amb les imatges en funció del número d'imatges que hi caben
        while (indexOfImagePerPage < numberOfImagesPerPage && !imagesToPrintWithPresentationState.isEmpty())
        {
            imagesPageList.append(imagesToPrintWithPresentationState.takeFirst());
            indexOfImagePerPage++;
        }

        dicomPrintPage.setImagesToPrint(imagesPageList);
        dicomPrintPageList.append(dicomPrintPage);
        numberOfPage++;
    }

    return dicomPrintPageList;
}

QList<Image*> QDicomPrintExtension::getSelectedImagesToAddToPrint() const
{
    QList<Image*> imagesToPrint, imagesVolum = m_2DView->getMainInput()->getImages();

    if (m_currentImageRadioButton->isChecked())
    {
        imagesToPrint.append(m_2DView->getMainInput()->getImage(m_2DView->getCurrentSlice(), m_2DView->getCurrentPhase()));
    }
    else
    {
        int indexOfImage = m_fromImageSlider->value() - 1;

        while (indexOfImage < m_toImageSlider->value())
        {
            imagesToPrint.append(imagesVolum.at(indexOfImage));
            indexOfImage += m_intervalImagesSlider->value();
        }
    }
    return imagesToPrint;
}

int QDicomPrintExtension::getNumberOfPagesToPrint() const
{
    int numberOfDicomPrintPagesToPrint = 0, numberOfImagesPerPage, numberOfImagesToPrint;
    DicomPrinter selectedDicomPrinter = getSelectedDicomPrinter();

    numberOfImagesToPrint = getImagesToPrint().count();

    numberOfImagesPerPage = selectedDicomPrinter.getDefaultFilmLayoutRows() * selectedDicomPrinter.getDefaultFilmLayoutColumns();

    if (numberOfImagesToPrint > 0 && numberOfImagesPerPage > 0)
    {
        numberOfDicomPrintPagesToPrint = numberOfImagesToPrint / numberOfImagesPerPage;
        // Si tenim residu hem d'incrementar en una el número de pàgines
        numberOfDicomPrintPagesToPrint += numberOfImagesToPrint % numberOfImagesPerPage > 0 ? 1 : 0;
    }

    return numberOfDicomPrintPagesToPrint;
}

DicomPrintPage QDicomPrintExtension::fillDicomPrintPagePrintSettings(const DicomPrinter &dicomPrinter) const
{
    DicomPrintPage dicomPrintPage;

    dicomPrintPage.setBorderDensity(dicomPrinter.getDefaultBorderDensity());
    dicomPrintPage.setEmptyImageDensity(dicomPrinter.getDefaultEmptyImageDensity());
    dicomPrintPage.setFilmLayout(dicomPrinter.getDefaultFilmLayout());
    dicomPrintPage.setFilmOrientation(dicomPrinter.getDefaultFilmOrientation());
    dicomPrintPage.setFilmSize(dicomPrinter.getDefaultFilmSize());
    dicomPrintPage.setMagnificationType(dicomPrinter.getDefaultMagnificationType());
    dicomPrintPage.setMaxDensity(dicomPrinter.getDefaultMaxDensity());
    dicomPrintPage.setMinDensity(dicomPrinter.getDefaultMinDensity());
    dicomPrintPage.setPolarity(dicomPrinter.getDefaultPolarity());
    dicomPrintPage.setSmoothingType(dicomPrinter.getDefaultSmoothingType());
    dicomPrintPage.setTrim(dicomPrinter.getDefaultTrim());
    dicomPrintPage.setConfigurationInformation(dicomPrinter.getDefaultConfigurationInformation());

    return dicomPrintPage;
}

void QDicomPrintExtension::addAnnotationsToDicomPrintPage(DicomPrintPage *dicomPrintPage, Image *imageToPrint)
{
    QDate dateToPrintInAnnotation;
    QTime timeToPrintInAnnotation;
    Series *seriesToPrint = imageToPrint->getParentSeries();
    Study *studyToPrint = seriesToPrint->getParentStudy();

    if (studyToPrint->getDate().isValid())
    {
        dateToPrintInAnnotation = studyToPrint->getDate();
        timeToPrintInAnnotation = studyToPrint->getTime();
    }
    else if (seriesToPrint->getDate().isValid())
    {
        //Si l'estudi no té data/hora posem la de la sèrie, val més posar la de la sèrie encara que en una pàgina podem tenir diverses sèries
        //que no posar-ne cap
        dateToPrintInAnnotation = seriesToPrint->getDate();
        timeToPrintInAnnotation = seriesToPrint->getTime();
    }

    // A la primera posicio: posem el nom de la institució que ha generat l'estudi
    dicomPrintPage->addAnnotation(1, seriesToPrint->getInstitutionName());
    // A la segona posició: el nom del, edat i sexe pacient
    dicomPrintPage->addAnnotation(2, studyToPrint->getParentPatient()->getFullName() + " " + studyToPrint->getPatientAge() + " " + studyToPrint->getParentPatient()->getSex());
    /// A la tercera posició: Modalitat seriesi Data/hora (de la sèrie i si no en té de l'estudi)
    dicomPrintPage->addAnnotation(3, seriesToPrint->getModality() + " " + dateToPrintInAnnotation.toString("dd/MM/yyyy") + " " + timeToPrintInAnnotation.toString("hh:mm:ss"));
    /// Quarta posició: Descripció estudi
    dicomPrintPage->addAnnotation(4, studyToPrint->getDescription());
    /// Cinquena posició: Patient ID + Acession number (El patientID l'han demanat els metges perquè és amb el camp que poden cercar els pacients en el SAP)
    dicomPrintPage->addAnnotation(5, studyToPrint->getParentPatient()->getID() + " " + studyToPrint->getAccessionNumber());
}

QString QDicomPrintExtension::getThumbnailPreviewDescriptionOfSelectedGroupedDICOMImagesToPrint() const
{
    Series *seriesParentImagesToPrint;
    seriesParentImagesToPrint = getSelectedImagesToAddToPrint().at(0)->getParentSeries();

    QString thumbnailDescription;
    thumbnailDescription = tr("Series ") + seriesParentImagesToPrint->getSeriesNumber() + "\n";

    if (getSelectedImagesToAddToPrint().count() == seriesParentImagesToPrint->getImages().count())
    {
        thumbnailDescription += tr("All images");
    }
    else if (m_currentImageRadioButton->isChecked())
    {
        Image *currentImage = getSelectedImagesToAddToPrint().at(0);
        thumbnailDescription += tr("image %1").arg(currentImage->getInstanceNumber());
    }
    else
    {
        if (m_intervalImagesSlider->value() > 1)
        {
            thumbnailDescription += tr("Every %1 images").arg(m_intervalImagesSlider->value());
        }

        if (m_fromImageSlider->value() != 1 || m_toImageSlider->value() != m_toImageSlider->maximum())
        {
            thumbnailDescription += tr(" from %1").arg(m_fromImageSlider->value());
            thumbnailDescription += tr(" to %1").arg(m_toImageSlider->value());
        }
    }

    return thumbnailDescription;
}

void QDicomPrintExtension::removeGroupedDICOMImagesToPrint(int IDGroup)
{
    int index = 0;
    bool notFound = true;

    while (index < m_groupedDICOMImagesToPrintList.count() && notFound)
    {
        if (m_groupedDICOMImagesToPrintList.at(index).ID == IDGroup)
        {
            m_groupedDICOMImagesToPrintList.removeAt(index);
            notFound = false;
        }

        index++;
    }
}

QList<QPair<Image*, DICOMPrintPresentationStateImage> > QDicomPrintExtension::getImagesToPrint() const
{
    QList<QPair<Image*, DICOMPrintPresentationStateImage> > imagesToPrint;

    foreach(GroupedDICOMImagesToPrint groupedDICOMImagesToPrint, m_groupedDICOMImagesToPrintList)
    {
        foreach(Image *image, groupedDICOMImagesToPrint.imagesToPrint)
        {
            QPair <Image*, DICOMPrintPresentationStateImage> imageWithPresentationState;
            imageWithPresentationState.first = image;
            imageWithPresentationState.second = groupedDICOMImagesToPrint.dicomPrintPresentationStateImage;

            imagesToPrint.append(imageWithPresentationState);
        }
    }

    return imagesToPrint;
}

DICOMPrintPresentationStateImage QDicomPrintExtension::getDICOMPrintPresentationStateImageForCurrentSelectedImages() const
{
    // TODO support VOI LUTs
    DICOMPrintPresentationStateImage dicomPrintPresentationStateImage;
    WindowLevel windowLevelFromViewer = m_2DView->getCurrentVoiLut().getWindowLevel();

    // Tenir en compte que imatges dins un mateixa sèrie poden tenir WL
    Image *currentImageInViewer = m_2DView->getMainInput()->getImage(m_2DView->getCurrentSlice(), m_2DView->getCurrentPhase());
    bool windowLevelHasBeenModifiedInViewer = currentImageInViewer->getVoiLut().getWindowLevel().getWidth() != windowLevelFromViewer.getWidth() ||
            currentImageInViewer->getVoiLut().getWindowLevel().getCenter() != windowLevelFromViewer.getCenter();

    //Si el WL no ha estat modificat per defecte s'aplicarà el que té cada imatge
    if (windowLevelHasBeenModifiedInViewer)
    {
        dicomPrintPresentationStateImage.setWindowLevel(windowLevelFromViewer.getWidth(), windowLevelFromViewer.getCenter());
    }

    return dicomPrintPresentationStateImage;
}

void QDicomPrintExtension::selectedDicomPrinterChanged(int indexOfSelectedDicomPrinter)
{
    m_qdicomPrinterBasicSettingsWidget->clear();

    if (indexOfSelectedDicomPrinter >= 0)
    {
        DicomPrinter selectedDicomPrinter = DicomPrinterManager().getPrinterByID(m_selectedPrinterComboBox->itemData(indexOfSelectedDicomPrinter).toInt());

        m_hostNameLabel->setText(selectedDicomPrinter.getHostname());
        m_portLabel->setText(QString().setNum(selectedDicomPrinter.getPort()));

        m_qdicomPrinterBasicSettingsWidget->setDicomPrinterBasicSettings(selectedDicomPrinter);

        setEnabledPrintControls(true);
        // Només ho habilitarem si la serie se suporta
        if (!m_noSupportedSeriesFrame->isVisible())
        {
            setEnabledAddImagesToPrintControls(true);
        }
    }
    else
    {
        m_hostNameLabel->setText("");
        m_portLabel->setText("");

        setEnabledPrintControls(false);
        setEnabledAddImagesToPrintControls(false);
    }
}

void QDicomPrintExtension::imageSelectionModeChanged()
{
    m_selectionImagesFrame->setEnabled(m_selectionImageRadioButton->isChecked());
}

void QDicomPrintExtension::m_intervalImagesSliderValueChanged(int value)
{
    m_intervalImagesLineEdit->setText(QString().setNum(value));
    updateNumberOfDicomPrintPagesToPrint();
}

void QDicomPrintExtension::m_fromImageSliderValueChanged()
{
    /// Fem que no es puguin solapar els sliders
    if (m_fromImageSlider->value() > m_toImageSlider->value())
    {
        m_fromImageSlider->setValue(m_toImageSlider->value());
    }

    m_fromImageLineEdit->setText(QString().setNum(m_fromImageSlider->value()));
    updateNumberOfDicomPrintPagesToPrint();
}

void QDicomPrintExtension::m_toImageSliderValueChanged()
{
    /// Fem que no es puguin solapar els sliders
    if (m_toImageSlider->value() < m_fromImageSlider->value())
    {
        m_toImageSlider->setValue(m_fromImageSlider->value());
    }

    m_toImageLineEdit->setText(QString().setNum(m_toImageSlider->value()));
    updateNumberOfDicomPrintPagesToPrint();
}

void QDicomPrintExtension::m_intervalImagesLineEditTextEdited(const QString &text)
{
    int value = text.toInt();

    if (value > m_intervalImagesSlider->maximum())
    {
        m_intervalImagesSlider->setValue(m_intervalImagesSlider->maximum());
        m_intervalImagesLineEdit->setText(QString().setNum(m_intervalImagesSlider->maximum()));
    }
    else
    {
        m_intervalImagesSlider->setValue(value);
    }

    updateNumberOfDicomPrintPagesToPrint();
}

void QDicomPrintExtension::m_fromImageLineEditTextEdited(const QString &text)
{
    int value = text.toInt();

    if (value > m_fromImageSlider->maximum())
    {
        m_fromImageSlider->setValue(m_fromImageSlider->maximum());
        m_fromImageLineEdit->setText(QString().setNum(m_fromImageSlider->maximum()));
    }
    else
    {
        m_fromImageSlider->setValue(value);
    }
}

void QDicomPrintExtension::m_toImageLineEditTextEdited(const QString &text)
{
    int value = text.toInt();

    if (value > m_toImageSlider->maximum())
    {
        m_toImageSlider->setValue(m_toImageSlider->maximum());
        m_toImageLineEdit->setText(QString().setNum(m_toImageSlider->maximum()));
    }
    else
    {
        m_toImageSlider->setValue(value);
    }

    updateNumberOfDicomPrintPagesToPrint();
}

void QDicomPrintExtension::updateNumberOfDicomPrintPagesToPrint()
{
    int numberOfDicomPrintPagesToPrint = getNumberOfPagesToPrint();

    if (numberOfDicomPrintPagesToPrint >= 0)
    {
        m_pagesToPrintLabel->setText(QString().setNum(numberOfDicomPrintPagesToPrint));
    }
    else
    {
        m_pagesToPrintLabel->setText("N/A");
    }
}

void QDicomPrintExtension::resetAndUpdateSelectionImagesValue()
{
    int tickInterval;
    int numberOfImagesOfVolume = m_2DView->getMainInput()->getImages().count();

    m_intervalImagesSlider->setValue(1);

    m_fromImageSlider->setMaximum(numberOfImagesOfVolume);
    m_fromImageSlider->setValue(1);

    m_toImageSlider->setMaximum(numberOfImagesOfVolume);
    m_toImageSlider->setValue(numberOfImagesOfVolume);
    m_toImageLineEdit->setText(QString().setNum(numberOfImagesOfVolume));

    if (numberOfImagesOfVolume < 10)
    {
        tickInterval = 1;
    }
    else if (numberOfImagesOfVolume < 50)
    {
        tickInterval = 2;
    }
    else if (numberOfImagesOfVolume < 100)
    {
        tickInterval = 5;
    }
    else if (numberOfImagesOfVolume < 400)
    {
        tickInterval = 10;
    }
    else
    {
        tickInterval = 20;
    }

    m_toImageSlider->setTickInterval(tickInterval);
    m_fromImageSlider->setTickInterval(tickInterval);

    updateNumberOfDicomPrintPagesToPrint();
}

DicomPrinter QDicomPrintExtension::getSelectedDicomPrinter() const
{
    DicomPrinter dicomPrinter;
    int indexOfSelectedDicomPrinter = m_selectedPrinterComboBox->currentIndex();

    dicomPrinter = DicomPrinterManager().getPrinterByID(m_selectedPrinterComboBox->itemData(indexOfSelectedDicomPrinter).toInt());

    // Sobreescrivim els settings que formen part configuració bàsica de la impressora amb els de control QDicomPrinterBasicSettings
    m_qdicomPrinterBasicSettingsWidget->getDicomPrinterBasicSettings(dicomPrinter);

    return dicomPrinter;
}

void QDicomPrintExtension::setEnabledPrintControls(bool enable)
{
    m_printButton->setEnabled(enable);
    m_qdicomPrinterBasicSettingsWidget->setEnabled(enable);
}

void QDicomPrintExtension::setEnabledAddImagesToPrintControls(bool enable)
{
    m_selectionImagesFrame->setEnabled(enable);
    m_currentImageRadioButton->setEnabled(enable);
    m_selectionImageRadioButton->setEnabled(enable);
    m_addToPrintButton->setEnabled(enable);

    // Si ens indiquen que activem els controls d'impressió però tenim el checkbox d'imprimir només pàgina actual el frame
    // per seleccionar les imatges a imprimir el desactivem, no té sentit que estigui activat
    if (enable && m_currentImageRadioButton->isChecked())
    {
        m_selectionImagesFrame->setEnabled(false);
    }
}

void QDicomPrintExtension::showDicomPrintError(DicomPrint::DicomPrintError error, bool printedSomePage)
{
    QString messageError;

    if (error != DicomPrint::Ok)
    {
        if (printedSomePage)
        {
            messageError = tr("Some of the film pages cannot be printed because ");
        }
        else
        {
            messageError = tr("The images cannot be printed because ");
        }

        switch (error)
        {
            case DicomPrint::CanNotConnectToDicomPrinter:
                messageError += tr("the printer does not respond.");
                messageError += "\n\n";
                messageError += tr("Be sure the computer is connected on the network and the printer network parameters are correct.");
                break;
            case DicomPrint::ErrorSendingDicomPrintJob:
                messageError += tr("the printer does not respond as expected.");
                messageError += "\n\n";
                messageError += tr("In most cases this error is produced because the printer does not support some of the print configuration parameters.");
                messageError += tr("Check printer DICOM Conformance to be sure that it accepts all your print parameters.");
                break;
            case DicomPrint::ErrorCreatingPrintSpool:
                messageError += tr("Unable to create print spool.");
                break;
            case DicomPrint::ErrorLoadingImagesToPrint:
                messageError += tr("Unable to load some of the images to print.");
                messageError += "\n\n";
                messageError += tr("Close 'DICOM print' tab and try it again.");
                break;
            case DicomPrint::UnknowError:
            default:
                messageError += tr("an unknown error has occurred.");
                break;
        }

        QMessageBox::critical(this, ApplicationNameString, messageError);
    }
}

void QDicomPrintExtension::updateVolumeSupport()
{
    // Comprovem si té color.
    if (m_2DView->getMainInput()->getImage(0)->getPhotometricInterpretation().isColor())
    {
        m_noSupportedSeriesMissage->setText(tr("This series cannot be added to print because color is not supported."));
        m_noSupportedSeriesFrame->setVisible(true);

        setEnabledAddImagesToPrintControls(false);
    }
    else
    {
        m_noSupportedSeriesFrame->setVisible(false);

        // Només activem les opcions si tenim impressores.
        if (m_selectedPrinterComboBox->count() > 0)
        {
            setEnabledAddImagesToPrintControls(true);
        }
    }
}

}
