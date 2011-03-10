/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *

 ***************************************************************************/

#include "convertdicomtolittleendian.h"

#include <QString>
#include <osconfig.h>    /* make sure OS specific configuration is included first */
#include <ofstdinc.h>
#include <dctk.h>
#include <ofconapp.h>
#include <QDir>

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#include "status.h"
#include "logging.h"

namespace udg {

ConvertDicomToLittleEndian::ConvertDicomToLittleEndian()
{
}

ConvertDicomToLittleEndian::~ConvertDicomToLittleEndian()
{
}

Status ConvertDicomToLittleEndian::convert( QString inputFile , QString outputFile )
{
    DcmFileFormat fileformat;
    DcmDataset * dataset = fileformat.getDataset();
    OFCondition error;
    Status state;
    E_TransferSyntax opt_ixfer = EXS_Unknown; //Transfer Syntax del fitxer d'entrada
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_oxfer = EXS_LittleEndianExplicit;
    QString descriptionError;
    E_EncodingType opt_oenctype = EET_ExplicitLength;
    E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
    E_PaddingEncoding opt_opadenc = EPD_noChange;
    OFCmdUnsignedInt opt_filepad = 0;
    OFCmdUnsignedInt opt_itempad = 0;
    OFBool opt_oDataset = OFFalse;

    error = fileformat.loadFile( qPrintable( QDir::toNativeSeparators( inputFile ) ) , opt_ixfer, EGL_noChange , DCM_MaxReadLength , opt_readMode );

    if ( error.bad() ) 
    {
        ERROR_LOG(QString("No s'ha pogut obrir el fitxer a convertir LittleEndian %1, descripcio error: %2").arg(inputFile, error.text()));
        return state.setStatus( error );
    }
    dataset->loadAllDataIntoMemory();

    DcmXfer opt_oxferSyn( opt_oxfer );

    dataset->chooseRepresentation( opt_oxfer , NULL );

    if ( !dataset->canWriteXfer( opt_oxfer ) )
    {
        descriptionError =  "Error: no conversion to transfer syntax " + QString( opt_oxferSyn.getXferName() ) + " possible";
        state.setStatus( qPrintable(descriptionError) , false , 1300 );
        ERROR_LOG(descriptionError);
        return state;
    }

    error = fileformat.saveFile( qPrintable( QDir::toNativeSeparators( outputFile ) ) , opt_oxfer , opt_oenctype , opt_oglenc , opt_opadenc , OFstatic_cast( Uint32 , opt_filepad ) , OFstatic_cast( Uint32 , opt_itempad ) , opt_oDataset );

    if (!error.good())
    {
        ERROR_LOG(QString("S'ha produit un error al intentar gravar la imatge %1 convertida a LittleEndian al path %2, descripcio error: %3").arg(inputFile, outputFile, error.text()));
    }

    return state.setStatus( error );
}

}
