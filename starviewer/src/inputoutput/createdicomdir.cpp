/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <string>
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

namespace udg {

CreateDicomdir::CreateDicomdir()
{
    m_optProfile = DicomDirInterface::AP_GeneralPurpose;//PErmet gravar al discdur i tb usb's
}

void CreateDicomdir::setDevice( Device deviceToCreateDicomdir )
{
    //indiquem que el propòsit d'aquest dicomdir
    switch ( deviceToCreateDicomdir ) 
    {
        case harddisk :
            m_optProfile = DicomDirInterface::AP_GeneralPurpose;
            break;
        case cdDvd :
            m_optProfile = DicomDirInterface::AP_GeneralPurposeDVD;
            break;
        case usb :
            m_optProfile = DicomDirInterface::AP_USBandFlash;
            break;
        default : 
            m_optProfile = DicomDirInterface::AP_GeneralPurpose;
            break;
    }
}

Status CreateDicomdir::create( std::string dicomdirPath )
{
    std::string errorMessage , outputDirectory = dicomdirPath + "/DICOMDIR";//Nom del fitxer dicomDir
    OFList<OFString> fileNames;/* create list of input files */
    const char *opt_directory = dicomdirPath.c_str();
    const char *opt_pattern = NULL;
    const char *opt_output = outputDirectory.c_str();
    const char *opt_fileset = DEFAULT_FILESETID;
    const char *opt_descriptor = NULL;
    const char *opt_charset = DEFAULT_DESCRIPTOR_CHARSET;
    OFCondition result;
    DicomDirInterface ddir;
    E_EncodingType opt_enctype = EET_ExplicitLength;
    E_GrpLenEncoding opt_glenc = EGL_withoutGL;
    
    Status state;
    
    //busquem el fitxers al dicomdir. Anteriorment a la classe ConvertoToDicomdir s'han d'haver copiat els fitxers dels estudis seleccionats, al directori dicomdir destí
    OFStandard::searchDirectoryRecursively( "" , fileNames, opt_pattern , opt_directory );
         
    //comprovem que el directori no estigui buit
    if ( fileNames.empty() ) 
    {     
        ERROR_LOG ( "El directori origen està buit" );
        state.setStatus( " no input files: the directory is empty " , false , 1301 );
        return state;
    }

    //creem el dicomdir
    result = ddir.createNewDicomDir( m_optProfile , opt_output , opt_fileset );
    
    if ( !result.good() )
    {
        errorMessage = "Error al crear el DICOMDIR. ERROR : ";
        errorMessage.append( result.text() );
        ERROR_LOG ( errorMessage.c_str() );
        state.setStatus( result );
        return state;
    }

    /* set fileset descriptor and character set */
    result = ddir.setFilesetDescriptor( opt_descriptor , opt_charset );
    if ( result.good() )
    {
        OFListIterator( OFString ) iter = fileNames.begin();
        OFListIterator( OFString ) last = fileNames.end();

        //iterem sobre la llista de fitxer i els afegim al dicomdir
        while ( ( iter != last ) && result.good() )
        {
            //afegim els fitxers al dicomdir
            result = ddir.addDicomFile( (*iter).c_str() , opt_directory );
            if ( result.good() ) iter++;
        }

        if ( !result.good() ) 
        {
			errorMessage = "Error al convertir a DICOMDIR el fitxer : ";
			errorMessage.append((*iter).c_str());
        	ERROR_LOG ( errorMessage.c_str() );
            result = EC_IllegalCall;
        }
        else result = ddir.writeDicomDir ( opt_enctype , opt_glenc ); //escribim el dicomDir
    }

    return state.setStatus( result );
}

CreateDicomdir::~CreateDicomdir()
{
}

}
