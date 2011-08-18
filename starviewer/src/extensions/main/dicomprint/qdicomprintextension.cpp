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
    // Posem a punt les annotacions que volem veure al viewer
    m_2DView->removeAnnotation(Q2DViewer::AllAnnotation);
    m_2DView->enableAnnotation(Q2DViewer::WindowInformationAnnotation | Q2DViewer::PatientOrientationAnnotation | Q2DViewer::SliceAnnotation |
                               Q2DViewer::PatientInformationAnnotation | Q2DViewer::AcquisitionInformationAnnotation, true);

    m_lastIDGroupedDICOMImagesToPrint = 0;
    m_thumbnailsPreviewWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
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
    connect(m_fromImageSlider, SIGNAL(valueChanged(int)), SLOT(m_fromImageSliderValueChanged(int)));
    connect(m_toImageSlider, SIGNAL(valueChanged(int)), SLOT(m_toImageSliderValueChanged(int)));
    // LineEdit canvien de valor
    connect(m_intervalImagesLineEdit, SIGNAL(textEdited (const QString &)), SLOT(m_intervalImagesLineEditTextEdited(const QString &)));
    connect(m_fromImageLineEdit, SIGNAL(textEdited (const QString &)), SLOT(m_fromImageLineEditTextEdited(const QString &)));
    connect(m_toImageLineEdit, SIGNAL(textEdited (const QString &)), SLOT(m_toImageLineEditTextEdited(const QString &)));

    connect(m_qdicomPrinterBasicSettingsWidget, SIGNAL(basicDicomPrinterSettingChanged()), SLOT(updateNumberOfDicomPrintPagesToPrint()));
    connect(m_printButton, SIGNAL(clicked()), SLOT(print()));
    connect(m_addToPrintButton, SIGNAL(clicked()),SLOT(addSelectedImagesToGroupedDICOMImagesToPrint()));
    connect(m_clearDICOMImagesSelectionToPrintButton, SIGNAL(clicked()), SLOT(clearDICOMImagesGroupedToPrint()));
    connect(m_removeSelectedDICOMImagesSelectionToPrintButton, SIGNAL(clicked()), SLOT(removeGroupedDICOMImagesToPrintSelectedInThumbnailsPreview()));

    //connect(m_2DView, SIGNAL(eventReceived(unsigned long)), SLOT(strokeEventHandler(unsigned long)));
    connect(m_sliceViewSlider, SIGNAL(valueChanged(int)), m_2DView, SLOT(setSlice(int)));
    connect(m_2DView, SIGNAL(sliceChanged(int)), m_sliceViewSlider, SLOT(setValue(int)));
    connect(m_2DView, SIGNAL(volumeChanged(Volume *)), this, SLOT(updateInput()));

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
    m_toolManager->registerTool("SlicingTool");
    m_toolManager->registerTool("WindowLevelTool");
    m_toolManager->registerTool("WindowLevelPresetsTool");
    m_toolManager->registerTool("SlicingKeyboardTool");

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "WindowLevelPresetsTool" << "SlicingKeyboardTool" << "SlicingTool" << "WindowLevelTool";
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
        // Només ho habilitarem si la serie se suporta
        if (!m_noSupportedSeriesFrame->isVisible())
        {
            setEnabledPrintControls(true);
        }

        // Si no hi ha cap impressora per defecte fem que quedi seleccionada la primera de la llista.
        if (noDefaultPrinter)
        {
            m_selectedPrinterComboBox->setCurrentIndex(0);
            selectedDicomPrinterChanged(0);
        }
    }
    else
    {
        setEnabledPrintControls(false);
        selectedDicomPrinterChanged(-1);
    }
}

