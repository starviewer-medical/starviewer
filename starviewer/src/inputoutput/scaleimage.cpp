/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "scaleimage.h"

#define HAVE_CONFIG_H 1
 
//#define BUILD_DCM2PNM_AS_DCMJ2PNM // compile "dcm2pnm" with dcmjpeg support

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"          /* for various dcmdata headers */
#include "dcutils.h"       /* for getSingleValue */
#include "dcdebug.h"       /* for SetDebugLevel */
#include "cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcuid.h"         /* for dcmtk version name */
#include "dcrledrg.h"      /* for DcmRLEDecoderRegistration */

#include "dcmimage.h"      /* for DicomImage */
#include "digsdfn.h"       /* for DiGSDFunction */
#include "diciefn.h"       /* for DiCIELABFunction */

#include "ofconapp.h"      /* for OFConsoleApplication */
#include "ofcmdln.h"       /* for OFCommandLine */

#include "diregist.h"      /* include to support color images */
#include "ofstd.h"         /* for OFStandard */

#ifdef WITH_ZLIB
#include <zlib.h>         /* for zlibVersion() */
#endif

#include "ofstream.h"

#include "const.h"

#define OFFIS_OUTFILE_DESCRIPTION "output filename to be written (default: stdout)"

#ifdef BUILD_DCM2PNM_AS_DCMJ2PNM
# define OFFIS_CONSOLE_APPLICATION "dcmj2pnm"
# ifdef WITH_LIBTIFF
#  define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM images to PGM, PPM, BMP, TIFF or JPEG"
# else
#  define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM images to PGM, PPM, BMP or JPEG"
# endif
#else
# define OFFIS_CONSOLE_APPLICATION "dcm2pnm"
# ifdef WITH_LIBTIFF
#  define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM images to PGM, PPM, BMP or TIFF"
# else
#  define OFFIS_CONSOLE_DESCRIPTION "Convert DICOM images to PGM, PPM or BMP"
# endif
#endif

namespace udg {

ScaleImage::ScaleImage()
{
}

int ScaleImage::dicom2lpgm(const char* dicomFile, const char* lpgmFile,int pixelsSize)
{
//valors per defecte trets de dcm2pnm.cc
    int                 opt_readAsDataset = 0;            /* default: fileformat or dataset */
    E_TransferSyntax    opt_transferSyntax = EXS_Unknown; /* default: xfer syntax recognition */

    unsigned long       opt_compatibilityMode = CIF_MayDetachPixelData | CIF_TakeOverExternalDataset;
                                                          /* default: pixel data may detached if no longer needed */
    OFCmdUnsignedInt    opt_frame = 1;                    /* default: first frame */
    OFCmdUnsignedInt    opt_frameCount = 1;               /* default: one frame */
    int                 opt_useAspectRatio = 1;           /* default: use aspect ratio for scaling */
    OFCmdUnsignedInt    opt_useInterpolation = 1;         /* default: use interpolation method '1' for scaling */

    OFCmdUnsignedInt    opt_scale_size_x = 1;
    OFCmdUnsignedInt    opt_scale_size_y = 1;

    /* make sure data dictionary is loaded */
    if ( !dcmDataDict.isDictionaryLoaded ())
    {
        return errorDictionaryNoLoaded;
    }

   //obrim el fitxer dicom
    DcmFileFormat *dfile = new DcmFileFormat();
    OFCondition cond = dfile->loadFile( dicomFile , opt_transferSyntax , EGL_withoutGL , DCM_MaxReadLength , opt_readAsDataset );

    if ( cond.bad() ) return errorDicomFileNotFound;
   
    E_TransferSyntax xfer = dfile->getDataset()->getOriginalXfer();

    //carreguem el fitxer dicom a escalar
    DicomImage *di = new DicomImage( dfile , xfer , opt_compatibilityMode , opt_frame - 1 , opt_frameCount );
    
    if ( di == NULL ) return errorOutofMemory;

    if ( di->getStatus() != EIS_Normal ) return errorOpeningDicomFile;


    di->hideAllOverlays();
    //escalem l'imatge    
    DicomImage *newimage;
    
        
    //Escalem pel cantó més gran
    if ( di->getWidth() < di->getHeight() )
    {
        opt_scale_size_x = 0;
        opt_scale_size_y = pixelsSize;
     }
     else
     {
        opt_scale_size_x = pixelsSize;
        opt_scale_size_y = 0;  
     }
    
    di->setMinMaxWindow( 1 ); //Establim el VOI LUT, aquí indiquem que aquesta imatge és per visualitzar per una finestra, aplica filtres perquè es vegi correctament
    
    newimage = di->createScaledImage( opt_scale_size_x , opt_scale_size_y , ( int ) opt_useInterpolation , opt_useAspectRatio );
           
    if ( newimage==NULL ) 
    {
        return errorScalingImage;
    }
    else if ( newimage->getStatus() != EIS_Normal )
    {
        return errorScalingImage;
    }
    else
    {
        delete di;
        di = newimage;
    }

    //escribim la imatge escalada al nout fitxer
    int result = 0;
    FILE *ofile = NULL;
    unsigned int fcount = ( unsigned int )( ( ( opt_frameCount > 0 ) && ( opt_frameCount <= di->getFrameCount() ) ) ? opt_frameCount : di->getFrameCount() );

    for ( unsigned int frame = 0; frame < fcount; frame++ )
    {        
        ofile = fopen( lpgmFile, "wb" );
        if ( ofile == NULL )
        {
            return errorOpeningNewImage;
        }
            /* finally create PGM BMP file */
       result = di->writeRawPPM( ofile , 8 , frame );
       fclose( ofile );
       
       if ( !result ) return errorWritingNewImage;
   }
    
    delete di;

    // deregister RLE decompression codec
    DcmRLEDecoderRegistration::cleanup();

    return 0;
}

ScaleImage::~ScaleImage()
{
}

};
;
