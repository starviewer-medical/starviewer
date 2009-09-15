#include "createdicomprintspool.h"

#include <QDir>
#include "dcmtk/dcmpstat/dviface.h"
#include "dcmtk/dcmpstat/dvpssp.h"      /* for class DVPSStoredPrint */
#include <dvpshlp.h>

//#include "volume.h"
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
    m_StoredPrint = new DVPSStoredPrint(2000 /*Valor per defecte a dcpstat.cfg getDefaultIlluminaton() */,10 /*Valor per defecte a dcpstat.cfg getDefaultReflection() */, qPrintable(m_dicomPrinter.getAETitle()));

    m_StoredPrint->setPrinterName(qPrintable(m_dicomPrinter.getAETitle()));
    //TODO: Cal ? Al inicialitzar DVPSStoredPrint ja li especifiquem el AETITLE
    m_StoredPrint->setDestination(qPrintable(m_dicomPrinter.getAETitle()));
    //m_StoredPrint->setOriginator("PROVA"); //TODO: Cal especificar l'origen?

    //TODO: El tamany del la placa està hardcoded
    m_StoredPrint->setImageDisplayFormat(2,2);  // Aixo s'ha de mirar

    m_StoredPrint->setFilmSizeID(qPrintable(m_dicomPrintJob.getPrintPage().getFilmSize()));

    m_StoredPrint->setMagnificationType("CUBIC");
    //m_StoredPrint->setSmoothingType(qPrintable(m_dicomPrintJob.getPrintPage().getSmoothingType()));
    m_StoredPrint->setBorderDensity(qPrintable(m_dicomPrintJob.getPrintPage().getBorderDensity()));
    m_StoredPrint->setEmtpyImageDensity(qPrintable(m_dicomPrintJob.getPrintPage().getEmptyImageDensity()));
    m_StoredPrint->setMaxDensity(qPrintable(QString().setNum(m_dicomPrintJob.getPrintPage().getMaxDensity())));
    m_StoredPrint->setMinDensity(qPrintable(QString().setNum(m_dicomPrintJob.getPrintPage().getMinDensity())));
    
    if (m_dicomPrintJob.getPrintPage().getFilmOrientation() == "PORTRAIT")
    {
        m_StoredPrint->setFilmOrientation(DVPSF_portrait);
    }
    else if (m_dicomPrintJob.getPrintPage().getFilmOrientation() == "LANDSCAPE")
    {
        m_StoredPrint->setFilmOrientation(DVPSF_landscape);
    }
    
    if (m_dicomPrintJob.getPrintPage().getTrim())
    {
        m_StoredPrint->setTrim(DVPSH_trim_on);
    }
    else m_StoredPrint->setTrim(DVPSH_trim_off);
    
    m_StoredPrint->setConfigurationInformation("");

    //m_StoredPrint->setResolutionID(NULL);

    //TODO: No sé si es guarden al fitxer StoredPrint potser s'han d'especificar en el moment d'imprimir ?
    m_StoredPrint->setRequestedDecimateCropBehaviour(DVPSI_decimate); //TODO: Valor hardcoded estudiar, si és el més correcte
    m_StoredPrint->setPrintIllumination(2000); //TODO: Valor hardcoded estudiar, si és el més correcte
    m_StoredPrint->setPrintReflectedAmbientLight(10);//TODO: Valor hardcoded estudiar, si és el més correcte
}

void CreateDicomPrintSpool::transformImageForPrinting(Image *image, const QString &spoolDirectoryPath)
{
  // Inicialitzem l'StoredPrint
  //TODO Esbrinar els camps DefaultIllumination i getDefaultReflection, DicomPrint utilitzen els mateixos valors per defecte
  
  DiDisplayFunction *l_displayFunction[DVPSD_max];
  //TODO Els valors per defecte són densitatMíninim H/V (MinPrintResolution) densitatMàxima H/V (MaxPrintResolution) i mida de previsualització de la imatge (PreviewSize)
  m_PresentationState = new DVPresentationState(OFstatic_cast(DiDisplayFunction **, l_displayFunction),1024 , 1024 , 8192 , 8192, 256, 256);
  DcmFileFormat *imageFileFormat = NULL;
  DcmDataset	*imageToPrintDataset = NULL;
  
  DVPSHelper::loadFileFormat(qPrintable(image->getPath()), imageFileFormat);
  imageToPrintDataset = imageFileFormat->getDataset();
  //TODO que significa el false?
  m_PresentationState->attachImage(imageFileFormat,false);

  unsigned long bitmapWidth, bitmapHeight, bitmapSize;
  double pixelAspectRatio;
  void *pixelData;

  bitmapSize = m_PresentationState->getPrintBitmapSize();
  pixelAspectRatio = m_PresentationState->getPrintBitmapPixelAspectRatio();
  m_PresentationState->getPrintBitmapWidthHeight(bitmapWidth, bitmapHeight);   	    
  pixelData = new char[bitmapSize];
  //TODO que significa el false ?
  m_PresentationState->getPrintBitmap(pixelData, bitmapSize, false); 

  this->createHardcopyGrayscaleImage(image, pixelData, bitmapWidth, bitmapHeight, pixelAspectRatio, spoolDirectoryPath);
}

