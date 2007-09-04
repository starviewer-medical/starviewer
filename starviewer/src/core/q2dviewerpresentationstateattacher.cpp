/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerpresentationstateattacher.h"

#include "logging.h"
#include "volume.h"
#include "q2dviewer.h"
#include "q2dviewerblackboard.h"
#include "shutterfilter.h"
#include "series.h"
#include "image.h"

// vtk
#include <vtkPoints.h>
// displayed area transform
#include <vtkImageChangeInformation.h>
#include <vtkImageResample.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>

#include <vtkImageActor.h>

// grayscale input del viewer
#include <vtkImageMapToWindowLevelColors.h>

// grayscale pipeline
#include <vtkWindowLevelLookupTable.h>
#include <vtkImageMapToColors.h>
#include <vtkImageShiftScale.h>

// dcmtk: presentation state
//\TODO hem de fer aquest define perquè sinó no compila. Caldria descobrir perquè passa això i si cal fer un altre include previ
#define HAVE_CONFIG_H 1
#include <dvpstat.h> // DVPresentationState
#include <dcmtk/dcmpstat/dvpstx.h>  /* for DVPSTextObject */
#include "dcmtk/dcmpstat/dvpsgr.h"  /* for DVPSGraphicObject */
#include "dcmtk/dcmpstat/dvpscu.h"  /* for DVPSCurve */
#include "dcmtk/dcmdata/dcdatset.h"

