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
#include "../core/starviewerapplication.h"

namespace udg
{

CreateDicomPrintSpool::CreateDicomPrintSpool()
{
    QDir spoolDir;
    //TODO: Spool path hardcode, s'hauria de guardar al settings
    m_spoolDirectoryPath = QDir::toNativeSeparators(UserDataRootPath + "/Spool");
    
    //TODO: S'ha de fer aquí ?
    if (!spoolDir.exists(m_spoolDirectoryPath))
    {
        spoolDir.mkdir(m_spoolDirectoryPath);
    }
}

/*void StoredDCMTK::setVolume(Volume *p_volume)
{
	m_volume=p_volume;	
	QStringList l_llistaImage=p_volume->getInputFiles();
	m_pathImage=l_llistaImage.at(0);
}*/

void CreateDicomPrintSpool::createPrintSpool(DicomPrinter dicomPrinter, DicomPrintJob dicomPrintJob)
{

    m_dicomPrintJob = dicomPrintJob;
    m_dicomPrinter = dicomPrinter;

    foreach(Image *image, dicomPrintJob.getPrintPage().getImagesToPrint())
    {
        DEBUG_LOG(QString("UID DE LA IMATGE A IMPRIMIR %1").arg(image->getSOPInstanceUID()));
        prepareForPrinting(image);
    }
}

void CreateDicomPrintSpool::prepareForPrinting(Image *image)
{
  // Inicialitzem l'StoredPrint
  //TODO Esbrinar els camps DefaultIllumination i getDefaultReflection, DicomPrint utilitzen els mateixos valors per defecte
  m_StoredPrint = new DVPSStoredPrint(2000 /*Valor per defecte a dcpstat.cfg getDefaultIlluminaton() */,10 /*Valor per defecte a dcpstat.cfg getDefaultReflection() */, qPrintable(m_dicomPrinter.getAETitle()));
  m_StoredPrint->setPrinterName(qPrintable(m_dicomPrinter.getName()));
  //TODO: Cal ? Al inicialitzar DVPSStoredPrint ja li especifiquem el AETITLE
  m_StoredPrint->setDestination(qPrintable(m_dicomPrinter.getAETitle())); 

  //TODO Això està bé però s'ha de modificar quan s'esculli la impressora correctament.
  m_StoredPrint->setImageDisplayFormat(2,2);  // Aixo s'ha de mirar
  
  //m_StoredPrint->setImageDisplayFormat(m_printer->getLayout());  // Aixo s'ha de mirar
  m_StoredPrint->setFilmSizeID(qPrintable(m_dicomPrintJob.getPrintPage().getFilmSize()));
  
  DiDisplayFunction *l_displayFunction[DVPSD_max];
  //TODO Els valors per defecte són densitatMíninim H/V (MinPrintResolution) densitatMàxima H/V (MaxPrintResolution) i mida de previsualització de la imatge (PreviewSize)
  m_PresentationState = new DVPresentationState(OFstatic_cast(DiDisplayFunction **, l_displayFunction),1024 , 1024 , 8192 , 8192, 256, 256);

  // Prova: No es pot agafar directament de l'objecte Volume??
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

  this->createHardcopyGrayscaleImage(image, pixelData, bitmapWidth, bitmapHeight, pixelAspectRatio);
  this->createStoredPrint();
}

void CreateDicomPrintSpool::createHardcopyGrayscaleImage(Image *imageToPrint, const void *pixelData, unsigned long bitmapWidth, unsigned long bitmapHeight, double pixelAspectRatio)
{	
	char tmp[70];
	OFString tmpString;
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
	
	dcmGenerateUniqueIdentifier(m_tranformedImageToPrintUID);    
	transformedImageDatasetToPrint->putAndInsertString(DCM_SOPInstanceUID, m_tranformedImageToPrintUID);		
	
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
	 sprintf(tmp, "%ld\\%ld", 1000L, OFstatic_cast(long, pixelAspectRatio*1000.0));
     transformedImageDatasetToPrint->putAndInsertString(DCM_PixelAspectRatio, tmp);
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

    transformedImagePath = m_spoolDirectoryPath + QDir::separator() + m_tranformedImageToPrintUID + ".dcm";
	// Temporal per provar!!!	
	OFCondition saveImageCondition = DVPSHelper::saveFileFormat(qPrintable(transformedImagePath), transformedImageToPrint, true);

	DVPSPresentationLUT *presLUT = m_PresentationState->getPresentationLUTData();
	const char *reqImageSize = NULL;
	OFString reqImageTmp;
	m_PresentationState->getPrintBitmapRequestedImageSize(reqImageTmp);
	reqImageSize=reqImageTmp.c_str();
	m_StoredPrint->addImageBox(qPrintable(m_dicomPrinter.getAETitle()), m_tranformedImageToPrintUID, reqImageSize, NULL, presLUT, m_PresentationState->isMonochrome1Image());
	
    DEBUG_LOG(QString("Imatge Creada %1").arg(m_tranformedImageToPrintUID));

    //TODO faltaran delete dels punters?
}


void CreateDicomPrintSpool::createStoredPrint()
{	
	DcmFileFormat *fileformat = new DcmFileFormat();
	DcmDataset *dataset	= fileformat->getDataset();	
	dataset	= fileformat->getDataset();
    QString storedPrintPath = m_spoolDirectoryPath + QDir::separator() + "storedPrint.dcm";

    char storedPrintUID[70];

    dcmGenerateUniqueIdentifier(storedPrintUID);

	m_StoredPrint->deleteAnnotations();	
	m_StoredPrint->setInstanceUID(storedPrintUID);	
	m_StoredPrint->write(*dataset, false, OFTrue, OFTrue, OFFalse);
	
    //TODO Atenció amb la / del nom delStorePrint, s'hauria d'aplicar ToNativeSeparators
	DVPSHelper::saveFileFormat(qPrintable(storedPrintPath), fileformat, true);
	//m_printSession->setStoredPrint(dataset);
    DEBUG_LOG(QString("Stored Creat %1").arg(m_tranformedImageToPrintUID));
}
} 