void CreateDicomPrintSpool::createHardcopyGrayscaleImage(Image *imageToPrint, const void *pixelData, unsigned long bitmapWidth, unsigned long bitmapHeight, double pixelAspectRatio, const QString &spoolDirectoryPath)
{	
    char InstanceUIDOfTransformedImage[70];
	OFString tmpString, requestedImageSizeAsOFString;
	DcmFileFormat *transformedImageToPrint = new DcmFileFormat();
	DcmDataset *transformedImageDatasetToPrint = transformedImageToPrint->getDataset();
    QString transformedImagePath;

	// write patient module
	m_PresentationState->writeHardcopyImageAttributes(*transformedImageDatasetToPrint);
	// write general study and general series module
	m_StoredPrint->writeHardcopyImageAttributes(*transformedImageDatasetToPrint);
	
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
	DVPSHelper::currentDate(tmpString);
	transformedImageDatasetToPrint->putAndInsertString(DCM_InstanceCreationDate, tmpString.c_str());
	
    //TODO:Prova de substiuir per mètode qt ? 
	DVPSHelper::currentTime(tmpString);
	transformedImageDatasetToPrint->putAndInsertString(DCM_InstanceCreationTime, tmpString.c_str());
	 
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

	if(m_PresentationState->getPresentationLUT() == DVPSP_table)
	{
		m_PresentationState->writePresentationLUTforPrint(*transformedImageDatasetToPrint);	
	}

    //TODO:S'hauria de fer a un altre lloc aquest càlcul perquè també s'utilitza a PrintDicomSpool
    transformedImagePath = spoolDirectoryPath + QDir::separator() + InstanceUIDOfTransformedImage + ".dcm";
	OFCondition saveImageCondition = DVPSHelper::saveFileFormat(qPrintable(transformedImagePath), transformedImageToPrint, true);

	m_PresentationState->getPrintBitmapRequestedImageSize(requestedImageSizeAsOFString);
	m_StoredPrint->addImageBox(qPrintable(m_dicomPrinter.getAETitle()), InstanceUIDOfTransformedImage, requestedImageSizeAsOFString.c_str(), NULL, 
                                          m_PresentationState->getPresentationLUTData(), m_PresentationState->isMonochrome1Image());
	
    DEBUG_LOG(QString("Imatge Creada %1").arg(InstanceUIDOfTransformedImage));
    DEBUG_LOG(qPrintable(m_dicomPrinter.getAETitle()));

    //TODO faltaran delete dels punters?
}

void CreateDicomPrintSpool::setImageBoxAttributes()
{
    size_t numImages = m_StoredPrint->getNumberOfImages();

    for (size_t i = 0; i < numImages; i++)
    {
        //TODO Assegurar que quan no s'han seleccionat aquests paràmetres tenen valor empty, podria ser que tinguessin valor NONE, funciona igualment ?
        if (!m_dicomPrintJob.getPrintPage().getPolarity().isEmpty())
        {
            m_StoredPrint->setImagePolarity(i, qPrintable(m_dicomPrintJob.getPrintPage().getPolarity()));
        }

        //TODO: El podem especificar a nivell de FilmBox, potser seria millor ja que no ho deixem triar a nivell d'imatge
        if (!m_dicomPrintJob.getPrintPage().getMagnificationType().isEmpty())
        {
            m_StoredPrint->setImageMagnificationType(i, qPrintable(m_dicomPrintJob.getPrintPage().getMagnificationType()));
        }

        //TODO: El podem especificar a nivell de FilmBox, potser seria millor ja que no ho deixem triar a nivell d'imatge
        if (!m_dicomPrintJob.getPrintPage().getSmoothingType().isEmpty())
        {
            m_StoredPrint->setImageSmoothingType(i, qPrintable(m_dicomPrintJob.getPrintPage().getSmoothingType()));
        }
     
        //TODO: Fa falta especificar-lo si té valor null?
        m_StoredPrint->setImageConfigurationInformation(i, NULL);
    }
}

void CreateDicomPrintSpool::createStoredPrintDcmtkFile(const QString &storedPrintDcmtkFilePath)
{	
	DcmFileFormat *fileformat = new DcmFileFormat();
	DcmDataset *dataset	= fileformat->getDataset();	
    char storedPrintInstanceUID[70];

    dcmGenerateUniqueIdentifier(storedPrintInstanceUID);

	m_StoredPrint->deleteAnnotations();	
	m_StoredPrint->setInstanceUID(storedPrintInstanceUID);	
	OFCondition write = m_StoredPrint->write(*dataset, false, OFTrue, OFFalse, OFTrue);
	
	write = DVPSHelper::saveFileFormat(qPrintable(storedPrintDcmtkFilePath), fileformat, true);
}
} 
