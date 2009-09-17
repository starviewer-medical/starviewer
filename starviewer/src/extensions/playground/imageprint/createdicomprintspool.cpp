#include "createdicomprintspool.h"

#include "dcmtk/dcmpstat/dviface.h"
#include "dcmtk/dcmpstat/dvpssp.h"      /* for class DVPSStoredPrint */
#include <dvpshlp.h>

#include <QDir>
#include <QDateTime>
#include "dicomprintjob.h"
#include "dicomprintpage.h"
#include "dicomprinter.h"
#include "image.h"
#include "logging.h"

namespace udg
{

QString CreateDicomPrintSpool::createPrintSpool(DicomPrinter dicomPrinter, DicomPrintJob dicomPrintJob, const QString &spoolDirectoryPath)
{
    QDir spoolDir;
    QString storedPrintDcmtkFilePath = spoolDirectoryPath + QDir::separator() + "storedPrint.dcm";

    //TODO: S'ha de fer aquí ? Comprovem si existeix el directori on s'ha de generar l'spool
    if (!spoolDir.exists(spoolDirectoryPath))
    {
        spoolDir.mkdir(spoolDirectoryPath);
    }

    m_dicomPrintJob = dicomPrintJob;
    m_dicomPrinter = dicomPrinter;

    setBasicFilmBoxAttributes();

    foreach(Image *image, dicomPrintJob.getPrintPage().getImagesToPrint())
    {
        transformImageForPrinting(image, spoolDirectoryPath);
    }

    setImageBoxAttributes();

    createStoredPrintDcmtkFile(storedPrintDcmtkFilePath);

    return storedPrintDcmtkFilePath;
}

void CreateDicomPrintSpool::setBasicFilmBoxAttributes()
{
    m_storedPrint = new DVPSStoredPrint(2000 /*Valor per defecte a dcpstat.cfg getDefaultIlluminaton() */,10 /*Valor per defecte a dcpstat.cfg getDefaultReflection() */, qPrintable(m_dicomPrinter.getAETitle()));

    m_storedPrint->setPrinterName(qPrintable(m_dicomPrinter.getAETitle()));
    //TODO: Cal ? Al inicialitzar DVPSStoredPrint ja li especifiquem el AETITLE
    m_storedPrint->setDestination(qPrintable(m_dicomPrinter.getAETitle()));
    //m_storedPrint->setOriginator("PROVA"); //TODO: Cal especificar l'origen?

    //TODO: El tamany del la placa està hardcoded
    m_storedPrint->setImageDisplayFormat(m_dicomPrintJob.getPrintPage().getFilmLayoutColumns(), m_dicomPrintJob.getPrintPage().getFilmLayoutRows());  // Aixo s'ha de mirar

    m_storedPrint->setFilmSizeID(qPrintable(m_dicomPrintJob.getPrintPage().getFilmSize()));

    m_storedPrint->setMagnificationType("CUBIC");
    //m_storedPrint->setSmoothingType(qPrintable(m_dicomPrintJob.getPrintPage().getSmoothingType()));
    m_storedPrint->setBorderDensity(qPrintable(m_dicomPrintJob.getPrintPage().getBorderDensity()));
    m_storedPrint->setEmtpyImageDensity(qPrintable(m_dicomPrintJob.getPrintPage().getEmptyImageDensity()));
    m_storedPrint->setMaxDensity(qPrintable(QString().setNum(m_dicomPrintJob.getPrintPage().getMaxDensity())));
    m_storedPrint->setMinDensity(qPrintable(QString().setNum(m_dicomPrintJob.getPrintPage().getMinDensity())));
    
    if (m_dicomPrintJob.getPrintPage().getFilmOrientation() == "PORTRAIT")
    {
        m_storedPrint->setFilmOrientation(DVPSF_portrait);
    }
    else if (m_dicomPrintJob.getPrintPage().getFilmOrientation() == "LANDSCAPE")
    {
        m_storedPrint->setFilmOrientation(DVPSF_landscape);
    }
    
    if (m_dicomPrintJob.getPrintPage().getTrim())
    {
        m_storedPrint->setTrim(DVPSH_trim_on);
    }
    else m_storedPrint->setTrim(DVPSH_trim_off);
    
    m_storedPrint->setConfigurationInformation("");

    //m_storedPrint->setResolutionID(NULL);

    //TODO: No sé si es guarden al fitxer StoredPrint potser s'han d'especificar en el moment d'imprimir ?
    m_storedPrint->setRequestedDecimateCropBehaviour(DVPSI_decimate); //TODO: Valor hardcoded estudiar, si és el més correcte
    m_storedPrint->setPrintIllumination(2000); //TODO: Valor hardcoded estudiar, si és el més correcte
    m_storedPrint->setPrintReflectedAmbientLight(10);//TODO: Valor hardcoded estudiar, si és el més correcte
}

void CreateDicomPrintSpool::transformImageForPrinting(Image *image, const QString &spoolDirectoryPath)
{
    // Inicialitzem l'StoredPrint
    //TODO Esbrinar els camps DefaultIllumination i getDefaultReflection, DicomPrint utilitzen els mateixos valors per defecte

    //TODO:m_presentationState cal que sigui global ? cada vegada li fem un new cal ?
    DiDisplayFunction *l_displayFunction[DVPSD_max];
    //TODO Els valors per defecte són densitatMíninim H/V (MinPrintResolution) densitatMàxima H/V (MaxPrintResolution) i mida de previsualització de la imatge (PreviewSize)
    m_presentationState = new DVPresentationState(OFstatic_cast(DiDisplayFunction **, l_displayFunction), 1024 , 1024 , 8192 , 8192, 256, 256);
    DcmFileFormat *imageFileFormat = NULL;
    DcmDataset *imageToPrintDataset = NULL;

    DVPSHelper::loadFileFormat(qPrintable(image->getPath()), imageFileFormat);
    imageToPrintDataset = imageFileFormat->getDataset();
    //TODO que significa el false?
    m_presentationState->attachImage(imageFileFormat,false);

    unsigned long bitmapWidth, bitmapHeight, bitmapSize;
    double pixelAspectRatio;
    void *pixelData;

    bitmapSize = m_presentationState->getPrintBitmapSize();
    pixelAspectRatio = m_presentationState->getPrintBitmapPixelAspectRatio();
    m_presentationState->getPrintBitmapWidthHeight(bitmapWidth, bitmapHeight);   	    
    pixelData = new char[bitmapSize];
    //TODO que significa el false ?
    m_presentationState->getPrintBitmap(pixelData, bitmapSize, false); 

    this->createHardcopyGrayscaleImage(image, pixelData, bitmapWidth, bitmapHeight, pixelAspectRatio, spoolDirectoryPath);
}

void CreateDicomPrintSpool::createHardcopyGrayscaleImage(Image *imageToPrint, const void *pixelData, unsigned long bitmapWidth, unsigned long bitmapHeight, double pixelAspectRatio, const QString &spoolDirectoryPath)
{
    char InstanceUIDOfTransformedImage[70];
    OFString requestedImageSizeAsOFString;
    DcmFileFormat *transformedImageToPrint = new DcmFileFormat();
    DcmDataset *transformedImageDatasetToPrint = transformedImageToPrint->getDataset();
    QString transformedImagePath;

    // write patient module
    m_presentationState->writeHardcopyImageAttributes(*transformedImageDatasetToPrint);
    // write general study and general series module
    m_storedPrint->writeHardcopyImageAttributes(*transformedImageDatasetToPrint);

    // Hardcopy Equipment Module
    //TODO: posar dades starviewer ?
    transformedImageDatasetToPrint->putAndInsertString(DCM_HardcopyDeviceManufacturer, "OFFIS", true);
    transformedImageDatasetToPrint->putAndInsertString(DCM_HardcopyDeviceSoftwareVersion, OFFIS_DTK_IMPLEMENTATION_VERSION_NAME, true);	

    // General Image Module
    transformedImageDatasetToPrint->putAndInsertString(DCM_InstanceNumber, qPrintable(imageToPrint->getInstanceNumber()));
    transformedImageDatasetToPrint->putAndInsertString(DCM_PatientOrientation, qPrintable(imageToPrint->getPatientOrientation()));
    transformedImageDatasetToPrint->putAndInsertString(DCM_ImageType, "DERIVED\\SECONDARY", true);
    transformedImageDatasetToPrint->putAndInsertString(DCM_DerivationDescription, "Hardcopy rendered using Presentation State");
    	
    // SOP Common Module
    transformedImageDatasetToPrint->putAndInsertString(DCM_SOPClassUID, UID_HardcopyGrayscaleImageStorage);

    dcmGenerateUniqueIdentifier(InstanceUIDOfTransformedImage);    
    transformedImageDatasetToPrint->putAndInsertString(DCM_SOPInstanceUID, InstanceUIDOfTransformedImage);		

    //TODO:Prova de substiuir per mètode qt ? 
    transformedImageDatasetToPrint->putAndInsertString(DCM_InstanceCreationDate, qPrintable(QDateTime::currentDateTime().toString("yyyyMMdd")));

    //TODO:Prova de substiuir per mètode qt ? 
    transformedImageDatasetToPrint->putAndInsertString(DCM_InstanceCreationTime, qPrintable(QDateTime::currentDateTime().toString("hhmmss")));

    // Hardcopy Grayscale Image Module
    transformedImageDatasetToPrint->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_SamplesPerPixel, 1);

