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


#include "qexportertool.h"

#include "qviewer.h"
#include "q2dviewer.h"
#include "volume.h"
#include "volumebuilderfromcaptures.h"
#include "dicomimagefilegenerator.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"
#include "inputoutputsettings.h"
#include "localdatabasemanager.h"
#include "dicommask.h"
#include "queryscreen.h"
#include "singleton.h"
#include "logging.h"
#include <vtkWindowToImageFilter.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPNGWriter.h>

#include <QDateTime>
#include <QMessageBox>
#include <QPixmap>
#include <QProgressDialog>

namespace udg {

QExporterTool::QExporterTool(QViewer *viewer, QWidget *parent)
: QDialog(parent)
{
    Q_ASSERT(viewer);

    setupUi(this);
    m_viewer = viewer;
    createConnections();
    initialize();
}

QExporterTool::~QExporterTool()
{

}

void QExporterTool::createConnections()
{
    connect(m_saveButton, SIGNAL(clicked()), this, SLOT (generateAndStoreNewSeries()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_currentImageRadioButton, SIGNAL(clicked()), this, SLOT(currentImageRadioButtonClicked()));
    connect(m_allImagesRadioButton, SIGNAL(clicked()), this, SLOT(allImagesRadioButtonClicked()));
    connect(m_imagesOfCurrentPhaseRadioButton, SIGNAL(clicked()), this, SLOT(imageOfCurrentPhaseRadioButtonClicked()));
    connect(m_phasesOfCurrentImageRadioButton, SIGNAL(clicked()), this, SLOT(phasesOfCurrentImageRadioButtonClicked()));
}

void QExporterTool::initialize()
{
    // Depenent de si és un visor 2d o no (3D) habilitem unes opcions o unes altres
    Q2DViewer *q2DViewer = Q2DViewer::castFromQViewer(m_viewer);

    if (q2DViewer)
    {
        Volume *input = q2DViewer->getMainInput();

        if (input->getNumberOfPhases() == 1)
        {
            m_imagesOfCurrentPhaseRadioButton->setVisible(false);
            m_phasesOfCurrentImageRadioButton->setVisible(false);
        }

        // Una sola imatge
        if (q2DViewer->getMaximumSlice() * input->getNumberOfPhases() == 0)
        {
            m_allImagesRadioButton->setVisible(false);
        }
    }
    else
    {
        // Si no és un 2DViewer només oferim la opció de guardar la imatge actual
        m_imagesOfCurrentPhaseRadioButton->setVisible(false);
        m_phasesOfCurrentImageRadioButton->setVisible(false);
        m_allImagesRadioButton->setVisible(false);
    }

    // Tenim el botó de capturar la image actual clicat
    this->currentImageRadioButtonClicked();

    // No PACS selected by default
    m_pacsList->clearSelection();
}

void QExporterTool::generateAndStoreNewSeries()
{
    if (!canAllocateEnoughMemory())
    {
        DEBUG_LOG("No hi ha prou memòria per generar el nou volum.");
        WARN_LOG("No hi ha prou memòria per generar el nou volum.");
        QMessageBox::warning(this, tr("No enough memory"), tr("The new volume cannot be generated because there is not enough memory. Please, select "
                                                              "less slices or reduce the window size."));
        return;
    }

    VolumeBuilderFromCaptures *builder = new VolumeBuilderFromCaptures();
    builder->setParentStudy(m_viewer->getMainInput()->getStudy());

    QProgressDialog progress(this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimum(0);
    if (!m_pacsList->getSelectedPacs().isEmpty())
    {
        progress.setMaximum(4);
    }
    else
    {
        progress.setMaximum(3);
    }
    progress.setMinimumDuration(0);
    progress.setCancelButton(0);
    progress.setModal(true);
    progress.setLabelText(tr("Generating images..."));
    progress.setValue(0);
    qApp->processEvents();

    if (m_currentImageRadioButton->isChecked())
    {
        // Capturem la vista
        builder->addCapture(this->captureCurrentView());
    }
    else if (m_allImagesRadioButton->isChecked())
    {
        Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

        // Guardem la llesca i la fase acutal
        int currentSlice = viewer2D->getCurrentSlice();
        int currentPhase = viewer2D->getCurrentPhase();

        int maxSlice = viewer2D->getMaximumSlice() + 1;

        // En cas que tinguem fases farem tantes passades com fases
        int phases = viewer2D->getNumberOfPhases();

        for (int i = 0; i < maxSlice; i++)
        {
            viewer2D->setSlice(i);
            for (int j = 0; j < phases; j++)
            {
                viewer2D->setPhase(j);

                // Capturem la vista
                builder->addCapture(this->captureCurrentView());
            }
        }
        // Restaurem
        viewer2D->setSlice(currentSlice);
        viewer2D->setPhase(currentPhase);

    }
    else if (m_imagesOfCurrentPhaseRadioButton->isChecked())
    {
        Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

        // Guardem la llesca acutal
        int currentSlice = viewer2D->getCurrentSlice();
        int maxSlice = viewer2D->getMaximumSlice() + 1;

        for (int i = 0; i < maxSlice; i++)
        {
            viewer2D->setSlice(i);

            // Capturem la vista
            builder->addCapture(this->captureCurrentView());
        }
        // Restaurem
        viewer2D->setSlice(currentSlice);
    }
    else if (m_phasesOfCurrentImageRadioButton->isChecked())
    {
        Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

        // Guardem la fase acutal
        int currentPhase = viewer2D->getCurrentPhase();
        int phases = viewer2D->getNumberOfPhases();

        for (int i = 0; i < phases; i++)
        {
            viewer2D->setPhase(i);

            // Capturem la vista
            builder->addCapture(this->captureCurrentView());
        }

        // Restaurem
        viewer2D->setPhase(currentPhase);
    }
    else
    {
        DEBUG_LOG(QString("Radio Button no identificat!"));
        return;
    }

    builder->setSeriesDescription(m_seriesDescription->text());

    Volume *generetedVolume = builder->build();

    progress.setLabelText(tr("Generating files..."));
    progress.setValue(progress.value() + 1);
    qApp->processEvents();

    DICOMImageFileGenerator generator;

    Settings settings;

    QString dirPath = settings.getValue(InputOutputSettings::CachePath).toString() + "/" + generetedVolume->getStudy()->getInstanceUID() + "/" +
                      generetedVolume->getImage(0)->getParentSeries()->getInstanceUID();
    generator.setDirPath(dirPath);
    generator.setInput(generetedVolume);

    bool result = generator.generateDICOMFiles();

    if (result)
    {
        DEBUG_LOG("Fitxers generats correctament");

        progress.setLabelText(tr("Storing in database..."));
        progress.setValue(progress.value() + 1);
        qApp->processEvents();

        LocalDatabaseManager manager;
        manager.save(generetedVolume->getImage(0)->getParentSeries());
        // TODO Comprovar error

        // Enviem a PACS
        if (!m_pacsList->getSelectedPacs().isEmpty())
        {
            progress.setLabelText(tr("Sending to PACS..."));
            progress.setValue(progress.value() + 1);
            qApp->processEvents();
            QueryScreen *queryScreen = SingletonPointer<QueryScreen>::instance();

            foreach (PacsDevice pacsDevice, m_pacsList->getSelectedPacs())
            {
                DEBUG_LOG(QString("Sending images to PACS %1 (%2)").arg(pacsDevice.getAETitle()).arg(pacsDevice.getDescription()));
                INFO_LOG(QString("Sending images to PACS %1 (%2)").arg(pacsDevice.getAETitle()).arg(pacsDevice.getDescription()));
                queryScreen->sendDicomObjectsToPacs(pacsDevice, generetedVolume->getImages());
            }
        }

        progress.setValue(progress.value() + 1);
        qApp->processEvents();
    }
    else
    {
        ERROR_LOG(QString("Error al escriure al directori: %1").arg(dirPath));
        QMessageBox::warning(this, tr("Export to DICOM"), tr("Could not generate the new series."));
    }

    delete builder;

    this->close();
}

vtkSmartPointer<vtkImageData> QExporterTool::captureCurrentView()
{
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(m_viewer->getRenderWindow());
    windowToImageFilter->Update();
    windowToImageFilter->Modified();

    vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
    image->ShallowCopy(windowToImageFilter->GetOutput());

    return image;
}

void QExporterTool::currentImageRadioButtonClicked()
{
    this->generateCurrentPreview();
    m_numberOfImagesToStore->setText(QString("1/1"));
}

void QExporterTool::allImagesRadioButtonClicked()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        this->generate2DPreview(0, 0);
        m_numberOfImagesToStore->setText(QString("1/%1").arg(QString::number((viewer2D->getMaximumSlice() + 1) * viewer2D->getNumberOfPhases())));
    }
    else
    {
        DEBUG_LOG(QString("Només està pensat per visors 2D."));
    }
}

void QExporterTool::imageOfCurrentPhaseRadioButtonClicked()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        this->generate2DPreview(0, viewer2D->getCurrentPhase());
        m_numberOfImagesToStore->setText(QString("1/%1").arg(QString::number(m_viewer->getMainInput()->getNumberOfSlicesPerPhase())));
    }
    else
    {
        DEBUG_LOG(QString("Només està pensat per visors 2D."));
    }
}

