/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "status.h"
#include "logging.h"

#include "createdicomdir.h"
#include "osconfig.h"     /* make sure OS specific configuration is included first */
#include "dctk.h"
#include "dcddirif.h"     /* for class DicomDirInterface */
#include "ofstd.h"        /* for class OFStandard */
#include "ofcond.h"       /* for class OFCondition */


#if defined (HAVE_WINDOWS_H) || defined(HAVE_FNMATCH_H)
#define PATTERN_MATCHING_AVAILABLE
#endif

#include "dicomtagreader.h"

namespace udg {

CreateDicomdir::CreateDicomdir()
{
    m_optProfile = DicomDirInterface::AP_GeneralPurpose;//PErmet gravar al discdur i tb usb's
}

void CreateDicomdir::setDevice( recordDeviceDicomDir deviceToCreateDicomdir )
{
    //indiquem que el propòsit d'aquest dicomdir
    switch ( deviceToCreateDicomdir )
    {
        case recordDeviceDicomDir(harddisk) :
            m_optProfile = DicomDirInterface::AP_GeneralPurpose;
            break;
        case recordDeviceDicomDir(cd) :
            m_optProfile = DicomDirInterface::AP_GeneralPurpose;
            break;
        case recordDeviceDicomDir(dvd) :
            m_optProfile = DicomDirInterface::AP_GeneralPurposeDVD;
            break;
        case recordDeviceDicomDir(usb) :
            m_optProfile = DicomDirInterface::AP_USBandFlash;
            break;
        default :
            m_optProfile = DicomDirInterface::AP_GeneralPurpose;
            break;
    }
}

void CreateDicomdir::setStrictMode(bool enabled)
{
    if ( enabled )
    {
        m_ddir.enableInventMode( OFFalse );//Rebutgem imatges que contingui tags de tipus 1 amb longitut 0
        m_ddir.disableEncodingCheck( OFFalse );//Rebutja Imatges que no compleixin l'estàndard dicom en la codificació de la informació dels pixels
        m_ddir.disableResolutionCheck( OFFalse );//rebutja imatges que no compleixin l'estàndard dicom en la codificació de la informació dels pixels
        m_ddir.enableInventPatientIDMode( OFFalse );//rebutgem imatges que no tinguin PatientID

        INFO_LOG( "Mode estricte ON" );
    }
    else // no volem mode estricte
    {
        m_ddir.enableInventMode( OFTrue) ;//si una imatge, no té algun tag de nivell 1, que són els tags obligatoris i que no poden tenir longitut 1, al crear el dicomdir se'ls inventa
        m_ddir.disableEncodingCheck( OFTrue) ;//Accepta Imatges que no compleixin l'estàndard dicom en la codificació de la informació dels pixels
        m_ddir.disableResolutionCheck( OFTrue );//Accepta Imatges que no compleixi la resolució espacial
        m_ddir.enableInventPatientIDMode( OFTrue );//en cas que una pacient no tingui PatientID se l'inventa

        INFO_LOG( "Mode estricte OFF" );
    }

}

Status CreateDicomdir::create( QString dicomdirPath )
{
    QString errorMessage , outputDirectory = dicomdirPath + "/DICOMDIR";//Nom del fitxer dicomDir
    OFList<OFString> fileNames;/* create list of input files */
    const char *opt_pattern = NULL;
    const char *opt_fileset = DEFAULT_FILESETID;
    const char *opt_descriptor = NULL;
    const char *opt_charset = DEFAULT_DESCRIPTOR_CHARSET;
    OFCondition result;
    E_EncodingType opt_enctype = EET_ExplicitLength;
    E_GrpLenEncoding opt_glenc = EGL_withoutGL;

    Status state;

    //busquem el fitxers al dicomdir. Anteriorment a la classe ConvertoToDicomdir s'han d'haver copiat els fitxers dels estudis seleccionats, al directori dicomdir destí
    OFStandard::searchDirectoryRecursively( "" , fileNames, opt_pattern , qPrintable(dicomdirPath) );

    //comprovem que el directori no estigui buit
    if ( fileNames.empty() )
    {
        ERROR_LOG ( "El directori origen està buit" );
        state.setStatus( " no input files: the directory is empty " , false , 1301 );
        return state;
    }

    //creem el dicomdir
    result = m_ddir.createNewDicomDir( m_optProfile , qPrintable(outputDirectory) , opt_fileset );

    if ( !result.good() )
    {
        errorMessage = "Error al crear el DICOMDIR. ERROR : ";
        errorMessage.append( result.text() );
        ERROR_LOG ( errorMessage );
        state.setStatus( result );
        return state;
    }

    /* set fileset descriptor and character set */
    result = m_ddir.setFilesetDescriptor( opt_descriptor , opt_charset );
    if ( result.good() )
    {
        OFListIterator( OFString ) iter = fileNames.begin();
        OFListIterator( OFString ) last = fileNames.end();

        //iterem sobre la llista de fitxer i els afegim al dicomdir
        while ( ( iter != last ) && result.good() )
        {
            //afegim els fitxers al dicomdir
            result = m_ddir.checkDicomFile( (*iter).c_str() , qPrintable(dicomdirPath) );
            result = m_ddir.addDicomFile( (*iter).c_str() , qPrintable(dicomdirPath) );
            if ( result.good() ) iter++;
        }

        if ( !result.good() )
        {
            QString imageErrorPath;

            imageErrorPath = dicomdirPath;
            imageErrorPath.append( "/" );
            imageErrorPath.append ( ( *iter ).c_str() );

			errorConvertingFile ( imageErrorPath );

            result = EC_IllegalCall;
        }
        else result = m_ddir.writeDicomDir ( opt_enctype , opt_glenc ); //escribim el dicomDir
    }

    return state.setStatus( result );
}

void CreateDicomdir::errorConvertingFile( QString imagePath )
{
    QString logMessage;
    DICOMTagReader dicomFile;

    if ( dicomFile.setFile(imagePath) )
    {
        logMessage = "Error al convertir a DICOMDIR el fitxer : ";
        logMessage.append( dicomFile.getAttributeByName(DCM_StudyInstanceUID) );
        logMessage.append( "/" );
        logMessage.append( dicomFile.getAttributeByName(DCM_SeriesInstanceUID) );
        logMessage.append( "/" );
        logMessage.append( dcmSOPClassUIDToModality( qPrintable(dicomFile.getAttributeByName(DCM_SOPClassUID)) ) );
        logMessage.append( "." );
        logMessage.append( dicomFile.getAttributeByName(DCM_SOPInstanceUID) );
    }
    else
    {
        logMessage = "Error al convertir a DICOMDIR el fitxer que es troba a la cache, al directori : ";
        logMessage.append( imagePath );
    }

    ERROR_LOG ( logMessage );
}

CreateDicomdir::~CreateDicomdir()
{
}

}
