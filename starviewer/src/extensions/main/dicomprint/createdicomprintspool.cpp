#include "createdicomprintspool.h"

#include <dviface.h>
// For class DVPSStoredPrint
#include <dvpssp.h>
#include <dvpshlp.h>
#include <dvpsabl.h>

#include <QDir>
#include <QDateTime>
#include <QPair>

#include "dicomprintjob.h"
#include "dicomprintpage.h"
#include "dicomprinter.h"
#include "image.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "pacsdevice.h"
#include "inputoutputsettings.h"
#include "dicomprintpresentationstateimage.h"

namespace udg {

QString CreateDicomPrintSpool::createPrintSpool(DicomPrinter dicomPrinter, DicomPrintPage dicomPrintPage, const QString &spoolDirectoryPath)
{
    QDir spoolDir;
    bool ok = false;

    m_lastError = CreateDicomPrintSpool::Ok;
    // TODO: S'ha de fer aquí ? Comprovem si existeix el directori on s'ha de generar l'spool
    if (!spoolDir.exists(spoolDirectoryPath))
    {
        INFO_LOG("Es crearà el directori d'spool " + spoolDirectoryPath);
        if (!spoolDir.mkdir(spoolDirectoryPath))
        {
            ERROR_LOG("No s'ha pogut crear el directori d'spool");
            m_lastError = CreateDicomPrintSpool::ErrorCreatingImageSpool;
            return "";
        }
    }

    m_dicomPrintPage = dicomPrintPage;
    m_dicomPrinter = dicomPrinter;

    setBasicFilmBoxAttributes();

    for (int index = 0; index < m_dicomPrintPage.getImagesToPrint().count(); index++)
    {
        QPair< Image*, DICOMPrintPresentationStateImage > imageToPrintWithPresentationState = m_dicomPrintPage.getImagesToPrint().at(index);
        ok = transformImageForPrinting(imageToPrintWithPresentationState.first, imageToPrintWithPresentationState.second, spoolDirectoryPath);

        if (!ok)
        {
            break;
        }
    }

    if (ok)
    {
        setImageBoxAttributes();
        createAnnotationBoxes();

        return createStoredPrintDcmtkFile(spoolDirectoryPath);
    }
    else
    {
        return "";
    }
}

void CreateDicomPrintSpool::setBasicFilmBoxAttributes()
{
    // El constructor del DVPStoredPrint se li ha de passar com a paràmetres
    //  1r El tag (2010,015E) Illumination de la Basic Film Box
    //  2n El tag (2010,0160) Reflected Ambient Light de la Basic Film Box
    //  3r AETitle del Starviewer

    //  Els dos primer paràmetres només s'utilitzen si la impressora suporta el Presentation Lut, ara mateix no ho soportem (no està implementat) per tant se
    //  suposa que aquests valors s'ignoraran. De totes maneres se li ha donat aquests valors per defecte 2000 i 10 respectivament perquè són els que utilitza
    // dcmtk i també s'ha consultat el dicom conformance de les impressores agfa i kodak i també utiltizen aquests valors per defecte.

    // TODO preguntar perquè necessita el Illumination i Reflected Ambient Ligth, preguntar si realement són aquests tags
    m_storedPrint = new DVPSStoredPrint(2000, 10, qPrintable(Settings().getValue(InputOutputSettings::LocalAETitle).toString()));
    // S'ha d'indicar el AETitle de la impressora
    m_storedPrint->setDestination(qPrintable(m_dicomPrinter.getAETitle()));
    m_storedPrint->setPrinterName(qPrintable(m_dicomPrinter.getAETitle()));

    // Indiquem el layout de la placa
    m_storedPrint->setImageDisplayFormat(m_dicomPrintPage.getFilmLayoutColumns(), m_dicomPrintPage.getFilmLayoutRows());

    m_storedPrint->setFilmSizeID(qPrintable(m_dicomPrintPage.getFilmSize()));

    // Interpolació que s'aplicarà si s'ha d'escalar o ampliar la imatge perquè càpiga a la cel·la
    // Aquest tag també es pot especificar a nivell de Image Box, assignant-li una valor diferent per cada imatge.
    m_storedPrint->setMagnificationType(qPrintable(m_dicomPrintPage.getMagnificationType()));

    if (m_dicomPrintPage.getMagnificationType().compare("CUBIC") == 0)
    {
        // El Smoothing Type, tag 2010,0080 del Basic Film Box, només se li pot donar valor sir el tag Magnification Type 2010,0060 té com a valor 'CUBIC'
        // Especifica el tipus de funció d'interpollació a aplicar.
        // Aquest tag també es pot especificar a nivell de Image Box, assignant-li una valor diferent per cada imatge.
        m_storedPrint->setSmoothingType(qPrintable(m_dicomPrintPage.getSmoothingType()));
    }

    // Densitat del marc que separa les imatges
    m_storedPrint->setBorderDensity(qPrintable(m_dicomPrintPage.getBorderDensity()));
    // Densitat de les cel·les buides
    m_storedPrint->setEmtpyImageDensity(qPrintable(m_dicomPrintPage.getEmptyImageDensity()));

    if (m_dicomPrintPage.getMinDensity() != 0)
    {
        // Si la densitat és 0, vol dir que no ens l'han especificat per tant no l'enviem
        m_storedPrint->setMinDensity(qPrintable(QString().setNum(m_dicomPrintPage.getMinDensity())));
    }

    if (m_dicomPrintPage.getMaxDensity() != 0)
    {
        // Si la densitat és 0, vol dir que no ens l'han especificat per tant no l'enviem
        m_storedPrint->setMaxDensity(qPrintable(QString().setNum(m_dicomPrintPage.getMaxDensity())));
    }

    if (m_dicomPrintPage.getFilmOrientation() == "PORTRAIT")
    {
        m_storedPrint->setFilmOrientation(DVPSF_portrait);
    }
    else if (m_dicomPrintPage.getFilmOrientation() == "LANDSCAPE")
    {
        m_storedPrint->setFilmOrientation(DVPSF_landscape);
    }

    m_storedPrint->setTrim(m_dicomPrintPage.getTrim() ? DVPSH_trim_on : DVPSH_trim_off);

    // Tag Configuration Information (2010,0150) de Basic Film Box no li donem valor ara mateix, aquest camp permet configurar les impressions
    // amb característiques que no són Dicom Conformance, sinó que són dependents de al impressora.
    // Aquest tag també es pot especificar a nivell de Image Box, assignant-li una valor diferent per cada imatge.
    m_storedPrint->setConfigurationInformation(qPrintable(m_dicomPrintPage.getConfigurationInformation()));

    // Tag Requested Resolution ID (2020,0050) de Basic Film Box serveix per especificar amb quina resolució s'han d'imprimir les imatges,
    // té dos valors STANTARD i HIGH.
    // No se li assigna valor, perquè mirant el dicom conformance de varies impressores, la majoria no accepten aquest tag i les que l'accepten
    // només l'accepten amb el valor STANDARD, per tant no s'especifica.

    //m_storedPrint->setResolutionID(NULL);

    // Tag Requested Decimate/Crop Behaviour (2020,0040) de Imagex Box Indica que s'ha de fer si la imatge excedeix el màxim de píxels que suporta la cel·la
    // Hi ha 3 comportaments :
    //      - Decimate : Escala la imatge fins que hi càpiga
    //      - Crop : El centra de la imatge es posa al centra de la cel·la i tot lo que no hi càpiga no s'imprimeix.
    //      - Fail : La impressora ens ha d'indicar que no ha pogut imprimir

    // Aquest tag no s'especifica, perquè mirant el dicom conformance de les impressores, per defecte tenen ja establert el comportament més desitjat que és el
    // del decimate, per això de moment no s'especifica, i la impressora es comportarà com estigui definida per defecte.

    //m_storedPrint->setRequestedDecimateCropBehaviour(DVPSI_decimate);

    INFO_LOG("Emplenats els tags del FilmBox a l'objecte DVPStoredPrint");
}

bool CreateDicomPrintSpool::transformImageForPrinting(Image *imageToPrint, DICOMPrintPresentationStateImage dicomPrintPresentationStateImage, const QString &spoolDirectoryPath)
{
    unsigned long bitmapWidth, bitmapHeight, bitmapSize;
    double pixelAspectRatio;
    char *pixelData;
    DcmFileFormat *imageToPrintDcmFileFormat = NULL;
    DcmDataset *imageToPrintDataset = NULL;
    OFCondition status;
    bool ok = false;

    // El constructor del mètode DVPresentationState necessita els següents paràmetres
    // 1r - Llista d'objectes que descriuen les característiques de la pantalla tipus objecte DiDisplayFunction, com aquestes imatges no han de ser
    // visualitzades per pantalla li donem valor null, no cal aplicar cap filtre per visualitzar-les

    // 2n, 3r i 4t, 5è Indiquen la resolució mínima d'impressió H/V i la màxima H/V respectivament, se li donen els valors per defecte de les dcmtk, consultant
    // el DICOM Conformance d'algunes impressores, s'ha vist que imprimint una sola imatge amb format STARDARD\1,1 per tamanys del film grans algunes
    // impressores poden imprimir en una resolució superior de fins 11000, però com que difícilment tindrem casos en els que s'imprimeixin una sola imatge en
    // films grans deixem els valors per defecte de les dcmtk.

    // 6è, 7è - Resolució per la previsualització de la imatge, com que no en farem previsualització deixem els valors standards.
    m_presentationState = new DVPresentationState(NULL, 1024, 1024, 8192, 8192, 256, 256);

    INFO_LOG(QString("Es transformara la imatge %1 frame %2 per imprimir.").arg(imageToPrint->getPath()).arg(imageToPrint->getFrameNumber()));

    // Carreguem la imatge que hem d'imprimor
    status = DVPSHelper::loadFileFormat(qPrintable(imageToPrint->getPath()), imageToPrintDcmFileFormat);
    if (status != EC_Normal)
    {
        ERROR_LOG("No s'ha pogut carregar la imatge " + imageToPrint->getPath() + " . Descripcio error: " + QString(status.text()));
        m_lastError = CreateDicomPrintSpool::ErrorCreatingImageSpool;
        return false;
    }

    imageToPrintDataset = imageToPrintDcmFileFormat->getDataset();

    // Traspassem la informació del mòdul de pacient i imatge entre d'altres al presentation state
    status = m_presentationState->createFromImage(*imageToPrintDataset);
    if (status != EC_Normal)
    {
        ERROR_LOG("No s'ha pogut el Presentation State a partir del dataSet de l'imatge. Descripcio error: " + QString(status.text()));
        m_lastError = CreateDicomPrintSpool::ErrorCreatingImageSpool;
        return false;
    }

    // El 2n paràmete del attach image indica, si el presentation state és l'amo de la imatge passada per paràmetre, per poder destruir l'objecte,
    // en aquest cas l'indiquem que no és l'amo, per poder-lo destruir nosaltres.
    m_presentationState->attachImage(imageToPrintDcmFileFormat, false);

    if (imageToPrint->getFrameNumber() != 0)
    {
        //Si no és el primer frame el seleccionem. El número de Frame per dcmtk sempre comença a partir del 1 mentre per nosaltres comença a partir del 0,
        //per això sumem més 1
        m_presentationState->selectImageFrameNumber(imageToPrint->getFrameNumber() + 1);
    }

    applyDICOMPrintPresentationStateImage(m_presentationState, dicomPrintPresentationStateImage);

    bitmapSize = m_presentationState->getPrintBitmapSize();

    status = m_presentationState->getPrintBitmapWidthHeight(bitmapWidth, bitmapHeight);
    if (status != EC_Normal)
    {
        ERROR_LOG("No s'ha pogut obtenir l'amplada\alçada de la imatge. Descripcio error: " + QString(status.text()));
        m_lastError = CreateDicomPrintSpool::ErrorCreatingImageSpool;
        return false;
    }

    pixelAspectRatio = m_presentationState->getPrintBitmapPixelAspectRatio();
    pixelData = new char[bitmapSize];

    // El 3r paràmetre indica si la imatge s'ha de redenritzar amb el presentation LUT invers
    status = m_presentationState->getPrintBitmap(pixelData, bitmapSize, false);
    if (status == EC_Normal)
    {
        // Guardem la imatge a disc
        ok = createHardcopyGrayscaleImage(imageToPrint, pixelData, bitmapWidth, bitmapHeight, pixelAspectRatio, spoolDirectoryPath);
    }
    else
    {
        ERROR_LOG("No s'ha pogut obtenir el pixelData de la imatge transformada. Descripcio del error: " + QString(status.text()));
        m_lastError = CreateDicomPrintSpool::ErrorCreatingImageSpool;
    }

    // No fem delete del imageToPrintDataset perquè és un punter que apunta al Dataset de l'objecte imageToPrintDcmFileFormat del qual ja fem un delete
    delete m_presentationState;
    delete pixelData;
    delete imageToPrintDcmFileFormat;

    return ok;
}

bool CreateDicomPrintSpool::createHardcopyGrayscaleImage(Image *imageToPrint, const char *pixelData, unsigned long bitmapWidth, unsigned long bitmapHeight,
                                                         double pixelAspectRatio, const QString &spoolDirectoryPath)
{
    char InstanceUIDOfTransformedImage[70];
    OFString requestedImageSizeAsOFString;
    DcmFileFormat *transformedImageToPrint = new DcmFileFormat();
    DcmDataset *transformedImageDatasetToPrint = transformedImageToPrint->getDataset();
    QString transformedImagePath;
    OFCondition status;
    bool ok = false;

    // Write patient module
    status = m_presentationState->writeHardcopyImageAttributes(*transformedImageDatasetToPrint);
    if (status != EC_Normal)
    {
        ERROR_LOG("No s'han pogut gravar a la imatge per imprimir les dades del pacient");
        m_lastError = CreateDicomPrintSpool::ErrorCreatingImageSpool;
        return false;
    }

    // Write general study and general series module
    status = m_storedPrint->writeHardcopyImageAttributes(*transformedImageDatasetToPrint);
    if (status != EC_Normal)
    {
        ERROR_LOG("No s'han pogut gravar a la imatge per imprimir les dades de l'estudi i la serie");
        m_lastError = CreateDicomPrintSpool::ErrorCreatingImageSpool;
        return false;
    }

    // Hardcopy Equipment Module
    transformedImageDatasetToPrint->putAndInsertString(DCM_HardcopyDeviceManufacturer, qPrintable(ApplicationNameString), true);
    transformedImageDatasetToPrint->putAndInsertString(DCM_HardcopyDeviceSoftwareVersion, qPrintable(StarviewerVersionString), true);

    // General Image Module
    transformedImageDatasetToPrint->putAndInsertString(DCM_InstanceNumber, qPrintable(imageToPrint->getInstanceNumber()));
    transformedImageDatasetToPrint->putAndInsertString(DCM_PatientOrientation, 
        qPrintable(imageToPrint->getPatientOrientation().getDICOMFormattedPatientOrientation()));
    transformedImageDatasetToPrint->putAndInsertString(DCM_ImageType, "DERIVED\\SECONDARY", true);
    transformedImageDatasetToPrint->putAndInsertString(DCM_DerivationDescription, "Hardcopy rendered using Presentation State");

    // SOP Common Module
    transformedImageDatasetToPrint->putAndInsertString(DCM_SOPClassUID, UID_HardcopyGrayscaleImageStorage);

    dcmGenerateUniqueIdentifier(InstanceUIDOfTransformedImage);
    transformedImageDatasetToPrint->putAndInsertString(DCM_SOPInstanceUID, InstanceUIDOfTransformedImage);

    // Instance Creation Modukle
    transformedImageDatasetToPrint->putAndInsertString(DCM_InstanceCreationDate, qPrintable(QDateTime::currentDateTime().toString("yyyyMMdd")));
    transformedImageDatasetToPrint->putAndInsertString(DCM_InstanceCreationTime, qPrintable(QDateTime::currentDateTime().toString("hhmmss")));

    // Hardcopy Grayscale Image Module
    // El valor d'aquests tags són hard coded obtinguts del mètode saveHardcopyGrayscaleImage de dviface.cxx
    transformedImageDatasetToPrint->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_SamplesPerPixel, 1);
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_Rows, OFstatic_cast(Uint16, bitmapHeight));
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_Columns, OFstatic_cast(Uint16, bitmapWidth));
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_BitsAllocated, 16);
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_BitsStored, 12);
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_HighBit, 11);
    transformedImageDatasetToPrint->putAndInsertUint16(DCM_PixelRepresentation, 0);

    if (pixelAspectRatio != 1.0)
    {
        char pixelAspectRatioAsChar[70];

        sprintf(pixelAspectRatioAsChar, "%ld\\%ld", 1000L, OFstatic_cast(long, pixelAspectRatio * 1000.0));
        transformedImageDatasetToPrint->putAndInsertString(DCM_PixelAspectRatio, pixelAspectRatioAsChar);
    }

    DcmPolymorphOBOW *pxData = new DcmPolymorphOBOW(DCM_PixelData);

    if (pxData)
    {
        void *pixelDataAsVoid = OFconst_cast(char *, pixelData);
        pxData->putUint16Array(OFstatic_cast(Uint16 *, pixelDataAsVoid), OFstatic_cast(unsigned long, bitmapWidth * bitmapHeight));
        transformedImageDatasetToPrint->insert(pxData, OFTrue);

        if (m_presentationState->getPresentationLUT() == DVPSP_table)
        {
            // En principi no treballem amb presentation LUT, per tant aquest codi crec que no s'hauria d'executar mai
            INFO_LOG("Gravem presentation LUT");
            status = m_presentationState->writePresentationLUTforPrint(*transformedImageDatasetToPrint);
            if (status != EC_Normal)
            {
                ERROR_LOG("No s'ha pogut gravar el presentation LUT. Descripcio error" + QString(status.text()));
            }
        }

        // TODO:S'hauria de fer a un altre lloc aquest càlcul perquè també s'utilitza a PrintDicomSpool
        transformedImagePath = QDir::toNativeSeparators(spoolDirectoryPath) + QDir::separator() + InstanceUIDOfTransformedImage + ".dcm";
        // Guardem la imatge transformada
        status = DVPSHelper::saveFileFormat(qPrintable(transformedImagePath), transformedImageToPrint, true);

        if (status == EC_Normal)
        {
            INFO_LOG("Creada imatge per imprimir al path " + transformedImagePath);

            m_presentationState->getPrintBitmapRequestedImageSize(requestedImageSizeAsOFString);
            // Afegim la imatge al Image Box
            status = m_storedPrint->addImageBox(qPrintable(Settings().getValue(InputOutputSettings::LocalAETitle).toString()), InstanceUIDOfTransformedImage,
                                                requestedImageSizeAsOFString.c_str(), NULL, m_presentationState->getPresentationLUTData(),
                                                m_presentationState->isMonochrome1Image());

            if (status != EC_Normal)
            {
                m_lastError = CreateDicomPrintSpool::ErrorCreatingImageSpool;
                ERROR_LOG("No s'ha pogut afegir l'imatge al ImageBox de l'objecte DVPSStoredPrint. Descripcio error: " + QString(status.text()));
            }
            else
            {
                ok = true;
            }
        }
        else
        {
            m_lastError = CreateDicomPrintSpool::ErrorCreatingImageSpool;
            ERROR_LOG("No s'ha pogut gravar la imatge preparada per imprimir " + transformedImagePath + " . Descripcio error " + QString(status.text()));
        }
    }
    else
    {
        m_lastError = CreateDicomPrintSpool::ErrorCreatingImageSpool;
        ERROR_LOG("No s'ha pogut crear l'objecte DcmPolymorphOBOW, l'error sol venir perque no hi ha suficent memòria RAM lliure");
    }

    delete transformedImageToPrint;

    return ok;
}