void QDicomPrintExtension::print()
{
    if (getImagesToPrint().count() == 0)
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Please add images to print."));
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
    m_groupedDICOMImagesToPrintList.append(selectedGroupedDICOMImagesToPrint);

    Image *firstSelectedImage = getSelectedImagesToAddToPrint().first();
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
    QList<Image*> imagesToPrint = getImagesToPrint();
    QList<DicomPrintPage> dicomPrintPageList;
    DicomPrinter dicomPrinter = getSelectedDicomPrinter();
    int numberOfImagesPerPage = dicomPrinter.getDefaultFilmLayoutColumns() * dicomPrinter.getDefaultFilmLayoutRows();
    int numberOfPage = 1;

    while (!imagesToPrint.isEmpty())
    {
        int indexOfImagePerPage = 0;
        QList<Image*> imagesPageList;
        DicomPrintPage dicomPrintPage = fillDicomPrintPagePrintSettings(dicomPrinter);

        // TODO:No tinc clar que això haig de ser responsabilitat de la Interfície emplenar les anotacions
        if (dicomPrinter.getSupportsAnnotationBox())
        {
            addSeriesInformationAsAnnotationsToDicomPrintPage(&dicomPrintPage, imagesToPrint.at(0)->getParentSeries());
        }

        dicomPrintPage.setPageNumber(numberOfPage);
        // Emplenen una dicomPrintPage amb les imatges en funció del número d'imatges que hi caben
        while (indexOfImagePerPage < numberOfImagesPerPage && !imagesToPrint.isEmpty())
        {
            imagesPageList.append(imagesToPrint.takeFirst());
            indexOfImagePerPage++;
        }

        dicomPrintPage.setImagesToPrint(imagesPageList);
        dicomPrintPageList.append(dicomPrintPage);
        numberOfPage++;
    }

    return dicomPrintPageList;
}