    transformedImageDatasetToPrint->putAndInsertUint16(DCM_Rows, OFstatic_cast(Uint16, bitmapWidth));
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_Columns, OFstatic_cast(Uint16, bitmapHeight));
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_BitsAllocated, 16);
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_BitsStored, 12);
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_HighBit, 11);
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_PixelRepresentation, 0);

    if(pixelAspectRatio!=1.0)
    {
        char pixelAspectRatioAsChar[70];

        sprintf(pixelAspectRatioAsChar, "%ld\\%ld", 1000L, OFstatic_cast(long, pixelAspectRatio*1000.0));
        transformedImageDatasetToPrint->putAndInsertString(DCM_PixelAspectRatio, pixelAspectRatioAsChar);
    }

    DcmPolymorphOBOW *pxData = new DcmPolymorphOBOW(DCM_PixelData);

    if(pxData)
    {
        pxData->putUint16Array(OFstatic_cast(Uint16 *, OFconst_cast(void *, pixelData)), OFstatic_cast(unsigned long, bitmapWidth*bitmapHeight));
        transformedImageDatasetToPrint->insert(pxData, OFTrue);
    }

    if(m_presentationState->getPresentationLUT() == DVPSP_table)
    {
        m_presentationState->writePresentationLUTforPrint(*transformedImageDatasetToPrint);	
    }

    //TODO:S'hauria de fer a un altre lloc aquest càlcul perquè també s'utilitza a PrintDicomSpool
    transformedImagePath = spoolDirectoryPath + QDir::separator() + InstanceUIDOfTransformedImage + ".dcm";
    OFCondition saveImageCondition = DVPSHelper::saveFileFormat(qPrintable(transformedImagePath), transformedImageToPrint, true);

    m_presentationState->getPrintBitmapRequestedImageSize(requestedImageSizeAsOFString);
    m_storedPrint->addImageBox(qPrintable(m_dicomPrinter.getAETitle()), InstanceUIDOfTransformedImage, requestedImageSizeAsOFString.c_str(), NULL, 
                               m_presentationState->getPresentationLUTData(), m_presentationState->isMonochrome1Image());

    DEBUG_LOG(QString("Imatge Creada %1").arg(InstanceUIDOfTransformedImage));
    DEBUG_LOG(qPrintable(m_dicomPrinter.getAETitle()));

    //TODO faltaran delete dels punters?
}