void CreateDicomPrintSpool::setImageBoxAttributes()
{
    size_t numImages = m_storedPrint->getNumberOfImages();

    for (size_t i = 0; i < numImages; i++)
    {
        // Com atribut del Image Box només especifiquem la polaritat, ja que el Magnification Type (2010,0060), el Smoothing Type (2010,0080)
        // i el Configuration Information (2010,0150) tot i que es poden especificar a nivell de Image Box com aquest tag té el mateix valor per totes
        // les imatges del Film Box, s'especifica a nivell de Film Box, els altres tags del Image Box són emplenats per les dcmtk

        if (!m_dicomPrintPage.getPolarity().isEmpty())
        {
            m_storedPrint->setImagePolarity(i, qPrintable(m_dicomPrintPage.getPolarity()));
        }
    }

    INFO_LOG("Afegits els atributs al ImageBox");
}

void CreateDicomPrintSpool::createAnnotationBoxes()
{
    m_annotationBoxes = new DVPSAnnotationContent_PList();

    if (m_dicomPrintPage.getPageAnnotations().count() > 0)
    {
        foreach (int position, m_dicomPrintPage.getPageAnnotations().keys())
        {
            // Es genera un UID inventat per cada AnnotationBox, quan des de la classe PrintDICOMSpoool creem un FilmBox a la impressora aquesta
            // com a resposta ens retorna els UID's amb el qual hem d'enviar cada un dels Annotation Box. Això ho fa transparentment DVPSStoredPrint
            // al invocar el mètode printSCUcreateBasicFilmSession i assignar a cada AnnotationBox un UID vàlid. Si s'envien AnnotationBox amb un
            // UID que no ens ha indicat la impressora les anotacions s'ignoren
            char newuid[70];
            dcmGenerateUniqueIdentifier(newuid);
            m_annotationBoxes->addAnnotationBox(newuid, qPrintable(m_dicomPrintPage.getPageAnnotations().value(position)), position);
        }

        m_annotationDisplayFormatIDTagValue = m_dicomPrinter.getAnnotationDisplayFormatID();
    }
}