QList<Image*> QDicomPrintExtension::getSelectedImagesToAddToPrint()
{
    QList<Image*> imagesToPrint, imagesVolum = m_2DView->getInput()->getImages();

    if (m_currentImageRadioButton->isChecked())
    {
        imagesToPrint.append(m_2DView->getInput()->getImage(m_2DView->getCurrentSlice(), m_2DView->getCurrentPhase()));
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

int QDicomPrintExtension::getNumberOfPagesToPrint()
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

DicomPrintPage QDicomPrintExtension::fillDicomPrintPagePrintSettings(DicomPrinter dicomPrinter)
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

void QDicomPrintExtension::addSeriesInformationAsAnnotationsToDicomPrintPage(DicomPrintPage *dicomPrintPage, Series *seriesToPrint)
{
    QDate dateToPrintInAnnotation;
    QTime timeToPrintInAnnotation;

    // Hi ha estudis com el CT del IDI-Girona que no tenen Data/Hora de sèries, sinó en tenen l'agafem de l'estudi
    if (seriesToPrint->getDate().isValid())
    {
        dateToPrintInAnnotation = seriesToPrint->getDate();
        timeToPrintInAnnotation = seriesToPrint->getTime();
    }
    else
    {
        dateToPrintInAnnotation = seriesToPrint->getParentStudy()->getDate();
        timeToPrintInAnnotation = seriesToPrint->getParentStudy()->getTime();
    }

    // A la primera posicio: posem el nom de la institució que ha generat l'estudi
    dicomPrintPage->addAnnotation(1, seriesToPrint->getInstitutionName());
    // A la segona posició: el nom del, edat i sexe pacient
    dicomPrintPage->addAnnotation(2, seriesToPrint->getParentStudy()->getParentPatient()->getFullName() + " " +
                                  seriesToPrint->getParentStudy()->getPatientAge() + " " + seriesToPrint->getParentStudy()->getParentPatient()->getSex());
    /// A la tercera posició: Modalitat seriesi Data/hora (de la sèrie i si no en té de l'estudi)
    dicomPrintPage->addAnnotation(3, seriesToPrint->getModality() + " " + dateToPrintInAnnotation.toString("dd/MM/yyyy") + " " +
                                  timeToPrintInAnnotation.toString("hh:mm:ss"));
    /// Quarta posició: Descripció estudi i descripció serie
    dicomPrintPage->addAnnotation(4, seriesToPrint->getParentStudy()->getDescription() + " - " + seriesToPrint->getDescription());
    /// Cinquena posició: Patient ID + Acession number (El patientID l'han demanat els metges perquè és amb el camp que poden cercar els pacients en el SAP)
    dicomPrintPage->addAnnotation(5, seriesToPrint->getParentStudy()->getParentPatient()->getID() + " " +
                                  seriesToPrint->getParentStudy()->getAccessionNumber());
}

QString QDicomPrintExtension::getThumbnailPreviewDescriptionOfSelectedGroupedDICOMImagesToPrint()
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

QList<Image*> QDicomPrintExtension::getImagesToPrint()
{
    QList<Image*> imagesToPrint;

    foreach(GroupedDICOMImagesToPrint groupedDICOMImagesToPrint, m_groupedDICOMImagesToPrintList)
    {
        imagesToPrint.append(groupedDICOMImagesToPrint.imagesToPrint);
    }

    return imagesToPrint;
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

        // Només ho habilitarem si la serie se suporta
        if (!m_noSupportedSeriesFrame->isVisible())
        {
            setEnabledPrintControls(true);
        }
    }
    else
    {
        m_hostNameLabel->setText("");
        m_portLabel->setText("");

        setEnabledPrintControls(false);
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

void QDicomPrintExtension::m_fromImageSliderValueChanged(int value)
{
    m_fromImageLineEdit->setText(QString().setNum(value));
    updateNumberOfDicomPrintPagesToPrint();
}

void QDicomPrintExtension::m_toImageSliderValueChanged(int value)
{
    m_toImageLineEdit->setText(QString().setNum(value));
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
    int numberOfImagesOfVolume = m_2DView->getInput()->getImages().count();

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

DicomPrinter QDicomPrintExtension::getSelectedDicomPrinter()
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
    m_selectionImagesFrame->setEnabled(enable);
    m_printButton->setEnabled(enable);
    m_currentImageRadioButton->setEnabled(enable);
    m_selectionImageRadioButton->setEnabled(enable);
    m_qdicomPrinterBasicSettingsWidget->setEnabled(enable);
    m_numberOfCopiesSpinBox->setEnabled(enable);

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
            messageError = tr("Some of the pages film can't be printed because ");
        }
        else
        {
            messageError = tr("The images can't be printed because ");
        }

        switch (error)
        {
            case DicomPrint::CanNotConnectToDicomPrinter:
                messageError += tr("the printer doesn't respond.\n\n");
                messageError += tr("Be sure the computer is connected on the network and the printer network parameters are correct.");
                break;
            case DicomPrint::ErrorSendingDicomPrintJob:
                messageError += tr("the printer doesn't respond as expected.\n\n");
                messageError += tr("In most cases this error is produced because the printer doesn't support some of the print configuration parameters.");
                messageError += tr("Check printer DICOM Conformance to be sure that it accepts all your print parameters.");
                break;
            case DicomPrint::ErrorCreatingPrintSpool:
                messageError += tr("%1 can't create print spool.").arg(ApplicationNameString);
                break;
            case DicomPrint::ErrorLoadingImagesToPrint:
                messageError += tr("%1 can't load some of the images to print.\n\n").arg(ApplicationNameString);
                messageError += tr("Close 'DICOM print' tab and try it again.");
                break;
            case DicomPrint::UnknowError:
            default:
                messageError += tr("an unknown error has produced.");
                break;
        }

        QMessageBox::critical(this, ApplicationNameString, messageError);
    }
}

void QDicomPrintExtension::updateVolumeSupport()
{
    // Comprovem si té color.
    QString pi = m_2DView->getInput()->getImage(0)->getPhotometricInterpretation();
    if (pi != "MONOCHROME1" && pi != "MONOCHROME2")
    {
        m_noSupportedSeriesMissage->setText(tr("This series cannot be printed because color is not supported."));
        m_noSupportedSeriesFrame->setVisible(true);

        setEnabledPrintControls(false);
    }
    else
    {
        m_noSupportedSeriesFrame->setVisible(false);

        // Només activem les opcions si tenim impressores.
        if (m_selectedPrinterComboBox->count() > 0)
        {
            setEnabledPrintControls(true);
        }
    }
}

}