void QExporterTool::phasesOfCurrentImageRadioButtonClicked()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        this->generate2DPreview(viewer2D->getCurrentSlice(), 0);
        m_numberOfImagesToStore->setText(QString("1/%1").arg(QString::number(m_viewer->getMainInput()->getNumberOfPhases())));
    }
    else
    {
        DEBUG_LOG(QString("Només està pensat per visors 2D."));
    }
}

void QExporterTool::generate2DPreview(int slice, int phase)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    Q_ASSERT(viewer2D);

    // Guardem la llesca i la fase acutal
    int currentSlice = viewer2D->getCurrentSlice();
    int currentPhase = viewer2D->getCurrentPhase();

    // Assignem la llesca i la fase
    viewer2D->setSlice(slice);
    viewer2D->setPhase(phase);

    generatePreview();

    // Restaurem
    viewer2D->setSlice(currentSlice);
    viewer2D->setPhase(currentPhase);
}

void QExporterTool::generateCurrentPreview()
{
    this->generatePreview();
}

void QExporterTool::generatePreview()
{
    QString path = QString("%1/preview.png").arg(QDir::tempPath());
    vtkImageWriter *writer = vtkPNGWriter::New();
    writer->SetInputData(captureCurrentView());
    writer->SetFileName(qPrintable(path));
    writer->Write();
    QPixmap pixmap(path);
    m_previewSliceLabel->setPixmap(pixmap.scaledToWidth(150));

    writer->Delete();
}
bool QExporterTool::canAllocateEnoughMemory()
{
    int numberOfScreenshots = 0;
    if (m_currentImageRadioButton->isChecked())
    {
        numberOfScreenshots = 1;
    }
    else if (m_allImagesRadioButton->isChecked())
    {
        Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

        int maxSlice = viewer2D->getMaximumSlice() + 1;
        int phases = viewer2D->getNumberOfPhases();
        numberOfScreenshots = maxSlice * phases;
    }
    else if (m_imagesOfCurrentPhaseRadioButton->isChecked())
    {
        Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

        numberOfScreenshots = viewer2D->getMaximumSlice() + 1;
    }
    else if (m_phasesOfCurrentImageRadioButton->isChecked())
    {
        Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

        numberOfScreenshots = viewer2D->getNumberOfPhases();
    }
    else
    {
        DEBUG_LOG(QString("Radio Button no identificat!"));
        return false;
    }

    return canAllocateEnoughMemory(numberOfScreenshots);
}
bool QExporterTool::canAllocateEnoughMemory(int numberOfScreenshots)
{
    QSize windowSize = m_viewer->getRenderWindowSize();
    // Calculem quanta memòria ens ocuparà el nou volum a generar
    // Cal calcular la quantitat de bytes memòria amb un tipus de píxel
    // prou gran com perquè no ens desbordi el valor i ens doni el valor correcte
    // El nou volum serà RGB = 3 bytes per píxel
    // TODO Potser aquesta mida ens l'hauria de donar vtkImageData?
    unsigned short int byteSize = 3 * sizeof(unsigned char);
    unsigned long long int imageSize = windowSize.width() * windowSize.height();
    unsigned long long int amountOfMemoryInBytes = imageSize * byteSize * numberOfScreenshots;
    DEBUG_LOG(QString("Window size: %1x%2 -- #Screenshots: %3 -- byteSize: %4").arg(windowSize.width()).arg(windowSize.height()).arg(numberOfScreenshots).arg(byteSize));
    DEBUG_LOG(QString("Simple secondary capture volume size: %1 bytes / %2 KBytes / %3 MBytes / %4 GBytes ").arg(amountOfMemoryInBytes)
                 .arg(amountOfMemoryInBytes / 1024).arg(amountOfMemoryInBytes / (1024.0 * 1024)).arg(amountOfMemoryInBytes / (1024.0 * 1024 * 1024)));
    // TODO Com a mida màxima s'agafa el doble del que ocuparà el volum ja que el vtkImageAppend allotja memòria per generar l'output.
    // Hi afegim 100MB de cortesia.
    unsigned int extraMemory = 1024 * 1024 * 100;
    amountOfMemoryInBytes += amountOfMemoryInBytes + extraMemory;

    DEBUG_LOG(QString("Total amount of memory needed (volume*2 + 100 MBytes): %1 bytes / %2 KBytes / %3 MBytes / %4 GBytes ").arg(amountOfMemoryInBytes)
                 .arg(amountOfMemoryInBytes / 1024).arg(amountOfMemoryInBytes / (1024.0 * 1024)).arg(amountOfMemoryInBytes / (1024.0 * 1024 * 1024)));

    char *p = 0;
    try
    {
        p = new char[amountOfMemoryInBytes];
        delete[] p;
        return true;
    }
    catch (std::bad_alloc)
    {
        return false;
    }
}

}