QString CreateDicomPrintSpool::createStoredPrintDcmtkFile(const QString &spoolDirectoryPath)
{
    DcmFileFormat *fileFormat = new DcmFileFormat();
    DcmDataset *dataset = fileFormat->getDataset();
    char storedPrintInstanceUID[70];
    dcmGenerateUniqueIdentifier(storedPrintInstanceUID);

    m_storedPrint->setInstanceUID(storedPrintInstanceUID);

    m_storedPrint->write(*dataset, false, OFTrue, OFFalse, OFTrue);

    // Si tenim anotacions les enviem
    if (m_annotationBoxes->size() > 0)
    {
        INFO_LOG("Hi ha anotacions per imprimir al FilmSession, creem les AnnotationBox");
        // HACK: DVPSStoredPrint només permet afegir una anotació al FilmBox a través del mètode setSingleAnnotation, com que la majoria de DICOMPrinters
        // permeten fins a 6 anotacions ens interessa evitar aquesta limitació. El que hem fet és guardar en la mateixa estructura que DVPSStoredPrint les
        // anotacions DVPSAnnotationContent_PList i llavors en el dataSet on gravem les dades de l'impressió de l'objecte m_storedPrint també hi gravem
        // les dades de les anotacions, d'aquesta manera podem tenir més d'una anotació en un FilmBox.
        // Quan des de PrintDICOMSpool es llegeix el dataset que hem generat en aquesta classe es troba que té més d'una anotació i les envia totes a imprimir.
        m_annotationBoxes->write(*dataset);

        DcmItem *sequenceFilmBox = NULL;
        dataset->findOrCreateSequenceItem(DCM_FilmBoxContentSequence, sequenceFilmBox);

        if (sequenceFilmBox)
        {
            // L'annotation Display Format només té sentit enviar-lo si tenim anotacions. Aquest camp serveix per indicar com volem que apareguin les anotacions
            // En Kodak, Fujifilm, Agfa,... si no especifiquem aquest tag al crear el BasicFilmBox la impressora no ens retorna els UID's amb les quals hem
            // d'enviar els Annotation Box. Per exemple per a Agfa ha de tenir el valor 'ANNOTATION' si no no ens retorna com a resposta els UID's amb els quals
            // s'han d'envies els Annotation box (DICOM Conformance de la dryStar 5500 pàgina 26
            // http://www.agfa.com/he/france/fr/binaries/000737_Drystar_5500_1.8%2C_2.0_%2C3.x_and_4.0_tcm224-21750.pdf) altres impressores com Kodak
            // aquest tag pog agafar diversos valors http://www.carestreamhealth.com/dv6800_dicom_9F2965.pdf
            sequenceFilmBox->putAndInsertString(DCM_AnnotationDisplayFormatID, qPrintable(m_annotationDisplayFormatIDTagValue), true);
        }
    }

    QString storedPrintDcmtkFilePath = QDir::toNativeSeparators(spoolDirectoryPath) + QDir::separator() + "SP_" + storedPrintInstanceUID + ".dcm";;
    OFCondition status = DVPSHelper::saveFileFormat(qPrintable(storedPrintDcmtkFilePath), fileFormat, true);

    if (!status.good())
    {
        ERROR_LOG(QString("Error al gravar el fitxer storedPrint a %1. Descripció error: %2").arg(storedPrintDcmtkFilePath, status.text()));
        storedPrintDcmtkFilePath = "";
    }
    else
    {
        INFO_LOG("Es guadar el fitxer del storedPrint(DVPSStoredPrint) al path " + QString(storedPrintDcmtkFilePath));
    }

    delete fileFormat;
    delete m_storedPrint;

    return storedPrintDcmtkFilePath;
}

