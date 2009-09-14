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
    m_spoolDirectoryPath = UserDataRootPath + QDir::separator() + "Spool";
    
    //TODO: S'ha de fer aquí ?
    if (!spoolDir.exists(m_spoolDirectoryPath))
    {
        spoolDir.mkdir(m_spoolDirectoryPath);
    }
}

void CreateDicomPrintSpool::createPrintSpool(DicomPrinter dicomPrinter, DicomPrintJob dicomPrintJob)
{
    m_dicomPrintJob = dicomPrintJob;
    m_dicomPrinter = dicomPrinter;

    configureDcmtkDVPSStoredPrint();

    foreach(Image *image, dicomPrintJob.getPrintPage().getImagesToPrint())
    {
        transformImageForPrinting(image);
    }

    setImageBoxAttributes();

    this->createStoredPrintDcmtkFile();
}

void CreateDicomPrintSpool::configureDcmtkDVPSStoredPrint()
{
    m_StoredPrint = new DVPSStoredPrint(2000 /*Valor per defecte a dcpstat.cfg getDefaultIlluminaton() */,10 /*Valor per defecte a dcpstat.cfg getDefaultReflection() */, qPrintable(m_dicomPrinter.getAETitle()));

    m_StoredPrint->setLog(&OFConsole::instance(), true, true);
    if (EC_Normal != m_StoredPrint->setPrinterName(qPrintable(m_dicomPrinter.getAETitle()))) DEBUG_LOG("FALLA setPrinterName");
    //TODO: Cal ? Al inicialitzar DVPSStoredPrint ja li especifiquem el AETITLE
    if (EC_Normal != m_StoredPrint->setDestination(qPrintable(m_dicomPrinter.getAETitle()))) DEBUG_LOG("FALLA set");
    //m_StoredPrint->setOriginator("PROVA"); //TODO: Cal especificar l'origen?

    //TODO Això està bé però s'ha de modificar quan s'esculli la impressora correctament.
    if (EC_Normal != m_StoredPrint->setImageDisplayFormat(2,2)) DEBUG_LOG("FALLA setImageDisplayFormat");  // Aixo s'ha de mirar

    //m_StoredPrint->setImageDisplayFormat(m_printer->getLayout());  // Aixo s'ha de mirar
    if (EC_Normal != m_StoredPrint->setFilmSizeID(qPrintable(m_dicomPrintJob.getPrintPage().getFilmSize()))) DEBUG_LOG("FALLA setFilmSize");

    if (EC_Normal != m_StoredPrint->setMagnificationType("CUBIC")) DEBUG_LOG("FALLA setMagnificationType");
    //if (EC_Normal != m_StoredPrint->setSmoothingType(qPrintable(m_dicomPrintJob.getPrintPage().getSmoothingType()))) DEBUG_LOG("FALLA setSmoothingType");
    if (EC_Normal != m_StoredPrint->setBorderDensity(qPrintable(m_dicomPrintJob.getPrintPage().getBorderDensity()))) DEBUG_LOG("FALLA setBorderDensity");
    if (EC_Normal != m_StoredPrint->setEmtpyImageDensity(qPrintable(m_dicomPrintJob.getPrintPage().getEmptyImageDensity()))) DEBUG_LOG("FALLA setEmptyImageDensity");
    if (EC_Normal != m_StoredPrint->setMaxDensity(qPrintable(QString().setNum(m_dicomPrintJob.getPrintPage().getMaxDensity()))) ) DEBUG_LOG("FALLA setMaxDensity");
    if (EC_Normal != m_StoredPrint->setMinDensity(qPrintable(QString().setNum(m_dicomPrintJob.getPrintPage().getMinDensity())))) DEBUG_LOG("FALLA setMinDensity");
    
    if (m_dicomPrintJob.getPrintPage().getFilmOrientation() == "PORTRAIT")
    {
        if (EC_Normal != m_StoredPrint->setFilmOrientation(DVPSF_portrait)) DEBUG_LOG("FALLA setFilmOrientaion");    
    }
    else if (m_dicomPrintJob.getPrintPage().getFilmOrientation() == "LANDSCAPE")
    {
        if (EC_Normal != m_StoredPrint->setFilmOrientation(DVPSF_landscape)) DEBUG_LOG("FALLA setFilmOrientation");
    }
    
    if (m_dicomPrintJob.getPrintPage().getTrim())
    {
        if (EC_Normal != m_StoredPrint->setTrim(DVPSH_trim_on)) DEBUG_LOG("FALLA setTrim");
    }
    else m_StoredPrint->setTrim(DVPSH_trim_off);   
    
    m_StoredPrint->setConfigurationInformation("");

    //m_StoredPrint->setResolutionID(NULL);

    //TODO: No sé si es guarden al fitxer StoredPrint potser s'han d'especificar en el moment d'imprimir ?
    if (EC_Normal != m_StoredPrint->setRequestedDecimateCropBehaviour(DVPSI_decimate)) DEBUG_LOG("FALLA setRequestedDecimate.."); //Ob
    if (EC_Normal != m_StoredPrint->setPrintIllumination(2000)) DEBUG_LOG("FALLA setPrintIllumination");
    if (EC_Normal != m_StoredPrint->setPrintReflectedAmbientLight(10)) DEBUG_LOG("FALLA setPrintIllumination");

    /*if ((opt_illumination != (OFCmdUnsignedInt)-1)&&(EC_Normal != dvi.getPrintHandler().setPrintIllumination((Uint16)opt_illumination)))
      CERR << "warning: cannot set illumination to '" << opt_illumination << "', ignoring." << endl;
    if ((opt_reflection != (OFCmdUnsignedInt)-1)&&(EC_Normal != dvi.getPrintHandler().setPrintReflectedAmbientLight((Uint16)opt_reflection)))
      CERR << "warning: cannot set reflected ambient light to '" << opt_reflection << "', ignoring." << endl;*/
}

void CreateDicomPrintSpool::transformImageForPrinting(Image *image)
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

  this->createHardcopyGrayscaleImage(image, pixelData, bitmapWidth, bitmapHeight, pixelAspectRatio);
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
    DEBUG_LOG(qPrintable(m_dicomPrinter.getAETitle()));

    //TODO faltaran delete dels punters?
}

void CreateDicomPrintSpool::setImageBoxAttributes()
{
    size_t numImages = m_StoredPrint->getNumberOfImages();
    OFCondition result;

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

void CreateDicomPrintSpool::createStoredPrintDcmtkFile()
{	
	DcmFileFormat *fileformat = new DcmFileFormat();
	DcmDataset *dataset	= fileformat->getDataset();	
    QString storedPrintPath = m_spoolDirectoryPath + QDir::separator() + "storedPrint.dcm";

    char storedPrintUID[70];

    dcmGenerateUniqueIdentifier(storedPrintUID);

	m_StoredPrint->deleteAnnotations();	
	m_StoredPrint->setInstanceUID(storedPrintUID);	
	OFCondition write = m_StoredPrint->write(*dataset, false, OFTrue, OFFalse, OFTrue);
	
	write = DVPSHelper::saveFileFormat(qPrintable(storedPrintPath), fileformat, true);
}
} 
