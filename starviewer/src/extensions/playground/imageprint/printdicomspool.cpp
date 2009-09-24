#include "printdicomspool.h"

//TODO Fer neteja d'includes
#include "dcmtk/dcmpstat/dvpsdef.h"    /* for constants */
#include "dcmtk/dcmpstat/dviface.h"    /* for DVInterface */
#include "dcmtk/ofstd/ofstring.h"   /* for OFString */
#include "dcmtk/ofstd/ofbmanip.h"   /* for OFBitmanipTemplate */
#include "dcmtk/ofstd/ofdatime.h"   /* for OFDateTime */
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmdata/cmdlnarg.h"   /* for prepareCmdLineArgs */
#include "dcmtk/ofstd/ofconapp.h"   /* for OFConsoleApplication */
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmpstat/dvpspr.h"
#include "dcmtk/dcmpstat/dvpssp.h"
#include "dcmtk/dcmpstat/dvpshlp.h"     /* for class DVPSHelper */
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcdebug.h"

#include <QDir>

#include "logging.h"
#include "../inputoutput/pacsdevice.h"

namespace udg{

void PrintDicomSpool::printBasicGrayscale(DicomPrinter dicomPrinter, DicomPrintJob dicomPrintJob, const QString &storedPrintDcmtkFilePath, const QString &spoolDirectoryPath)
{
    m_dicomPrinter = dicomPrinter;
    m_dicomPrintJob = dicomPrintJob;

    m_storedPrintDcmtk = loadStoredPrintFileDcmtk(storedPrintDcmtkFilePath);

    printBasicGrayscaleSpool(spoolDirectoryPath);

    delete m_storedPrintDcmtk;
}

void PrintDicomSpool::printBasicGrayscaleSpool(const QString &spoolDirectoryPath)
{
    DVPSPrintMessageHandler printerConnection;
    OFCondition result;

    //TODO: podem utilitzar el del DVSStoredPrint
    //Connectem amb la impressora
    result = printerConnection.negotiateAssociation(NULL/*tlayer només s'utilitza per ssl*/, qPrintable(PacsDevice::getLocalAETitle()),
                                               qPrintable(m_dicomPrinter.getAETitle()), qPrintable(m_dicomPrinter.getHostname()), m_dicomPrinter.getPort(),
                                               ASC_DEFAULTMAXPDU, false /*targetSupportsPLUT*/, true /*targetSupportsAnnotation*/, true /*targetImplicitOnly*/);
    
    if (result.bad())
    {
        ERROR_LOG(QString("No s'ha pogut connectar amb la impressora amb AETitle: %1, IP: %2, port: %3, descripció error:%4 ").arg(m_dicomPrinter.getAETitle(), 
                           m_dicomPrinter.getHostname(), QString().setNum(m_dicomPrinter.getPort()), result.text()));

        return;
    }

    if (EC_Normal != (result = m_storedPrintDcmtk->printSCUgetPrinterInstance(printerConnection)))
    {
        //TODO: Request printer settings els podem obtenir ?
        DEBUG_LOG(QString("spooler: printer communication failed, unable to request printer settings. %1").arg(result.text()));
    }

    if (EC_Normal==result) if (EC_Normal != (result = m_storedPrintDcmtk->printSCUpreparePresentationLUT(
    printerConnection, true /*targetRequiresMatchingLUT*/, true/*targetPreferSCPLUTRendering*/, true/*targetSupports12bit*/)))
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to create presentation LUT.").arg(result.text()));
    }
    if (!createAndSendBasicFilmSession(printerConnection).good())
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to create basic film session. %1").arg(result.text()));
    }

    if (EC_Normal != (result = m_storedPrintDcmtk->printSCUcreateBasicFilmBox(printerConnection, true /*targetPLUTinFilmSession*/)))
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to create basic film box. %1").arg(result.text()));
    }

    for (size_t imageNumber = 0; imageNumber < m_storedPrintDcmtk->getNumberOfImages(); imageNumber++)
    {
        createAndSendBasicGrayscaleImageBox(printerConnection, imageNumber, spoolDirectoryPath);
    }

    if (EC_Normal != (result = m_storedPrintDcmtk->printSCUprintBasicFilmSession(printerConnection)))
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to print (at film session level). %1").arg(result.text()));
    } 
    else 
    {
        if (EC_Normal != (result = m_storedPrintDcmtk->printSCUprintBasicFilmBox(printerConnection)))
        {
            DEBUG_LOG(QString("spooler: printer communication failed, unable to print. %1").arg(result.text()));
        }
    }

    if (EC_Normal==result) if (EC_Normal != (result = m_storedPrintDcmtk->printSCUdelete(printerConnection)))
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to delete print objects %1.").arg(result.text()));
    }

    result = printerConnection.releaseAssociation();
    if (result.bad())
    {
        DEBUG_LOG(QString("spooler: release of connection to printer failed. %1").arg(result.text()));
    }
}