CreateDicomPrintSpool::CreateDicomPrintSpoolError CreateDicomPrintSpool::getLastError()
{
    return m_lastError;
}

void CreateDicomPrintSpool::applyDICOMPrintPresentationStateImage(DVPresentationState *dvPresentationState,
                                                                  const DICOMPrintPresentationStateImage &dicomPrintPresentationStateImage)
{
    if (!dicomPrintPresentationStateImage.applyDefaultWindowLevelToImage())
    {
        int windowWidth;
        int windowCenter = dicomPrintPresentationStateImage.getWindowCenter();

        if (dicomPrintPresentationStateImage.getWindowWidth() < 0)
        {
            //Dcmtk només accepta Window Width positius, l'equivalència de Window Witdh negatiu a dcmtk és invertir els colors de la imatge i aplicar el mateix
            //window width amb signe positu
            dvPresentationState->invertImage();
            windowWidth = -dicomPrintPresentationStateImage.getWindowWidth();
        }
        else
        {
            windowWidth = dicomPrintPresentationStateImage.getWindowWidth();
        }

        OFCondition cond = dvPresentationState->setVOIWindow(windowCenter, windowWidth);

        if (!cond.good())
        {
            ERROR_LOG(QString("No s'ha pogut aplicar el WL a la imatge a imprimir WC:%1, WL:%2, Descripcio error: %3").
                      arg(QString::number(windowCenter), QString::number(windowWidth), QString(cond.text())));
        }
    }
}

}