void CreateDicomPrintSpool::setImageBoxAttributes()
{
    size_t numImages = m_storedPrint->getNumberOfImages();

    for (size_t i = 0; i < numImages; i++)
    {
        //TODO Assegurar que quan no s'han seleccionat aquests paràmetres tenen valor empty, podria ser que tinguessin valor NONE, funciona igualment ?
        if (!m_dicomPrintJob.getPrintPage().getPolarity().isEmpty())
        {
            m_storedPrint->setImagePolarity(i, qPrintable(m_dicomPrintJob.getPrintPage().getPolarity()));
        }

        //TODO: El podem especificar a nivell de FilmBox, potser seria millor ja que no ho deixem triar a nivell d'imatge
        if (!m_dicomPrintJob.getPrintPage().getMagnificationType().isEmpty())
        {
            m_storedPrint->setImageMagnificationType(i, qPrintable(m_dicomPrintJob.getPrintPage().getMagnificationType()));
        }

        //TODO: El podem especificar a nivell de FilmBox, potser seria millor ja que no ho deixem triar a nivell d'imatge
        if (!m_dicomPrintJob.getPrintPage().getSmoothingType().isEmpty())
        {
            m_storedPrint->setImageSmoothingType(i, qPrintable(m_dicomPrintJob.getPrintPage().getSmoothingType()));
        }
     
        //TODO: Fa falta especificar-lo si té valor null?
        m_storedPrint->setImageConfigurationInformation(i, NULL);
    }
}

void CreateDicomPrintSpool::createStoredPrintDcmtkFile(const QString &storedPrintDcmtkFilePath)
{
    DcmFileFormat *fileformat = new DcmFileFormat();
    DcmDataset *dataset	= fileformat->getDataset();	
    char storedPrintInstanceUID[70];

    dcmGenerateUniqueIdentifier(storedPrintInstanceUID);

    m_storedPrint->deleteAnnotations();	
    m_storedPrint->setInstanceUID(storedPrintInstanceUID);	
    OFCondition write = m_storedPrint->write(*dataset, false, OFTrue, OFFalse, OFTrue);

    write = DVPSHelper::saveFileFormat(qPrintable(storedPrintDcmtkFilePath), fileformat, true);
}
}