Status PrintDicomSpool::createAndSendBasicFilmSession(DVPSPrintMessageHandler& printerConnection)
{
    OFCondition result;
    DcmDataset datasetBasicFilmSession;
    DcmElement *attributeBasicFilmSession = NULL;
    Status state;

    attributeBasicFilmSession = new DcmCodeString(DCM_MediumType);
    attributeBasicFilmSession->putString(qPrintable(m_dicomPrintJob.getMediumType()));
    result = datasetBasicFilmSession.insert(attributeBasicFilmSession);

    attributeBasicFilmSession = new DcmCodeString(DCM_FilmDestination);
    attributeBasicFilmSession->putString(qPrintable(m_dicomPrintJob.getFilmDestination())); 
    datasetBasicFilmSession.insert(attributeBasicFilmSession);

    //TODO:Comprovar si l'utilitzarem  
    /*attributeBasicFilmSession = new DcmLongString(DCM_FilmSessionLabel);
    attributeBasicFilmSession->putString(printerFilmSessionLabel.c_str()); else result=EC_IllegalCall;
    datasetBasicFilmSession.insert(attributeBasicFilmSession, OFTrue replaceOld);
   */

    attributeBasicFilmSession = new DcmCodeString(DCM_PrintPriority);
    attributeBasicFilmSession->putString(qPrintable(m_dicomPrintJob.getPrintPriority()));
    datasetBasicFilmSession.insert(attributeBasicFilmSession);

    /*TODO:Si no m'equivoco només serveix per quan s'esborra el treball imprés del servidor d'impressió, si no l'especifiquem 
     el servidor d'impressió l'emplena automàticament*/
    /*attributeBasicFilmSession = new DcmShortString(DCM_OwnerID);
    attributeBasicFilmSession->putString(printerOwnerID.c_str());
    datasetBasicFilmSession.insert(attributeBasicFilmSession, OFTrue replaceOld);
    */

    attributeBasicFilmSession = new DcmIntegerString(DCM_NumberOfCopies);
    attributeBasicFilmSession->putString(qPrintable(QString().setNum(m_dicomPrintJob.getNumberOfCopies()))); 
    datasetBasicFilmSession.insert(attributeBasicFilmSession);

    result = m_storedPrintDcmtk->printSCUcreateBasicFilmSession(printerConnection, datasetBasicFilmSession, true /*plutInSession*/);
    
    return state.setStatus(result);
}

Status PrintDicomSpool::createAndSendBasicGrayscaleImageBox(DVPSPrintMessageHandler& printerConnection, size_t imageNumber, const QString &spoolDirectoryPath)
{
    OFCondition result;
    const char *studyUID = NULL, *seriesUID = NULL, *instanceUID = NULL;
    DicomImage *imageToPrint;
    QString imageToPrintPath;

    result = m_storedPrintDcmtk->getImageReference(imageNumber, studyUID, seriesUID, instanceUID);

    if (result != EC_Normal || !studyUID || !seriesUID || !instanceUID)
    {
        ERROR_LOG(QString("No s'ha trobat la imatge número %1 per imprimir").arg(QString().setNum(imageNumber)));
        return Status().setStatus(result);
    }

    //TODO:S'hauria de fer a un altre lloc el càlcul del path de la imatge perquè també s'utilitza a CreateDicomPrintSpool
    imageToPrintPath = QDir::toNativeSeparators(spoolDirectoryPath + QDir::separator() + instanceUID + ".dcm");
    imageToPrint = new DicomImage(qPrintable(imageToPrintPath));

    if (imageToPrint && EIS_Normal == imageToPrint->getStatus())
    {
        //Enviem la imatge
        result = m_storedPrintDcmtk->printSCUsetBasicImageBox(printerConnection, imageNumber, *imageToPrint, true/*opt_Monochrome1*/);
        if (EC_Normal != result)
        {
            DEBUG_LOG(QString("spooler: printer communication failed, unable to transmit basic grayscale image box. %1").arg(result.text()));
        }
    } 
    else 
    {
        DEBUG_LOG("spooler: unable to load image file " + imageToPrintPath );
    }
    
    //No s'ha de fer el delete del studyUID, seriesUID i instanceUID perquè són un punter a informació del storedPrint
    delete imageToPrint;
    
    return Status().setStatus(result);
}

DVPSStoredPrint* PrintDicomSpool::loadStoredPrintFileDcmtk(const QString &pathStoredPrintDcmtkFile)
{
    /*El constructor del DVPStoredPrint se li ha de passar com a paràmetres
        1r El tag (2010,015E) Illumination de la Basic Film Box
        2n El tag (2010,0160) Reflected Ambient Light de la Basic Film Box
        3r AETitle del Starviewer

        Els dos primer paràmetres només s'utilitzen si la impressora suporta el Presentation Lut, ara mateix no ho soportem (no està implementat) per tant se 
        suposa que aquests valors s'ignoraran. De totes maneres se li ha donat aquests valors per defecte 2000 i 10 respectivament perquè són els que utilitza dcmtk i també
        s'ha consultat el dicom conformance de les impressores agfa i kodak i també utiltizen aquests valors per defecte.
     */
    DVPSStoredPrint *storedPrint = new DVPSStoredPrint(2000, 10, qPrintable(PacsDevice::getLocalAETitle()));
    DcmFileFormat *storedPrintDcmtkFile = NULL;
    DcmDataset *datasetStoredPrintDcmtkFile = NULL;

    OFCondition result = DVPSHelper::loadFileFormat(qPrintable(QDir::toNativeSeparators(pathStoredPrintDcmtkFile)), storedPrintDcmtkFile);
    if (EC_Normal != result)
    {
        ERROR_LOG("No s'ha pogut obrir el fitxer StoredPrint de dcmtk, path:" + pathStoredPrintDcmtkFile);
        return NULL;
    }
    
    datasetStoredPrintDcmtkFile  = storedPrintDcmtkFile->getDataset();

    if (!datasetStoredPrintDcmtkFile)
    {
        ERROR_LOG("No s'ha pogut obtenir el dataset del fitxer StoredPrint de dcmtk");
        return NULL;
    }

    result = storedPrint->read(*datasetStoredPrintDcmtkFile); 

    if (EC_Normal != result)
    {
        ERROR_LOG("No s'ha pogut carregar les dades del StoredPrint per imprimir");
        return NULL;
    }

    delete storedPrintDcmtkFile;

    return storedPrint;
}
}