namespace udg {

Q2DViewerPresentationStateAttacher::Q2DViewerPresentationStateAttacher( Q2DViewer *viewer, const QString &presentationStateFile, QObject *parent)
 : QObject(parent), m_presentationStateFilename(""), m_presentationStateData(0), m_viewerInput(0), m_board(0), m_modalityLUTRescale(0), m_modalityLut(0), m_windowLevelLut(0), m_presentationLut(0)
{
    m_2DViewer = viewer;
    if( m_2DViewer )
    {
        m_viewerInput = m_2DViewer->getInput();
        m_board = new Q2DViewerBlackBoard( m_2DViewer );
    }
    else
    {
        DEBUG_LOG("El viewer proporcionat és NUL!");
    }
    m_presentationStateFilename = presentationStateFile;
    m_presentationStateHandler = new DVPresentationState;
}

Q2DViewerPresentationStateAttacher::~Q2DViewerPresentationStateAttacher()
{
    //\TODO cal alliberar els DcmFileFormat de m_attachedDatasetsList?
}

void Q2DViewerPresentationStateAttacher::setPresentationStateFileAndAttach( const QString &presentationStateFile )
{
    m_presentationStateFilename = presentationStateFile;
    this->detach();
    this->attach();
}

bool Q2DViewerPresentationStateAttacher::attach()
{
    bool ok = true;
    DcmFileFormat presentationStateFile;
    OFCondition status;
    if( presentationStateFile.loadFile( qPrintable( m_presentationStateFilename ) ).good() )
    {
        m_presentationStateData = presentationStateFile.getDataset();
        if( m_presentationStateData )
        {
            status = m_presentationStateHandler->read( *m_presentationStateData );
            if( status.good() )
            {
                DEBUG_LOG("ºººººººººººººººººº LLEGINT PRESENTATION STATE ºººººººººººººººººº");
                int references = m_presentationStateHandler->numberOfImageReferences();
                int i;
                DEBUG_LOG( QString("Nombre de referències del presentation state: %1").arg( references ) );
                bool isThisReferenced = false;
                for( i = 0; i < references; i++ )
                {
                    OFString studyUID, seriesUID, sopClassUID, instanceUID, frames, aetitle, filesetID, filesetUID;
                    m_presentationStateHandler->getImageReference(
                        i,
                        studyUID,  //  the Study Instance UID is returned in this string
                        seriesUID,  // the Series Instance UID is returned in this string
                        sopClassUID,  //     the SOP Class UID is returned in this string
                        instanceUID,  //     the SOP Instance UID is returned in this string
                        frames,  //  the list of frames is returned in this string
                        aetitle,  //     the series retrieveAETitle is returned in this string
                        filesetID,  //   the series storageMediaFileSetID is returned in this string
                        filesetUID  //  the series storageMediaFileSetUID is returned in this string
                    );

                    DEBUG_LOG( QString("Dades per l'element refernciat %1:\nStudy UID: %2\nSeries UID: %3\nSOP Class UID: %4\nInstance UID: %5\nFrames: %6\nAETitle: %7\nFileset ID: %8\nFileset UID: %9 ").arg( i )
                            .arg( studyUID.c_str() )
                            .arg( seriesUID.c_str() )
                            .arg( sopClassUID.c_str() )
                            .arg( instanceUID.c_str() )
                            .arg( frames.c_str() )
                            .arg( aetitle.c_str() )
                            .arg( filesetID.c_str() )
                            .arg( filesetUID.c_str() )
                            );
                    // comprovem si aquest presentation state fa referència al volum actual
                    if( !isThisReferenced && m_viewerInput->getSeries()->getInstanceUID() == QString( seriesUID.c_str() ) )
                        isThisReferenced = true;
                }
                if( isThisReferenced )
                {
                    DEBUG_LOG( "This volume is almost referenced once from the current presentation state" );
                }
                else
                {
                    DEBUG_LOG( "WARNING! This volume IS NOT referenced by the current presentation state" );
                }
                // carreguem la llista d'imatges que adjuntarem en un moment o altre segons convingui al presentation state
                QStringList fileList = m_viewerInput->getInputFiles();
                int images = fileList.size();
                DcmFileFormat *dicomSlice = NULL;
                for( int i = 0; i < images; i++ )
                {
                    dicomSlice = new DcmFileFormat;
                    dicomSlice->loadFile( qPrintable( fileList.at(i) ) );
                    m_attachedDatasetsList.push_back( dicomSlice );
                }
                // abans d'aplicar el presentation state cal fer un reset, pot ser que la imatge l'hàgim mogut, flipat, rotat, etc i això afectaria la processament de les transformacions espacials
                m_2DViewer->reset();
                // ATENCIÓ!!!!
                // Nosaltres canviarem una mica l'ordre de transformacions. El pipeline de grisos l'aplicarem un cop hàgim fet la resta de transformacions que ens toquen les dades,per tant el grayscale el farem al final

                //
                // Pipeline de transformacions de l'escala de grisos
                //
                applyGrayscaleTransformPipeline();

                //
                // 5. Image Annotation
                //

                applyShutterTransformation();
                // no l'executem perquè simplement llegeix informació i prou
//                 applyOverlayPlanes(); // això ben bé no sabem on ho hem de posar

                //
                // 6. Image Annotation ( pixel space )
                //
                applyPreSpatialTransformAnnotation();

                //
                // 7. Spatial Transformation
                //
                applySpatialTransformation();

                //
                // 8. Displayed Area
                //
                applyDisplayedAreaTransformation();

                //
                // 9. Image Annotation ( display relative )
                //
                applyPostSpatialTransformAnnotation();


                // extra
    //             const char *c;
    //             std::cout << "Attached image SOP Class UID: ";
    //             c = m_presentationStateHandler->getAttachedImageSOPClassUID();
    //             if( c )
    //                 std::cout << c << std::endl;
    //             else
    //                 std::cout << "(none) "<< std::endl;
    //             std::cout << "Attached image SOP Instance UID: ";
    //             if( c )
    //                 std::cout << c << std::endl;
    //             else
    //                 std::cout << "(none) "<< std::endl;

                m_2DViewer->refresh();
            }
            else
            {
                ok = false;
                ERROR_LOG( QString("Ha fallat la lectura del presentation state. Missatge d'error dcmtk: [%1]. Segurament l'arxiu no és un presentation state vàlid.").arg( status.text() ) );
                //\TODO aquí es podria fer un 'dcmpschk' i deixar la sortida en el log o mostrar un diàleg amb el missatge amb la sortida del dcmpschk
            }
        }
        else
            ok = false;
    }
    else
    {
        // potser el que caldria fer és carregar les parts del presentation state que porti la imatge en sí
        ok = false;
    }

    return ok;
}

bool Q2DViewerPresentationStateAttacher::detach()
{
    bool ok = false;
    if( m_board )
    {
        m_board->clear();
        m_presentationStateHandler->detachImage();
        m_attachedDatasetsList.clear();
        if( m_modalityLut )
        {
            m_modalityLut->Delete();
            m_modalityLut = 0;
        }
        if( m_windowLevelLut )
        {
            m_windowLevelLut->Delete();
            m_windowLevelLut = 0;
        }
        if( m_presentationLut )
        {
            m_presentationLut->Delete();
            m_presentationLut = 0;
        }
        ok = true;
    }
    return ok;
}

void Q2DViewerPresentationStateAttacher::applyShutterTransformation()
{
    DEBUG_LOG("^^^^^^^^^^^^^^ Aplicant SHUTTERS ^^^^^^^^^^^^^^");

    ShutterFilter *shutter = new ShutterFilter;
    Volume *dummy = new Volume( m_2DViewer->getWindowLevelMapper()->GetOutput() );
    //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
    dummy->setImages( m_viewerInput->getImages() );
    shutter->setInput( dummy );
    shutter->setPresentationStateShutters( qPrintable( m_presentationStateFilename ) );
    vtkImageData *output = shutter->getOutput();
    if( output )
    {
        m_2DViewer->getImageActor()->SetInput( output );
    }
}


void Q2DViewerPresentationStateAttacher::applyGrayscaleTransformPipeline()
{
    applyModalityLUT();
//     applyMaskSubstraction();
    applyVOILUT();

    // IMPORTANT !!!!!!!!!!!!!!!!!!!!
    // quan la imatge és MONOCHROME1 caldrà invertir els valors de sortida de la VOI lut. Quan tenim un presentation state en principi no cal fer cas de la Photometric Interpretation ja que el PS redefineix per sí sol totes les transformacions grayscale i la Photom. Int. afecta a la imatge només. El problema és si les gdcm transformen MONOCHROME1 a MONOCHROME2 caldrà actuar una mica diferent.
    applyPresentationLUT();

    if( m_viewerInput->getImages().at(0)->getPhotometricInterpretation() == "MONOCHROME1" )
    {
        DEBUG_LOG("La imatge és MONOCHROME1: ¿invertim les dades després de la VOI LUT i abans de la presentation LUT?... Si hi ha presentation state això no s'hauria de fer!");
        if( m_presentationLut )
        {
            m_presentationLut->InverseVideoOn();
            DEBUG_LOG("Inverteixo la PRESENTATION LUT");
        }
        else if( m_windowLevelLut )
        {
            m_windowLevelLut->InverseVideoOn();
            DEBUG_LOG("Inverteixo la VOI LUT");
        }
        else if( m_modalityLut )
        {
            // \TODO potser és millor fer servir un window negatiu en comptes de l'inverse video, tal com diu en la docu del propi mètode SetInverseVideo()
            m_modalityLut->InverseVideoOn();
            DEBUG_LOG("Inverteixo la MODALITY LUT");
        }
    }

    m_2DViewer->applyGrayscalePipeline();
    m_2DViewer->updateWindowLevelAnnotation();
}


void Q2DViewerPresentationStateAttacher::applyModalityLUT()
{
    // If the Modality LUT or equivalent Attributes are part of both the Image and the Presentation State, then the Presentation State Modality LUT shall be used instead of the Image Modality LUT or equivalent Attributes in the Image. If the Modality LUT is not present in the Presentation State it shall be assumed to be an identity transformation. Any Modality LUT or equivalent Attributes in the Image shall not be used.

    // busquem si existeix una modality lut
    m_modalityLut = this->parseLookupTable( 0 );
    if( m_modalityLut )
    {
        m_modalityRange[0] = m_modalityLut->GetTableRange()[0];
        m_modalityRange[1] = m_modalityLut->GetTableRange()[1];
        if( m_modalityLUTRescale )
        {
            m_modalityLUTRescale->Delete();
            m_modalityLUTRescale = 0;
        }
        m_2DViewer->setModalityLUT( m_modalityLut );
    }
    // si no hi ha lut busquem els paràmetres de rescale
    else
    {
        if( m_presentationStateData->tagExists( DCM_RescaleIntercept ) )
        {
            double rescaleSlope, rescaleIntercept;
            m_presentationStateData->findAndGetFloat64( DcmTagKey(DCM_RescaleSlope) , rescaleSlope );
            m_presentationStateData->findAndGetFloat64( DcmTagKey(DCM_RescaleIntercept) , rescaleIntercept );

            m_modalityLUTRescale = vtkImageShiftScale::New();
            switch( m_viewerInput->getImages().at(0)->getBitsStored() )
            {
            case 8:
                if( m_viewerInput->getImages().at(0)->getPixelRepresentation() == 0 )
                    m_modalityLUTRescale->SetOutputScalarTypeToUnsignedChar();
                else
                    m_modalityLUTRescale->SetOutputScalarTypeToChar();
            break;
            case 16:
                if( m_viewerInput->getImages().at(0)->getPixelRepresentation() == 0 )
                    m_modalityLUTRescale->SetOutputScalarTypeToUnsignedInt();
                else
                    m_modalityLUTRescale->SetOutputScalarTypeToInt();
            break;
            }
            m_modalityLUTRescale->SetScale( rescaleSlope );
            m_modalityLUTRescale->SetShift( rescaleIntercept );

            m_modalityLUTRescale->SetInput( m_viewerInput->getVtkData() );
            m_modalityLUTRescale->Update();

            m_modalityRange[0] = m_modalityLUTRescale->GetOutput()->GetScalarRange()[0];
            m_modalityRange[1] = m_modalityLUTRescale->GetOutput()->GetScalarRange()[1];

            DEBUG_LOG( QString("Presentation State Modality LUT Adjustment: Rescale Slope %1, Rescale Intercept %2")
                .arg( rescaleSlope )
                .arg( rescaleIntercept )
             );

//             m_2DViewer->setModalityRescale( rescaleSlope, rescaleIntercept );
            m_2DViewer->setModalityRescale( m_modalityLUTRescale );
        }
        else
        {
            // si no n'hi ha cap no cal que apliquem ni m_modalityLUTRescale ni m_modaltyLUT al pipeline, per tant tindrem l'input inicial pel següent pas del pipeline. Això ho fem per assegurar en cas que es re-executi després d'haver introduit canvis i transformacions
            if( m_modalityLUTRescale )
            {
                m_modalityLUTRescale->Delete();
                m_modalityLUTRescale = 0;
            }
            m_modalityRange[0] = m_viewerInput->getVtkData()->GetScalarRange()[0];
            m_modalityRange[1] = m_viewerInput->getVtkData()->GetScalarRange()[1];
            DEBUG_LOG( "There's no Modality LUT nor in DicomDataset nor in Presentation State File " );
        }
    }
}

void Q2DViewerPresentationStateAttacher::applyMaskSubstraction()
{
    //
    // 2. Mask Substraction
    //
    //             The mask transformation may be applied in the case of multi-frame images for which other frames at a fixed frame position or time interval relative to the current frame may be subtracted from the current frame. Multiple mask frames may be averaged, and sub-pixel shifted before subtraction.
    //             The result will be a signed value with a bit length one longer than the source frames.
    // When there is no difference between corresponding pixel values, the subtracted image pixel will have a value of 0.
    // If a pixel in the current frame has a greater value than in the mask frame, then the resulting frame shall have a positive value. If it has a lesser value, then the resulting frame shall have a negative value.

    // aquests canvis s'apliquen a totes les imatges
}

void Q2DViewerPresentationStateAttacher::applyVOILUT()
{
    //
    // 3. VOI LUT
    //
    // If a VOI LUT is part of both the Image and the Presentation State then the Presentation State VOI LUT shall be used instead of the Image VOI LUT. If a VOI LUT (that applies to the Image) is not present in the Presentation State , it shall be assumed to be an identity transformation. Any VOI LUT or equivalent values in the Image shall not be used.

    // aquests canvis es poden aplicar a un subconjunt de imatges/frames. Per tant podem tenir diverses VOI LUT per una mateixa sèrie que s'apliquen a diverses imatges.

    // mirem totes les VOI LUT possibles que hi hagi per cadascuna de les imatges/frames
    int images = m_attachedDatasetsList.size();
    int sliceNumber = 0;
    for( int i = 0; i < images; i++ )
    {
        DEBUG_LOG( QString("Attaching image #%1").arg( i ) );
        m_presentationStateHandler->attachImage( m_attachedDatasetsList[i], false );
        // ara hem de mirar quants frames té la imatge
        unsigned long frames;
        OFCondition status = m_presentationStateHandler->getImageNumberOfFrames(frames);
        if( status.bad() )
        {
            DEBUG_LOG( QString("Error a l'hora de llegir el nombre de frames de la imatge adjuntada::[%1]").arg( status.text() ) );
        }
        else
        {
            DEBUG_LOG( QString("Nombre de frames en la imatge adjuntada: %1").arg(frames) );
        }
        for( unsigned long j = 0; j < frames; j++ )
        {
            //\TODO falta tractar l'associació de cada VOILUT per cada Imatge Frame

            // primer busquem voi lut, sinó busquem window level
            m_windowLevelLut = this->parseLookupTable(1);
            // si hi ha una VOI LUT
            if( m_windowLevelLut )
            {
                // Ara podem considerar els tres casos anteriors de la modality: hi ha rescale, hi ha lut o no hi ha res. el 1r i el tercer es tracten igual.
                if( m_modalityLut )
                {
                    DEBUG_LOG("Encara no sabem què fer en aquest cas (concatenar modality + voi LUT)");
                }
                else
                {
                    m_window = m_windowLevelLut->GetTableRange()[1] - m_windowLevelLut->GetTableRange()[0];
                    m_level = m_window / 2.0;
                }
                m_2DViewer->setVOILUT( m_windowLevelLut );
            }
            else
            {
                const char *c;
                if( m_presentationStateHandler->haveActiveVOIWindow() )
                {
                    m_presentationStateHandler->getCurrentWindowWidth( m_window );
                    m_presentationStateHandler->getCurrentWindowCenter( m_level );
                    m_2DViewer->setDefaultWindowLevel( m_window, m_level );

                    DEBUG_LOG( QString("Presentation State VOI Adjustment: Window: %1, Level: %2")
                        .arg( m_window )
                        .arg( m_level )
                        );

                    c = m_presentationStateHandler->getCurrentVOIDescription();
                    if( c )
                        DEBUG_LOG( QString("Window/Level Description: ") + QString(c) )
                    else
                        DEBUG_LOG( "Window/Level has no Description" );
                }
                //\TODO això en principi no cal perquè ja ho fem abans
                else if( m_presentationStateHandler->haveActiveVOILUT() )
                {
                    c = m_presentationStateHandler->getCurrentVOIDescription();
                    if( c )
                        DEBUG_LOG( QString("Window/Level LUT Description: %1 ").arg( c ) )
                    else
                        DEBUG_LOG( "Window/Level LUT has no Description" );
                    // crear la LUT i assignar-li al filtre
                    m_windowLevelLut = this->parseLookupTable( 1 );
                }
                else
                {
                    // ajustar un al rang de dades adequat
                    m_window = fabs( m_modalityRange[1] - m_modalityRange[0] );
                    m_level = ( m_modalityRange[1] + m_modalityRange[0] )/ 2.0;
                    m_2DViewer->setDefaultWindowLevel( m_window, m_level );

                    DEBUG_LOG( QString("No Presentation VOI Adjustment, creating a nice and automatic one: Window: %1, Level: %2")
                    .arg( m_window )
                    .arg( m_level )
                    );
                }
                // això és per si tenim en compte que poden haver-hi més d'una VOI lut tant en la imatge com en el presentation state
                // de moment no farem cas de si en tenim més d'una

                // \TODO Caldria guardar una llista de window levels a escollir, com en la classe VolumeSourceInformation
                //     size_t max;
                //     max = m_presentationStateHandler->getNumberOfVOIWindowsInImage();
                //     std::cout << "VOI windows available in attached image: " << max << std::endl;
                //     for( size_t winidx = 0; winidx < max; winidx++ )
                //     {
                //         std::cout << "  Window #" << winidx+1 << ": description=";
                //         c = m_presentationStateHandler->getDescriptionOfVOIWindowsInImage( winidx );
                //         if( c )
                //             std::cout << c << std::endl;
                //         else
                //             std::cout << "(none)" << std::endl;
                //     }
                //
                //     max = m_presentationStateHandler->getNumberOfVOILUTsInImage();
                //     std::cout << "VOI LUTs available in attached image: " << max << std::endl;
                //     for( size_t lutidx = 0; lutidx < max; lutidx++ )
                //     {
                //         std::cout << "  LUT #" << lutidx+1 << ": description=";
                //         c = m_presentationStateHandler->getDescriptionOfVOILUTsInImage( lutidx );
                //         if( c )
                //             std::cout << c << std::endl;
                //         else
                //             std::cout << "(none)" << std::endl;
                //     }
            }
        }
    }
}

void Q2DViewerPresentationStateAttacher::applyPresentationLUT()
{
    //
    // 4. Presentation LUT
    //
    //             A Softcopy Presentation LUT Module is always present in a Presentation State. If a Presentation LUT is present in the Image then the Presentation State Presentation LUT shall be used instead of the Image Presentation LUT.

    // aquests canvis s'apliquen a totes les imatges de la sèrie

    // només hi ha presentation lut en els presentation states
    const char *c;
    if( m_presentationStateHandler->havePresentationLookupTable() )
    {
        m_presentationLut = parseLookupTable( 2 );
        c = m_presentationStateHandler->getPresentationLUTExplanation();
        if( c )
            DEBUG_LOG( QString( "Presentation LUT Explanation: %1" ).arg( c ) )
        else
            DEBUG_LOG( "Presentation LUT has no Explanation." );
        m_2DViewer->setPresentationLUT( m_presentationLut );
    }
    else
    {
        // no hi ha lut, és possible que hi hagi presentation lut shape
        //\TODO aquí en principi no cal fer res d'això perquè si l'imatge és MONOCHROME1 les gdcm ja fan la conversió per què sigui MONOCHROME2, per tant podem obviar aquests paràmetres. El problema el tenim quan és MONOCHROME1 i tenim una modality/presentation lut amb valors invertits, caldria invertir la lut.
        const char *lutShape;
        m_presentationStateData->findAndGetString( DCM_PresentationLUTShape, lutShape );
        QString lutShapeStr( lutShape );
        if( lutShapeStr == "IDENTITY" )
        {
        }
        else if( lutShapeStr == "INVERSE" )
        {
        }
        DEBUG_LOG( QString("Presentation LUT SHAPE:: %1").arg(lutShapeStr) );
    }
}

vtkWindowLevelLookupTable *Q2DViewerPresentationStateAttacher::parseLookupTable( int type )
{
    DcmTagKey lutType;
    vtkWindowLevelLookupTable *vtkLut = 0;
    QString lutDescription;
    bool signedRepresentation = false;
    if( m_viewerInput->getImages().at(0)->getPixelRepresentation() == 0 ) // signed
            signedRepresentation = true;
    switch( type )
    {
    case 0:
        lutType = DCM_ModalityLUTSequence;
        lutDescription = "Modality LUT";
    break;

    case 1:
        lutType = DCM_VOILUTSequence;
        lutDescription = "Window Level LUT";
    break;

    case 2:
        lutType = DCM_PresentationLUTSequence;
        lutDescription = "Presentation LUT";
    break;
    }
    DcmStack stack;
    bool ok = false;
    if( m_presentationStateData->search( lutType, stack ).good() )
    {
        ok = true;
        DEBUG_LOG( QString("Parsing %1 from Presentation state file").arg( lutDescription ) );
    }
    if( ok )
    {
        DcmSequenceOfItems *lutSequence = NULL;
        lutSequence = OFstatic_cast( DcmSequenceOfItems *,stack.top() );
        DcmItem *item = lutSequence->getItem( 0 );
        // obtenim la descripció de la lut que ens especifica el format d'aquesta
        const Uint16 *lutDescriptor;
        OFCondition status = item->findAndGetUint16Array( DcmTagKey( DCM_LUTDescriptor ) , lutDescriptor  );
        if( status.good() )
        {
            // 0: # d'entrades a la lookup table. Quan val 0 equival a 2^16 ( 65536 )

            // 1: FirstStoredPixelValueMapped: El valor del primer valor de pixel de la imatge mapejat. Aquest valor es mapeja a la primera entrada de la lut. Tots els valors de pixel de la imatge menors que aquest es mapejen amb el valor de la primera entrada de la lut. Un pixel amb valor FirstStoredPixelValueMapped + 1, es mapejarà amb la següent entrada de la lut i així fins arribar a la última entrada de la lut. Els valors per sobre de la última entrada també es mapejen amb el valor de l'última entrada de la lut. Aquest valor estableix la relació entre els valors de la imatge i les entrades de la lut.
            // Per exemple, FirstStoredPixelValueMapped = 63488. Els valors de la imatge que estiguin per sota d'aquest se'ls assignarà el valor de la primera entrada de la lut (lut[0]). FirstStoredPixelValueMapped+1 es correspondrà amb lut[1], etc fins FirstStoredPixelValueMapped+n=> lut[n-1], on n seran el nombre d'entrades de la lut. Els valors de pixel més grans que FirstStoredPixelValueMapped+n-1 es mapejaran amb el valor de lut[n-1].

            // 2: Especifica el nombre de bits per cada entrada a la lut, que podran ser 8 o 16, corresponent amb el rang de valors de la lut que podrà ser de 256 o 65536 respectivament

            // allotjem la memòria per les dades de la lut
            int numberOfEntries;
            if( lutDescriptor[0] == 0 )
                numberOfEntries = 65535;
            else
                numberOfEntries =  lutDescriptor[0];
            signed int firstStored;
            if( signedRepresentation )
                firstStored = static_cast<signed short>( lutDescriptor[1] );
            else
                firstStored = lutDescriptor[1];

            DEBUG_LOG( QString("LUT Descriptor: %1\\%2\\%3")
            .arg( numberOfEntries )
            .arg( firstStored )
            .arg( lutDescriptor[2] )
            );

            vtkLut = vtkWindowLevelLookupTable::New();
            vtkLut->SetNumberOfTableValues( numberOfEntries );
            vtkLut->SetTableRange( firstStored , firstStored + numberOfEntries );

            // Encara que lutDescriptor[2] pugui ser 8,10,12 o 16 bits, sempre llegirem en un vector de components de 16 bits per comoditat
            const Uint16 *lutData16;
            lutData16 = new Uint16[ numberOfEntries ];
            // obtenim les dades de la lut
            status = item->findAndGetUint16Array( DcmTagKey( DCM_LUTData ) , lutData16  );
            if( status.good() )
            {
                unsigned int min = 65535, max = 0;
                for( int i = 0; i < numberOfEntries; i++ )
                {
                    if( lutData16[i] > max )
                        max = lutData16[i];
                    if( lutData16[i] < min )
                        min = lutData16[i];
                }
//                 std::cout << std::endl << std::endl << "************ COMENÇUT **************" << std::endl << std::endl;
//                 for( int i =0; i < numberOfEntries; i++ )
//                 {
//                     double value;
//                     if( m_windowLevelLut ) // ens precedeix una VOI lut
//                         value = m_windowLevelLut->GetTableValue( i )[0];
//                         //value = m_windowLevelLut->GetLuminance( i );
//                     else if( m_modalityLut ) // només ens precedeix una modality
//                         value = m_modalityLut->GetTableValue( i )[0];
//                         //value = m_modalityLut->GetLuminance( i );
//                     else // no hi ha cap lut precedent
//                         value = (double)lutData16[ i ]/max;
// //                     std::cout << "Value que coloco a l'índex[" << i << "]: " << value << std::endl;
//                     vtkLut->SetTableValue( i , value , value , value , 1.0 );
//                 }
//                 std::cout << std::endl << std::endl << "************ ACABUT **************" << std::endl << std::endl;
            }
            else
                DEBUG_LOG( QString("Error message:: ") + status.text() );
        }
        else
            DEBUG_LOG( QString("Error message:: ") + status.text() );
    }
    return vtkLut;
}

QColor Q2DViewerPresentationStateAttacher::getRecommendedColor( int layer )
{
    // valor de color rgb amb el que pintarem l'anotació, per defecte blanc
    QColor color = Qt::white;
    if( m_presentationStateHandler->haveGraphicLayerRecommendedDisplayValue( layer ) )
    {
        Uint16 r, g, b;
        if( EC_Normal == m_presentationStateHandler->getGraphicLayerRecommendedDisplayValueGray( layer, g ) )
        {
            DEBUG_LOG( QString("Valor de gris recomenat pel display: %1 al layer %2").arg(g).arg(layer) );
            // normalitzem el valor
            color.setRedF( g/65535.0 );
            color.setBlueF( g/65535.0 );
            color.setGreenF( g/65535.0 );
        }
        else
            DEBUG_LOG( QString("No hi ha valor de gris recomenat pel display al layer %1").arg(layer) );

        if( EC_Normal == m_presentationStateHandler->getGraphicLayerRecommendedDisplayValueRGB( layer, r, g, b ) )
        {
            DEBUG_LOG( QString("Valor de color RGB recomenat pel display: %1,%2,%3").arg(r).arg(g).arg(b) );
            color.setRedF( r/65535.0 );
            color.setBlueF( g/65535.0 );
            color.setGreenF( b/65535.0 );
        }
        else
            DEBUG_LOG( QString("No hi ha valor de color rgb recomenat pel display al layer %1").arg(layer) );
    }
    else
        DEBUG_LOG( QString("No hi ha valor recomenat pel display al layer %1").arg(layer) );
    return color;
}

void Q2DViewerPresentationStateAttacher::applyPreSpatialTransformAnnotation()
{
    // aquests canvis es poden aplicar a un subconjunt de imatges/frames. És a dir, una anotació pot estar present en una sola imatge o en més d'una.
    // aquí només apliquem les anotacions en L'ESPAI DE PIXEL
    const char *c;
    size_t max;

    double origin[3],spacing[3];
    m_viewerInput->updateInformation();
    m_viewerInput->getOrigin( origin );
    m_viewerInput->getSpacing( spacing );

    DEBUG_LOG("**** Aplicant les anotacions gràfiques ABANS DE LES TRANSFORMACIONS ESPACIALS (pixel space)****");

    // mirem totes les annotacions que hi hagi per cadascuna de les imatges
    int images = m_attachedDatasetsList.size();
    // també controlem amb  una variable el # de llesca per la següent raó: Com que podem tenir imatges multiframe i no ens consta que totes les imatges multi-frame han de tenir forçosament el mateix nombre de frames no podem calcular la llesca on s'apliquen les annotacions per la senzilla norma i*numFramesImg + j per tant és més segur de moment portar un comptador que s'incrementa a cada iteració.
    int sliceNumber = 0;
    for( int i = 0; i < images; i++ )
    {
        DEBUG_LOG( QString("Attaching image #%1").arg( i ) );
        m_presentationStateHandler->attachImage( m_attachedDatasetsList[i], false );
        // ara hem de mirar quants frames té la imatge
        unsigned long frames;
        OFCondition status = m_presentationStateHandler->getImageNumberOfFrames(frames);
        if( status.bad() )
        {
            DEBUG_LOG( QString("Error a l'hora de llegir el nombre de frames de la imatge adjuntada::[%1]").arg( status.text() ) );
        }
        else
        {
            DEBUG_LOG( QString("Nombre de frames en la imatge adjuntada: %1").arg(frames) );
        }
        for( unsigned long j = 0; j < frames; j++ )
        {
            // seleccionem el frame
            m_presentationStateHandler->selectImageFrameNumber(j+1);
            DEBUG_LOG( QString("Tractant frame #%1").arg(j+1) );
            m_presentationStateHandler->sortGraphicLayers();  // to order of display
            // valor de color rgb amb el que pintarem l'anotació en cada layer
            QColor color;
            for( size_t layer=0; layer < m_presentationStateHandler->getNumberOfGraphicLayers(); layer++ )
            {
                c = m_presentationStateHandler->getGraphicLayerName( layer );
                DEBUG_LOG( QString("Graphic Layer #%1 [%2]").arg( layer+1 ).arg( c ) );

                c = m_presentationStateHandler->getGraphicLayerDescription(layer);
                DEBUG_LOG( QString("Descripció: %1").arg(c) );

                color = this->getRecommendedColor( layer );
                // graphic objects
                max = m_presentationStateHandler->getNumberOfGraphicObjects(layer);
                DEBUG_LOG( QString("Nombre d'objectes gràfics: %1").arg( max ) );
                DVPSGraphicObject *pgraphic = NULL;
                for( size_t graphicidx=0; graphicidx < max; graphicidx++ )
                {
                    pgraphic = m_presentationStateHandler->getGraphicObject(layer, graphicidx);
                    if( pgraphic && pgraphic->getAnnotationUnits() == DVPSA_pixels )
                    {
                        // display contents of graphic object
                        QString message = QString("Graphic %1: #points: %2").arg( graphicidx+1 ).arg( pgraphic->getNumberOfPoints() );
                        j = pgraphic->getNumberOfPoints();
                        Float32 fx = 0.0, fy = 0.0;
                        // els punts del polígon en format vtk
                        vtkPoints *points = vtkPoints::New();
                        double point[2];
                        for( unsigned long k = 0; k < j; k++ )
                        {
                            if( EC_Normal == pgraphic->getPoint( k,fx,fy ) )
                            {
                                point[0] = origin[0] + (fx - 1) * spacing[0];
                                point[1] = origin[1] + (fy - 1) * spacing[1];
                                message += QString(" %1,%2 ").arg( point[0] ).arg( point[1] );
                                // potser en compte de 0.0, hauria de ser la coordenada de la Z de la imatge on correspon?
                                points->InsertPoint( k , point[0], point[1], 0.0
);
                            }
                            else
                                message += "???\\???, ";
                        }
                        if( pgraphic->isFilled() )
                            message += " Filled";

                        switch( pgraphic->getGraphicType() )
                        {
                        case DVPST_polyline:
                            message += " type: POLYLINE";
                            m_board->addPolyline( points, sliceNumber,  Q2DViewer::Axial, false, pgraphic->isFilled(), color );
                        break;

                        case DVPST_interpolated:
                        {
                            message += " type: INTERPOLATED";
                            m_board->addPolyline( points, sliceNumber, Q2DViewer::Axial, true, pgraphic->isFilled(), color );
                        }
                        break;

                        case DVPST_circle:
                        {
                            message += " type: CIRCLE";
                            double radius = sqrt( pow( (points->GetPoint(0)[0]-points->GetPoint(1)[0]) ,2) + pow( (points->GetPoint(0)[1]-points->GetPoint(1)[1]) ,2)  );
                            double center[2] = { points->GetPoint(0)[0], points->GetPoint(0)[1] };
                            m_board->addCircle( center, radius, sliceNumber, Q2DViewer::Axial, pgraphic->isFilled(), color );
                        }
                        break;

                        case DVPST_ellipse:
                        {
                            message += " type: ELLIPSE";
                            double xAxis1[2] = { points->GetPoint(0)[0], points->GetPoint(0)[1] };
                            double xAxis2[2] = { points->GetPoint(1)[0], points->GetPoint(1)[1] };
                            double yAxis1[2] = { points->GetPoint(2)[0], points->GetPoint(2)[1] };
                            double yAxis2[2] = { points->GetPoint(3)[0], points->GetPoint(3)[1] };
                            m_board->addEllipse( xAxis1, xAxis2, yAxis1, yAxis2, sliceNumber, Q2DViewer::Axial, pgraphic->isFilled(), color );
                        }
                        break;

                        case DVPST_point:
                        {
                            message += " type: POINT";
                            double center[2] = { points->GetPoint(0)[0], points->GetPoint(0)[1] };
                            m_board->addPoint( center, sliceNumber, Q2DViewer::Axial, color );
                        }
                        break;
                        }
                        DEBUG_LOG( message );
                    }
                }
            }
            sliceNumber++;
        }
    }
}

void Q2DViewerPresentationStateAttacher::applySpatialTransformation()
{
    // aquests canvis s'apliquen a totes les imatges de la sèrie
    DEBUG_LOG("------------ Aplicant transformacions espacials ------------");
    int rotation;
    switch( m_presentationStateHandler->getRotation() )
    {
    case DVPSR_0_deg:
        DEBUG_LOG("No hi ha Rotació");
        rotation = 0;
    break;

    case DVPSR_90_deg:
        DEBUG_LOG("Rotació de 90º");
        rotation = 1;
    break;

    case DVPSR_180_deg:
        DEBUG_LOG("Rotació de 180º");
        rotation = 2;
    break;

    case DVPSR_270_deg:
        DEBUG_LOG("Rotació de 270º");
        rotation = 3;
    break;
    }
    m_2DViewer->setRotationFactor( rotation );
    if( m_presentationStateHandler->getFlip() )
    {
        DEBUG_LOG("Hi ha flip horitzontal");
        m_2DViewer->horizontalFlip();
    }
    else
    {
        DEBUG_LOG("NO Hi ha flip horitzontal");
    }
}

void Q2DViewerPresentationStateAttacher::applyDisplayedAreaTransformation()
{
    // aquests canvis es poden aplicar a un subconjunt de imatges/frames. Per tant podem tenir diverses display area en una sèrie que s'apliquen a diferents imatges
    Sint32 topLeftX = 0;
    Sint32 topLeftY = 0;
    Sint32 bottomRightX = 0;
    Sint32 bottomRightY = 0;
    DEBUG_LOG("------------ Aplicant Displayed Area Transforms ------------");

    m_presentationStateHandler->getStandardDisplayedArea( topLeftX, topLeftY, bottomRightX, bottomRightY );
    DEBUG_LOG( QString( "Displayed area TLHC[Top Left Hand Corner] %1,%2").arg( topLeftX ).arg( topLeftY ) );
    DEBUG_LOG( QString( "Displayed area BRHC[Bottom Right Hand Corner] %1,%2").arg( bottomRightX ).arg( bottomRightY ) );

    m_viewerInput->updateInformation();
    double spacing[3];
    double origin[3];
    m_viewerInput->getSpacing( spacing );
    m_viewerInput->getOrigin( origin );

    m_displayedAreaWorldTopLeft[0] = origin[0] + topLeftX * spacing[0];
    m_displayedAreaWorldTopLeft[1] = origin[1] + topLeftY * spacing[1];
    m_displayedAreaWorldBottomRight[0] = origin[0] + bottomRightX * spacing[0];
    m_displayedAreaWorldBottomRight[1] = origin[1] + bottomRightY * spacing[1];


    double presentationPixelSpacing[2];
    if( EC_Normal == m_presentationStateHandler->getDisplayedAreaPresentationPixelSpacing( presentationPixelSpacing[0], presentationPixelSpacing[1] ) )
    {
        DEBUG_LOG( QString("Presentation pixel spacing: X=%1mm Y=%2mm")
            .arg( presentationPixelSpacing[0] ).arg( presentationPixelSpacing[1] ) );
    }
    else
    {
        //\TODO també podríem obtenir el ratio ( DCM_PresentationPixelAspectRatio : 0070,0102 ), però no sabem perquè les dcmtk ens diuen que no troben el tag. Segurament és degut a que està en una seqüència
        double ratio = m_presentationStateHandler->getDisplayedAreaPresentationPixelAspectRatio();
        DEBUG_LOG( QString("Ratio obtingut %1").arg( ratio ) );
        m_2DViewer->setPixelAspectRatio( ratio );
    }

    DVPSPresentationSizeMode sizemode = m_presentationStateHandler->getDisplayedAreaPresentationSizeMode();
    double factor = 1.0;

    switch( sizemode )
    {
    case DVPSD_scaleToFit:
        DEBUG_LOG("Presentation size mode: SCALE TO FIT");
        m_2DViewer->scaleToFit( m_displayedAreaWorldTopLeft[0], m_displayedAreaWorldTopLeft[1], m_displayedAreaWorldBottomRight[0], m_displayedAreaWorldBottomRight[1]);
    break;

    case DVPSD_trueSize:
    {
        m_2DViewer->setTrueSizeMode();
    }
    break;

    case DVPSD_magnify:
    {
        m_presentationStateHandler->getDisplayedAreaPresentationPixelMagnificationRatio( factor );
        DEBUG_LOG( QString("Presentation size mode: MAGNIFY factor=%1").arg(factor) );
        m_2DViewer->setMagnificationFactor( factor );
    }
    break;

    default:
        DEBUG_LOG( QString("Size mode no retorna cap valor dels 3 esperats: valor retornat: %1").arg( sizemode ) );
    break;
    }
}

void Q2DViewerPresentationStateAttacher::processTextObjects( int layer, int slice )
{
    double origin[3],spacing[3];
    m_viewerInput->updateInformation();
    m_viewerInput->getOrigin( origin );
    m_viewerInput->getSpacing( spacing );

    unsigned int max;
    max = m_presentationStateHandler->getNumberOfTextObjects( layer );
    DEBUG_LOG( QString("Nombre d'objectes de texte: %1").arg( max ) );
    DVPSTextObject *ptext = NULL;

    // per les coordenades relatives a display
    int rotation = m_presentationStateHandler->getRotation();
    int xIndex = ( rotation == DVPSR_0_deg || rotation == DVPSR_180_deg ) ? 0 : 1;
    int yIndex = ( rotation == DVPSR_0_deg || rotation == DVPSR_180_deg ) ? 1 : 0;

    double displayedWidth = m_displayedAreaWorldBottomRight[ xIndex ] - m_displayedAreaWorldTopLeft[ xIndex ];
    double displayedHeight = m_displayedAreaWorldBottomRight[ yIndex ] - m_displayedAreaWorldTopLeft[ yIndex ];

    double topLeft[2] = { m_displayedAreaWorldTopLeft[ xIndex ], m_displayedAreaWorldTopLeft[ yIndex ] };
    for( size_t textidx=0; textidx < max; textidx++ )
    {
        ptext = m_presentationStateHandler->getTextObject( layer, textidx );
        if( ptext )
        {
            QString msg = QString("Text #%1: ['%2']").arg( textidx+1 ).arg( ptext->getText() );
            int orientation = Q2DViewerBlackBoard::NormalTextOrientation;
            double attachPoint[2] = { 0.0, 0.0 };
            bool hasAnchor = false;
            if( ptext->haveAnchorPoint() )
            {
                hasAnchor = true;
                if( ptext->getAnchorPointAnnotationUnits() == DVPSA_pixels )
                {
                    attachPoint[0] = origin[0] + (ptext->getAnchorPoint_x()-1) * spacing[0];
                    attachPoint[1] = origin[1] + (ptext->getAnchorPoint_y()-1) * spacing[1];
                }
                else
                {
                    attachPoint[ xIndex ] = topLeft[ 0 ] + displayedWidth * ptext->getAnchorPoint_x();
                    attachPoint[ yIndex ] = topLeft[ 1 ] + displayedHeight * ptext->getAnchorPoint_y();
                }
                msg += QString(" %1,%2").arg( ptext->getAnchorPoint_x() ).arg( ptext->getAnchorPoint_y() );
                msg += QString(" Anchor Point (final World coords): %1,%2").arg( attachPoint[0] ).arg( attachPoint[1] );
                if( ptext->anchorPointIsVisible() )
                    msg += " anchor point is visible";
                else
                    msg += " anchor point is NOT visible";
            }
            else
                msg += " doesn't have anchor point";

            DEBUG_LOG( msg );

            double position[2] = { 0.0, 0.0 }, position2[2] = { 0.0, 0.0 };
            if( ptext->haveBoundingBox() )
            {
                DEBUG_LOG( QString("Bounding Box de l'annotació: TLHC: %1,%2, BRHC: %3,%4")
                    .arg( ptext->getBoundingBoxTLHC_x() )
                    .arg( ptext->getBoundingBoxTLHC_y() )
                    .arg( ptext->getBoundingBoxBRHC_x() )
                    .arg( ptext->getBoundingBoxBRHC_y() )
                        );
                double referencePoint[2], referencePoint2[2];
                if( ptext->getBoundingBoxBRHC_x() >= ptext->getBoundingBoxTLHC_x() )
                {
                    if( ptext->getBoundingBoxBRHC_y() >= ptext->getBoundingBoxTLHC_y() ) // sentit d'esquerra a dreta
                    {
                        referencePoint[0] = ptext->getBoundingBoxTLHC_x();
                        referencePoint[1] = ptext->getBoundingBoxBRHC_y();
                        referencePoint2[0] = ptext->getBoundingBoxBRHC_x();
                        referencePoint2[1] = ptext->getBoundingBoxTLHC_y();
                        orientation = Q2DViewerBlackBoard::NormalTextOrientation;
                    }
                    else // sentit d'abaix cap amunt
                    {
                        referencePoint[0] = ptext->getBoundingBoxTLHC_x();
                        referencePoint[1] = ptext->getBoundingBoxTLHC_y();
                        referencePoint2[0] = ptext->getBoundingBoxBRHC_x();
                        referencePoint2[1] = ptext->getBoundingBoxBRHC_y();
                        orientation = Q2DViewerBlackBoard::LeftRotatedTextOrientation;
                    }
                }
                else // sentit de dreta esquerra cap per avall
                {
                    if( ptext->getBoundingBoxBRHC_y() < ptext->getBoundingBoxTLHC_y() ) // && position2[0] < position1[0]
                    {
                        referencePoint[0] = ptext->getBoundingBoxBRHC_x();
                        referencePoint[1] = ptext->getBoundingBoxTLHC_y();
                        referencePoint2[0] = ptext->getBoundingBoxTLHC_x();
                        referencePoint2[1] = ptext->getBoundingBoxBRHC_y();
                        orientation = Q2DViewerBlackBoard::UpsideDownTextOrientation;

                    }
                    else // sentit de dalt a baix
                    {
                        referencePoint[0] = ptext->getBoundingBoxBRHC_x();
                        referencePoint[1] = ptext->getBoundingBoxBRHC_y();
                        referencePoint2[0] = ptext->getBoundingBoxTLHC_x();
                        referencePoint2[1] = ptext->getBoundingBoxTLHC_y();
                        orientation = Q2DViewerBlackBoard::RightRotatedTextOrientation;
                    }
                }
                // Atenció: les posicions en l'actor no són TLHC i BRHC, Position = lower-left Position2 = upper-right, per això posem les coordenades de la següent manera
                //\TODO cal comprovar que els reference point són correctes!
                if( ptext->getBoundingBoxAnnotationUnits() == DVPSA_pixels )
                {
                    DEBUG_LOG("Coordenades Bounding box en espai de Pixel");

                    position[0] = origin[0] + (referencePoint[0]-1) * spacing[0];
                    position[1] = origin[1] + (referencePoint[1]-1) * spacing[1];

                    position2[0] = origin[0] + (referencePoint2[0]-1) * spacing[0];
                    position2[1] = origin[1] + (referencePoint2[1]-1) * spacing[1];
                }
                else
                {
                    DEBUG_LOG("Coordenades Bounding box relatives a la Displayed Area");

                    position[ xIndex ] = topLeft[ 0 ] + displayedWidth * referencePoint[0];
                    position[ yIndex ] = topLeft[ 1 ] + displayedHeight * referencePoint[1];

                    position2[ xIndex ] = topLeft[ 0 ] + displayedWidth * referencePoint2[0];
                    position2[ yIndex ] = topLeft[ 1 ] + displayedHeight * referencePoint2[1];
                }
            }
            else
            {
                DEBUG_LOG("L'anotació de texte no té Bounding Box");
                // El dicom no diu res de com s'ha de renderitzar el texte quan nomé hi ha l'anchor point, així que ho deixarem tal qual i ens quedem tant amples\TODO es podria fer de diverses formes però ara no ens matarem pas, veure apartat C.10.5.1.1 del Sup 33 del DICOM
                // el situarem al mig de la que serà la displayed area
            }

            DVPSTextJustification justification = ptext->getBoundingBoxHorizontalJustification();
            // l'enumeració de les dcmtk respecte a les de vtk en quant a la justificació són diferents, cal controlar això. En Dcmtk left = 0, right = 1, center = 2, a vtk VTK_TEXT_LEFT = 0, VTK_TEXT_CENTERED = 1, VTK_TEXT_RIGHT = 2
            int textJustification;
            switch( justification )
            {
            case DVPSX_left:
                DEBUG_LOG( "Format: Justificat esquerra" );
                textJustification = 0;
            break;

            case DVPSX_right:
                DEBUG_LOG( "Format: Justificat dreta" );
                textJustification = 2;
            break;

            case DVPSX_center:
                DEBUG_LOG( "Format: Justificat centrat" );
                textJustification = 1;
            break;
            }

            m_board->addTextAnnotation( ptext->getText(), slice, Q2DViewer::Axial, ptext->haveBoundingBox(), ptext->haveAnchorPoint(), attachPoint, position, position2, orientation, textJustification, this->getRecommendedColor( layer ), ptext->haveBoundingBox(), ptext->anchorPointIsVisible() );
        }
    }
}

void Q2DViewerPresentationStateAttacher::applyPostSpatialTransformAnnotation()
{
    // aquests canvis es poden aplicar a un subconjunt de imatges/frames. És a dir, una anotació pot estar present en una sola imatge o en més d'una.
    // aquí només apliquem les anotacions relatives a la displayed area
    const char *c;
    size_t max;

    int rotation = m_presentationStateHandler->getRotation();
    int xIndex = ( rotation == DVPSR_0_deg || rotation == DVPSR_180_deg ) ? 0 : 1;
    int yIndex = ( rotation == DVPSR_0_deg || rotation == DVPSR_180_deg ) ? 1 : 0;

    double displayedWidth = m_displayedAreaWorldBottomRight[ xIndex ] - m_displayedAreaWorldTopLeft[ xIndex ];
    double displayedHeight = m_displayedAreaWorldBottomRight[ yIndex ] - m_displayedAreaWorldTopLeft[ yIndex ];

    double topLeft[2] = { m_displayedAreaWorldTopLeft[ xIndex ], m_displayedAreaWorldTopLeft[ yIndex ] };

    DEBUG_LOG("**** Aplicant les anotacions gràfiques DESPRÉS DE LES TRANSFORMACIONS ESPACIALS (displayed area relative space)****");

     // mirem totes les annotacions que hi hagi per cadascuna de les imatges
    int images = m_attachedDatasetsList.size();
    // també controlem amb  una variable el # de llesca per la següent raó: Com que podem tenir imatges multiframe i no ens consta que totes les imatges multi-frame han de tenir forçosament el mateix nombre de frames no podem calcular la llesca on s'apliquen les annotacions per la senzilla norma i*numFramesImg + j per tant és més segur de moment portar un comptador que s'incrementa a cada iteració.
    int sliceNumber = 0;
    for( int i = 0; i < images; i++ )
    {
        DEBUG_LOG( QString("Attaching image #%1").arg( i ) );
        m_presentationStateHandler->attachImage( m_attachedDatasetsList[i], false );
        // ara hem de mirar quants frames té la imatge
        unsigned long frames;
        OFCondition status = m_presentationStateHandler->getImageNumberOfFrames(frames);
        if( status.bad() )
        {
            DEBUG_LOG( QString("Error a l'hora de llegir el nombre de frames de la imatge adjuntada::[%1]").arg( status.text() ) );
        }
        else
        {
            DEBUG_LOG( QString("Nombre de frames en la imatge adjuntada: %1").arg(frames) );
        }
        for( unsigned long j = 0; j < frames; j++ )
        {
            m_presentationStateHandler->selectImageFrameNumber(j+1);
            DEBUG_LOG( QString("Tractant frame #%1").arg(j+1) );
            m_presentationStateHandler->sortGraphicLayers();  // to order of display
            // valor de color rgb amb el que pintarem l'anotació a cada layer
            QColor color;
            for( size_t layer=0; layer < m_presentationStateHandler->getNumberOfGraphicLayers(); layer++ )
            {
                c = m_presentationStateHandler->getGraphicLayerName( layer );
                DEBUG_LOG( QString("Graphic Layer #%1 [%2]").arg( layer+1 ).arg( c ) );

                c = m_presentationStateHandler->getGraphicLayerDescription(layer);
                DEBUG_LOG( QString("Descripció: %1").arg(c) );

                color = this->getRecommendedColor(layer);
                // text objects
                this->processTextObjects( layer, sliceNumber );
                // graphic objects
                max = m_presentationStateHandler->getNumberOfGraphicObjects(layer);
                DEBUG_LOG( QString("Nombre d'objectes gràfics: %1").arg( max ) );
                DVPSGraphicObject *pgraphic = NULL;
                for( size_t graphicidx=0; graphicidx < max; graphicidx++ )
                {
                    pgraphic = m_presentationStateHandler->getGraphicObject(layer, graphicidx);
                    if( pgraphic && pgraphic->getAnnotationUnits() == DVPSA_display )
                    {
                        // display contents of graphic object
                        QString message = QString("Graphic %1: #points: %2").arg( graphicidx+1 ).arg( pgraphic->getNumberOfPoints() );
                        j = pgraphic->getNumberOfPoints();
                        Float32 fx = 0.0, fy = 0.0;
                        // els punts del polígon en format vtk
                        vtkPoints *points = vtkPoints::New();
                        // si les coordenades són relatives haurem de transformar els punts
                        double point[2];
                        for( unsigned long k = 0; k < j; i++ )
                        {
                            if( EC_Normal == pgraphic->getPoint( k,fx,fy ) )
                            {
                                message += QString("\nin display relative space: %1,%2 ").arg( fx ).arg( fy );
                                // ara transformem els punts relatius
                                point[ xIndex ] = topLeft[ 0 ] + displayedWidth * fx;
                                point[ yIndex ] = topLeft[ 1 ] + displayedHeight * fy;
                                message += QString(" in world absolute space: %1,%2 ").arg( point[0] ).arg( point[1] );
                                // potser en compte de 0.0, hauria de ser la coordenada de la Z de la imatge on correspon?
                                points->InsertPoint( k, point[0], point[1],
0.0);
                            }
                            else
                                message += "???\\???, ";
                        }
                        if( pgraphic->isFilled() )
                            message += " Filled";

                        switch( pgraphic->getGraphicType() )
                        {
                        case DVPST_polyline:
                            message += " type: POLYLINE";
                            m_board->addPolyline( points, sliceNumber, Q2DViewer::Axial, false, pgraphic->isFilled(), color );
                        break;

                        case DVPST_interpolated:
                        {
                            message += " type: INTERPOLATED";
                            m_board->addPolyline( points, sliceNumber, Q2DViewer::Axial, true, pgraphic->isFilled(), color );
                        }
                        break;

                        case DVPST_circle:
                        {
                            message += " type: CIRCLE";
                            double radius = sqrt( pow( (points->GetPoint(0)[0]-points->GetPoint(1)[0]) ,2) + pow( (points->GetPoint(0)[1]-points->GetPoint(1)[1]) ,2)  );
                            double center[2] = { points->GetPoint(0)[0], points->GetPoint(0)[1] };
                            m_board->addCircle( center, radius, sliceNumber, Q2DViewer::Axial, pgraphic->isFilled(), color );
                        }
                        break;

                        case DVPST_ellipse:
                        {
                            message += " type: ELLIPSE";
                            double xAxis1[2] = { points->GetPoint(0)[0], points->GetPoint(0)[1] };
                            double xAxis2[2] = { points->GetPoint(1)[0], points->GetPoint(1)[1] };
                            double yAxis1[2] = { points->GetPoint(2)[0], points->GetPoint(2)[1] };
                            double yAxis2[2] = { points->GetPoint(3)[0], points->GetPoint(3)[1] };
                            m_board->addEllipse( xAxis1, xAxis2, yAxis1, yAxis2, sliceNumber, Q2DViewer::Axial, pgraphic->isFilled(), color );
                        }
                        break;

                        case DVPST_point:
                        {
                            message += " type: POINT";
                            double center[2] = { points->GetPoint(0)[0], points->GetPoint(0)[1] };
                            m_board->addPoint( center, sliceNumber, Q2DViewer::Axial, color );
                        }
                        break;
                        }
                        DEBUG_LOG( message );
                    }
                }
            }
            sliceNumber++;
        }
    }
}

void Q2DViewerPresentationStateAttacher::applyOverlayPlanes()
{
    //\TODO ara nomes agafa els de la imatge (if attached), cal agafar també els del presentation state
    // aquests overlays són els que hi ha a la imatge adjunta, es pot fer el mateix però referent al presentation state
    // TODO aquests overlays encara no s'apliquen, només es llegeixen
    size_t maxImg;
    const char *c;
    DEBUG_LOG("* * *______________PARSING OVERLAY DATA______________* * *");
    // mirem totes les annotacions que hi hagi per cadascuna de les imatges
    int images = m_attachedDatasetsList.size();
    for( int i = 0; i < images; i++ )
    {
        DEBUG_LOG( QString("Attaching image #%1").arg( i ) );
        m_presentationStateHandler->attachImage( m_attachedDatasetsList[i], false );
        // ara hem de mirar quants frames té la imatge
        unsigned long frames;
        OFCondition status = m_presentationStateHandler->getImageNumberOfFrames(frames);
        if( status.bad() )
        {
            DEBUG_LOG( QString("Error a l'hora de llegir el nombre de frames de la imatge adjuntada::[%1]").arg( status.text() ) );
        }
        else
        {
            DEBUG_LOG( QString("Nombre de frames en la imatge adjuntada: %1").arg(frames) );
        }
        for( unsigned long j = 0; j < frames; j++ )
        {
            // seleccionem el frame
            m_presentationStateHandler->selectImageFrameNumber(j+1);
            DEBUG_LOG( QString("Tractant frame #%1").arg(j+1) );
            maxImg = m_presentationStateHandler->getNumberOfOverlaysInImage();
            DEBUG_LOG( QString("Overlays available (non-shadowed) in attached image: %1").arg( maxImg ) );
            for( size_t oidx = 0; oidx < maxImg; oidx++ )
            {
                std::cout << "  Overlay #" << oidx+1 << ": group=0x" << hex << m_presentationStateHandler->getOverlayInImageGroup( oidx ) << dec << " label=\"";
                c = m_presentationStateHandler->getOverlayInImageLabel( oidx );
                if( c )
                    std::cout << c;
                else
                    std::cout << "(none)";
                std::cout << "\" description=\"";
                c = m_presentationStateHandler->getOverlayInImageDescription( oidx );
                if( c )
                    std::cout << c;
                else
                    std::cout << "(none)";
                std::cout << "\" type=";
                if( m_presentationStateHandler->overlayInImageIsROI( oidx ) )
                    std::cout << "ROI";
                else
                    std::cout << "graphic";
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
    // ara fem el mateix per als overlays del presentation state
    size_t maxPS;
    maxPS = m_presentationStateHandler->getNumberOfOverlaysInPresentationState();
    std::cout << "Overlays available (non-shadowed) in presentation state: " << maxPS << std::endl;
    for( size_t oidx = 0; oidx < maxPS; oidx++ )
    {
        std::cout << "  Overlay #" << oidx+1 << ": group=0x" << hex << m_presentationStateHandler->getOverlayInPresentationStateGroup( oidx ) << dec << " label=\"";
        c = m_presentationStateHandler->getOverlayInPresentationStateLabel( oidx );
        if( c )
            std::cout << c;
        else
            std::cout << "(none)";
        std::cout << "\" description=\"";
        c = m_presentationStateHandler->getOverlayInPresentationStateDescription( oidx );
        if( c )
            std::cout << c;
        else
            std::cout << "(none)";
        std::cout << "\" type=";
        if( m_presentationStateHandler->overlayInPresentationStateIsROI( oidx ) )
            std::cout << "ROI";
        else
            std::cout << "graphic";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Q2DViewerPresentationStateAttacher::renderCurveObjects()
{
    const char *c;
    size_t max, i ,j;

    DEBUG_LOG( "___________Curve Objects___________" );
    m_presentationStateHandler->sortGraphicLayers();  // to order of display
    for( size_t layer=0; layer < m_presentationStateHandler->getNumberOfGraphicLayers(); layer++ )
    {
        c = m_presentationStateHandler->getGraphicLayerName( layer );
        DEBUG_LOG( QString("Graphic Layer #%1 [%2]").arg( layer+1 ).arg( c ) );

        c = m_presentationStateHandler->getGraphicLayerDescription(layer);
        DEBUG_LOG( QString("Descripció: %1").arg(c) );

        // curve objects
        max = m_presentationStateHandler->getNumberOfCurves( layer );
        std::cout << "  Number of activated curves: " << max << std::endl;
        DVPSCurve *pcurve = NULL;
        for( size_t curveidx=0; curveidx<max; curveidx++ )
        {
            pcurve = m_presentationStateHandler->getCurve( layer, curveidx );
            if( pcurve )
            {
                // display contents of curve
                std::cout << "      curve " << curveidx+1 << ": points=" << pcurve->getNumberOfPoints()
                    << " type=";
                switch( pcurve->getTypeOfData() )
                {
                case DVPSL_roiCurve:
                    std::cout << "roi units=";
                break;

                case DVPSL_polylineCurve:
                    std::cout << "poly units=";
                break;
                }

                c = pcurve->getCurveAxisUnitsX();
                if( c && (strlen(c) > 0) )
                    std::cout << c << "\\";
                else std::cout << "(none)\\";

                c = pcurve->getCurveAxisUnitsY();
                if( c && (strlen(c) > 0) )
                    std::cout << c << std::endl;
                else
                    std::cout << "(none)" << std::endl;
                std::cout << "        label=";

                c = pcurve->getCurveLabel();
                if( c && (strlen(c) > 0) )
                    std::cout << c << " description=";
                else
                    std::cout << "(none) description=";

                c = pcurve->getCurveDescription();
                if( c && (strlen(c) > 0) )
                    std::cout << c << std::endl;
                else
                    std::cout << "(none)" << std::endl;
                std::cout << "        coordinates: ";
                j = pcurve->getNumberOfPoints();
                double dx = 0.0, dy = 0.0;
                for( i=0; i<j; i++ )
                {
                    if( EC_Normal==pcurve->getPoint(i,dx,dy) )
                        std::cout << dx << "\\" << dy << ", ";
                    else
                        std::cout << "???\\???, ";
                }
                std::cout << std::endl;
            }
            else
                std::cout << "      curve " << curveidx+1 << " not present in image." << std::endl;
        }
    }
}

}
