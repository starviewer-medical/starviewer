/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocolmanager.h"

#include "viewerslayout.h"
#include "patient.h"
#include "series.h"
#include "image.h"
#include "volume.h"
#include "q2dviewerwidget.h"
#include "hangingprotocolsrepository.h"
#include "hangingprotocol.h"
#include "hangingprotocollayout.h"
#include "hangingprotocolmask.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "dicomtagreader.h"
#include "../inputoutput/localdatabasemanager.h"
#include "../inputoutput/dicommask.h"
#include "dicomdictionary.h"
#include "identifier.h"
#include "logging.h"
#include "../inputoutput/previousstudiesmanager.h"
#include "volumerepository.h"

#include <QList>
#include <QDate>
#include <QMovie>
#include <QFlags>

namespace udg {

HangingProtocolManager::HangingProtocolManager(QObject *parent)
 : QObject( parent )
{
    // Mappeig de les operacions per la vista sagital
    m_operationsMap.insert("A\\F-A\\H", "0,1"); // per passar de AF -> AH calen 0 rotacions i 1 flip vertical
    m_operationsMap.insert("A\\F-P\\F", "2,1"); m_operationsMap.insert("A\\F-P\\H", "2,0"); m_operationsMap.insert("A\\F-H\\A", "1,0");
    m_operationsMap.insert("A\\F-F\\A", "3,1"); m_operationsMap.insert("A\\F-F\\P", "3,0"); m_operationsMap.insert("A\\F-H\\P", "1,1");

    m_operationsMap.insert("A\\H-A\\F", "0,1"); m_operationsMap.insert("A\\H-P\\F", "2,0"); m_operationsMap.insert("A\\H-P\\H", "2,1");
    m_operationsMap.insert("A\\H-F\\A", "1,0"); m_operationsMap.insert("A\\H-H\\A", "3,1"); m_operationsMap.insert("A\\H-F\\P", "1,1");
    m_operationsMap.insert("A\\H-H\\P", "3,0");

    m_operationsMap.insert("P\\F-A\\F", "2,1"); m_operationsMap.insert("P\\F-A\\H", "2,0"); m_operationsMap.insert("P\\F-P\\H", "0,1");
    m_operationsMap.insert("P\\F-F\\A", "3,0"); m_operationsMap.insert("P\\F-H\\A", "1,1"); m_operationsMap.insert("P\\F-F\\P", "3,1");
    m_operationsMap.insert("P\\F-H\\P", "1,0");

    m_operationsMap.insert("P\\H-A\\F", "2,0"); m_operationsMap.insert("P\\H-A\\H", "2,1"); m_operationsMap.insert("P\\H-P\\F", "0,1");
    m_operationsMap.insert("P\\H-F\\A", "1,1"); m_operationsMap.insert("P\\H-H\\A", "3,0"); m_operationsMap.insert("P\\H-F\\P", "1,0");
    m_operationsMap.insert("P\\H-H\\P", "3,1");

    m_operationsMap.insert("F\\A-A\\F", "3,1"); m_operationsMap.insert("F\\A-A\\H", "3,0"); m_operationsMap.insert("F\\A-P\\F", "1,0");
    m_operationsMap.insert("F\\A-P\\H", "1,1"); m_operationsMap.insert("F\\A-H\\A", "2,1"); m_operationsMap.insert("F\\A-F\\P", "0,1");
    m_operationsMap.insert("F\\A-H\\P", "2,0");

    m_operationsMap.insert("H\\A-A\\F", "3,0"); m_operationsMap.insert("H\\A-A\\H", "3,1"); m_operationsMap.insert("H\\A-P\\F", "1,1");
    m_operationsMap.insert("H\\A-P\\H", "1,0"); m_operationsMap.insert("H\\A-F\\A", "2,1"); m_operationsMap.insert("H\\A-F\\P", "2,0");
    m_operationsMap.insert("H\\A-H\\P", "0,1");

    m_operationsMap.insert("F\\P-A\\F", "1,0"); m_operationsMap.insert("F\\P-A\\H", "1,1"); m_operationsMap.insert("F\\P-P\\F", "3,1");
    m_operationsMap.insert("F\\P-P\\H", "3,0"); m_operationsMap.insert("F\\P-F\\A", "0,1"); m_operationsMap.insert("F\\P-H\\A", "2,0");
    m_operationsMap.insert("F\\P-H\\P", "2,1");

    m_operationsMap.insert("S\\P-A\\F", "3,1"); m_operationsMap.insert("S\\P-A\\H", "1,0"); m_operationsMap.insert("S\\P-P\\F", "3,0");
    m_operationsMap.insert("S\\P-P\\S", "1,1"); m_operationsMap.insert("S\\P-F\\A", "2,0"); m_operationsMap.insert("S\\P-H\\A", "0,1");
    m_operationsMap.insert("S\\P-F\\P", "2,1");

    // Mappeig de les operacions per la vista axial
    m_operationsMap.insert("A\\R-A\\L", "0,1");	m_operationsMap.insert("A\\R-P\\R", "2,1");	m_operationsMap.insert("A\\R-P\\L", "2,0");
    m_operationsMap.insert("A\\R-R\\A", "3,1");	m_operationsMap.insert("A\\R-L\\A", "1,0");	m_operationsMap.insert("A\\R-R\\P", "3,0");
    m_operationsMap.insert("A\\R-L\\P", "1,1");

    m_operationsMap.insert("A\\L-A\\R", "0,1");	m_operationsMap.insert("A\\L-P\\R", "2,0");	m_operationsMap.insert("A\\L-P\\L", "2,1");
    m_operationsMap.insert("A\\L-R\\A", "1,0");	m_operationsMap.insert("A\\L-L\\A", "3,1");	m_operationsMap.insert("A\\L-R\\P", "1,1");
    m_operationsMap.insert("A\\L-L\\P", "3,0");

    m_operationsMap.insert("P\\R-A\\R", "2,1");	m_operationsMap.insert("P\\R-A\\L", "2,0");	m_operationsMap.insert("P\\R-P\\L", "0,1");
    m_operationsMap.insert("P\\R-R\\A", "3,0");	m_operationsMap.insert("P\\R-L\\A", "1,1");	m_operationsMap.insert("P\\R-R\\P", "3,1");
    m_operationsMap.insert("P\\R-L\\P", "1,0");

    m_operationsMap.insert("P\\L-A\\R", "2,0");	m_operationsMap.insert("P\\L-A\\L", "2,1");	m_operationsMap.insert("P\\L-P\\R", "0,1");
    m_operationsMap.insert("P\\L-R\\A", "1,1");	m_operationsMap.insert("P\\L-L\\A", "3,0");	m_operationsMap.insert("P\\L-R\\P", "1,0");
    m_operationsMap.insert("P\\L-L\\P", "3,1");

    m_operationsMap.insert("R\\A-A\\R", "3,1");	m_operationsMap.insert("R\\A-A\\L", "3,0");	m_operationsMap.insert("R\\A-P\\R", "1,0");
    m_operationsMap.insert("R\\A-P\\L", "1,1");	m_operationsMap.insert("R\\A-L\\A", "2,1");	m_operationsMap.insert("R\\A-R\\P", "0,1");
    m_operationsMap.insert("R\\A-L\\P", "2,0");

    m_operationsMap.insert("L\\A-A\\R", "3,0");	m_operationsMap.insert("L\\A-A\\L", "3,1");	m_operationsMap.insert("L\\A-P\\R", "1,1");
    m_operationsMap.insert("L\\A-P\\L", "1,0");	m_operationsMap.insert("L\\A-R\\A", "2,1");	m_operationsMap.insert("L\\A-R\\P", "2,0");
    m_operationsMap.insert("L\\A-L\\P", "0,1");

    m_operationsMap.insert("R\\P-A\\R", "1,0");	m_operationsMap.insert("R\\P-A\\L", "1,1");	m_operationsMap.insert("R\\P-P\\R", "3,1");
    m_operationsMap.insert("R\\P-P\\L", "3,0");	m_operationsMap.insert("R\\P-R\\A", "0,1");	m_operationsMap.insert("R\\P-L\\A", "2,0");
    m_operationsMap.insert("R\\P-L\\P", "2,1");

    m_operationsMap.insert("L\\P-A\\R", "1,1");	m_operationsMap.insert("L\\P-A\\L", "1,0");	m_operationsMap.insert("L\\P-P\\R", "3,0");
    m_operationsMap.insert("L\\P-P\\L", "3,1");	m_operationsMap.insert("L\\P-R\\A", "2,0");	m_operationsMap.insert("L\\P-L\\A", "0,1");
    m_operationsMap.insert("L\\P-R\\P", "2,1");

    m_studiesDownloading = new QMultiHash<QString, StructPreviousStudyDownloading*>();
    m_patient = 0;
}

HangingProtocolManager::~HangingProtocolManager()
{
    cancelHangingProtocolDowloading();
    delete m_studiesDownloading;
}

QList<HangingProtocol * > HangingProtocolManager::searchHangingProtocols( ViewersLayout *layout, Patient *patient, bool applyBestHangingProtocol )
{
    Identifier id;
    HangingProtocol *hangingProtocol;
    HangingProtocol *bestHangingProtocol = NULL;
    double adjustmentOfHanging = 0.0; // Inicialment pensem que no existeix cap hanging
    double bestAdjustmentOfHanging = 0.0; // Inicialment pensem que no existeix cap hanging
    int numberOfItems = HangingProtocolsRepository::getRepository()->getNumberOfItems();
    int hangingProtocolNumber;
    int imageSetNumber;
    HangingProtocolImageSet *imageSet;
    Series *serie;

    QList<HangingProtocol * > candidates;
    QList<Series *> seriesList;
    QString hangingProtocolNamesLogList; // Noms per mostrar al log

    QList<Series *> allSeries;
    QList<Study *> allStudies = sortStudiesByDate( patient->getStudies() );

    foreach (Study *study, allStudies )
    {
        foreach( Series *series, study->getViewableSeries() )
        {
            allSeries.push_back(series);
        }
    }

    int numberOfSeriesAssigned;

    // Buscar el hangingProtocol que s'ajusta millor a l'estudi del pacient
    // Aprofitem per assignar ja les series, per millorar el rendiment
    for( hangingProtocolNumber = 0; hangingProtocolNumber < numberOfItems; hangingProtocolNumber++)
    {
        //Inicialitzacions
        id.setValue( hangingProtocolNumber );
        hangingProtocol = HangingProtocolsRepository::getRepository()->getItem( id );
        numberOfSeriesAssigned = 0;
        imageSetNumber = 1;
        serie = 0;
        seriesList.clear();
        seriesList += allSeries;// Copia de les series perquè es van eliminant de la llista al ser assignades

        if( isValid( hangingProtocol, patient) && !hangingProtocol->hasStudiesToDownload() )
        {
            while( imageSetNumber <= hangingProtocol->getNumberOfImageSets() )
            {
                imageSet = hangingProtocol->getImageSet( imageSetNumber );
                serie = searchSerie( seriesList, imageSet, hangingProtocol->getAllDiferent(), hangingProtocol );

                if( serie != 0 )
                {
                    numberOfSeriesAssigned++;
                }
                imageSetNumber++;
            }
            adjustmentOfHanging = ((double)numberOfSeriesAssigned)/hangingProtocol->getNumberOfImageSets();

            if( hangingProtocol->getStrictness() && adjustmentOfHanging != 1.0 )
                adjustmentOfHanging = 0.0;

            if( (adjustmentOfHanging >= bestAdjustmentOfHanging) && (adjustmentOfHanging > 0.0) && (hangingProtocol->gratherThan(bestHangingProtocol) ) )
            {
                bestHangingProtocol = hangingProtocol;
                bestAdjustmentOfHanging = adjustmentOfHanging;
            }
            if( adjustmentOfHanging > 0 )
            {
                candidates << hangingProtocol;
                hangingProtocolNamesLogList.append( QString( "%1, " ).arg( hangingProtocol->getName() ) ); // Afegim el hanging a la llista pel log
            }
        }
    }

    // Aplicar el hanging protocol trobat, si és que se n'ha trobat algun i s'ha escollit la opció d'aplicar-lo
    if( bestHangingProtocol )
    {
        if( applyBestHangingProtocol )
        {
            DEBUG_LOG( QString("Hanging protocol que s'aplica: %1").arg(bestHangingProtocol->getName() ) );
            applyHangingProtocol(bestHangingProtocol,layout, patient);
            INFO_LOG( QString("Hanging protocols carregats: %1").arg( hangingProtocolNamesLogList ) );
            INFO_LOG( QString("Hanging protocol aplicat: %1").arg( bestHangingProtocol->getName() ) );
        }
        else
        {
            DEBUG_LOG("S'han buscat hanging protocols, però no s'aplica cap per defecte");
        }

    }
    else
    {
        INFO_LOG( QString("No s'ha trobat cap hanging protocol") );
    }
    return candidates;
}

void HangingProtocolManager::applyHangingProtocol( int hangingProtocolNumber, ViewersLayout * layout, Patient * patient )
{
    Identifier id;
    id.setValue( hangingProtocolNumber );

    cancelHangingProtocolDowloading(); // Si hi havia algun estudi descarregant, es treu de la llista d'espera

    HangingProtocol *hangingProtocol = HangingProtocolsRepository::getRepository()->getItem( id );
    // TODO aixo no deixa de ser un HACK perquè quedi seleccionat el primer dels widgets
    // Caldria incoporar algun paràmetre per indicar quin és el visor seleccionat per defecte
    // Es buiden tots els visors per tal que no hi hagi res assignat
    layout->setGrid(1,1);
    applyHangingProtocol(hangingProtocol,layout, patient);
    INFO_LOG( QString("Hanging protocol aplicat: %1").arg( hangingProtocol->getName() ) );
}

void HangingProtocolManager::applyHangingProtocol( HangingProtocol *hangingProtocol, ViewersLayout *layout, Patient * patient )
{
    int displaySetNumber;
    HangingProtocolImageSet *hangingProtocolImageSet;
    HangingProtocolDisplaySet *displaySet;
    Series *serie;
    Q2DViewerWidget *viewerWidget;
    PreviousStudiesManager * previousStudiesManager = new PreviousStudiesManager();

    for( displaySetNumber = 0; displaySetNumber < hangingProtocol->getNumberOfDisplaySets(); displaySetNumber++ )
    {
        serie = 0;
        displaySet = hangingProtocol->getDisplaySet( displaySetNumber + 1 );
        hangingProtocolImageSet = hangingProtocol->getImageSet( displaySet->getImageSetNumber() );
        serie = hangingProtocolImageSet->getSeriesToDisplay();
        viewerWidget = layout->addViewer( displaySet->getPosition() );

        if( hangingProtocolImageSet->isDownloaded() == false )
        {
            ///TODO
            QWidget * downloadingWidget = createDownloadingWidget( layout );
            layout->setDownloadingItem( viewerWidget, downloadingWidget );

            StructPreviousStudyDownloading * structPreviousStudyDownloading = new StructPreviousStudyDownloading;
            structPreviousStudyDownloading->widgetToDisplay = viewerWidget;
            structPreviousStudyDownloading->downloadingWidget = downloadingWidget;
            structPreviousStudyDownloading->layout = layout;
            structPreviousStudyDownloading->imageSet = hangingProtocolImageSet;
            structPreviousStudyDownloading->hangingProtocol = hangingProtocol;
            structPreviousStudyDownloading->displaySet = displaySet;

            bool isDownloading = m_studiesDownloading->contains( hangingProtocolImageSet->getPreviousStudyToDisplay()->getInstanceUID() );

            m_studiesDownloading->insert( hangingProtocolImageSet->getPreviousStudyToDisplay()->getInstanceUID(), structPreviousStudyDownloading );            
            m_patient = patient;

            connect( m_patient, SIGNAL( patientFused() ), SLOT(previousStudyDownloaded() ) );
            connect( previousStudiesManager, SIGNAL(errorDownloadingPreviousStudy(QString)), SLOT( errorDowlonadingPreviousStudies(QString) ) );

            if( !isDownloading )
                previousStudiesManager->downloadStudy( hangingProtocolImageSet->getPreviousStudyToDisplay(), hangingProtocolImageSet->getPreviousStudyPacs() );
        }
        else
        {
            if( serie ) // Ens podem trobar que un viewer no tingui serie, llavors no hi posem input
            {
                // cal que la sèrie que escollim sigui vàlida, sinó no posarem pas res
                if( serie->isViewable() && serie->getFirstVolume() )
                {
                    viewerWidget->setInput( serie->getFirstVolume() );
                    qApp->processEvents( QEventLoop::ExcludeUserInputEvents );
                    if( hangingProtocolImageSet->getTypeOfItem() == "image" )
                    {
                        viewerWidget->getViewer()->setSlice( hangingProtocolImageSet->getImageToDisplay() );
                        applyDisplayTransformations( serie, hangingProtocolImageSet->getImageToDisplay(), viewerWidget, displaySet );
                    }
                    else
                    {
                        applyDisplayTransformations( serie, 0, viewerWidget, displaySet );
                    }
                    if( displaySet->getToolActivation() != 0 ) // Tenim tools activades per defecte des del hanging protocol
                    {
                        if( displaySet->getToolActivation() == "synchronization" ) // S'activa la tool de sincronització
                            viewerWidget->setSynchronized( true );
                        else // Es desactiva la tool de sincronització, per si estava activada
                            viewerWidget->setSynchronized( false );
                    }
                    else // es desactiven totes les tools que puguin estar actives
                    {
                        viewerWidget->setSynchronized( false );
                    }
                }
            }
        }
    }
}

bool HangingProtocolManager::isValid( HangingProtocol *protocol, Patient *patient)
{
    bool valid = false;
    bool found = false;
    int i;

    foreach (Study *study, patient->getStudies())
    {
        QList<QString> listOfModalities = study->getModalitiesAsSingleString().split("/");
        found = false;
        i = 0;

        while( !found  && i < listOfModalities.size() )
        {
            if( protocol->getHangingProtocolMask()->getProtocolList().contains( listOfModalities.value(i) ) )
            {
                valid = true;
                found = true;
            }
            i++;
        }
    }

    return valid;
}

Series * HangingProtocolManager::searchSerie( QList<Series*> &listOfSeries, HangingProtocolImageSet *imageSet, bool quitStudy, HangingProtocol * hangingProtocol )
{
    bool found = false;
    int i = 0;
    int imageNumber = 0;
    int numberSeries = listOfSeries.size();
    int numberImages;
    Series *serie = 0;
    Image *image = 0;
    QList< Image * > listOfImages;

    while( !found && i < numberSeries )
    {
        serie = listOfSeries.value( i );

        if( imageSet->getTypeOfItem() != "image" )
        {
            if( isValidSerie( serie, imageSet, hangingProtocol ) )
            {
                found = true;
                imageSet->setSeriesToDisplay( serie );
                if( quitStudy )
                {
                    listOfSeries.removeAt(i);
                }
            }
        }
        else
        {
            // Comprovem que la sèrie sigui de la modalitat del hanging protocol per evitar haver-ho de comprovar a cada imatge
            if( hangingProtocol->getHangingProtocolMask()->getProtocolList().contains( serie->getModality() ) )
            {
                imageNumber = 0;
                listOfImages = serie->getImages();
                numberImages = listOfImages.size();

                while( !found && imageNumber < numberImages )
                {
                    image = listOfImages.value( imageNumber );
                    if( isValidImage( image, imageSet, hangingProtocol ) )
                    {
                        found = true;
                        imageSet->setImageToDisplay( imageNumber );
                        imageSet->setSeriesToDisplay( serie );
                        if( quitStudy )
                            listOfSeries.removeAt(i);
                    }
                    imageNumber++;
                }
            }
            if( !found )
                imageSet->setImageToDisplay( 0 );
        }

        if( found )
            return serie;
        i++;
    }
    imageSet->setSeriesToDisplay( 0 );//Important, no hi posem cap serie!
    return 0;
}

bool HangingProtocolManager::isValidSerie( Series *serie, HangingProtocolImageSet *imageSet, HangingProtocol * hangingProtocol )
{
    bool valid = true;
    int i = 0;
    QList< HangingProtocolImageSet::Restriction > listOfRestrictions = imageSet->getRestrictions();
    int numberRestrictions = listOfRestrictions.size();
    HangingProtocolImageSet::Restriction restriction;
    DICOMTagReader dicomReader;

    valid = (serie->getModality() != "PR"); // Els presentation states per defecte no es mostren

    if( valid )
    {
        valid = hangingProtocol->getHangingProtocolMask()->getProtocolList().contains( serie->getModality() );
    }
    
    while ( valid && i < numberRestrictions )
    {
        restriction = listOfRestrictions.value( i );

        if( restriction.selectorAttribute == "BodyPartExamined" )
        {
            if( serie->getBodyPartExamined() != restriction.valueRepresentation )
                valid = false;
        }
        else if( restriction.selectorAttribute == "ProtocolName" )
        {
            if( ! serie->getProtocolName().contains( restriction.valueRepresentation ) )
                valid = false;
        }
        else if( restriction.selectorAttribute == "ViewPosition" )
        {
            if( serie->getViewPosition() != restriction.valueRepresentation )
                valid = false;
        }
        else if( restriction.selectorAttribute == "SeriesDescription" )
        {
            bool contains = serie->getDescription().contains( restriction.valueRepresentation, Qt::CaseInsensitive );
            bool match = ( restriction.usageFlag  == HangingProtocolImageSet::NoMatch );
            valid = contains ^ match;
        }
        else if( restriction.selectorAttribute == "ScanOptions" )
        {
            Image *image = serie->getImages().value( 0 );
            if( image )
            {
                dicomReader.setFile( image->getPath() );
                if( dicomReader.getAttributeByName( DICOMScanOptions ) != restriction.valueRepresentation )
                    valid = false;
            }
            else
            {
                valid = false;
            }
        }
        else if( restriction.selectorAttribute == "PatientName" )
        {
            if( serie->getParentStudy()->getParentPatient()->getFullName() != restriction.valueRepresentation )
                valid = false;
        }
        else if( restriction.selectorAttribute == "SeriesNumber" )
        {
            if( serie->getSeriesNumber() != restriction.valueRepresentation )
                valid = false;
        }
        else if( restriction.selectorAttribute == "Anterior" )
        {
            int imageSetNumber = restriction.valueRepresentation.toInt();

            /// S'ha de tenir en compte que a la imatge a què es refereix pot estar pendent de descarrega (prèvia)
            Study * referenceStudy = 0;
            HangingProtocolImageSet * referenceImageSet = hangingProtocol->getImageSet( imageSetNumber );

            if( referenceImageSet->isDownloaded() ) // L'estudi de referència està descarregat
                if( referenceImageSet->getSeriesToDisplay() != 0 ) // no te sèrie anterior, per tant no és valid
                    referenceStudy = referenceImageSet->getSeriesToDisplay()->getParentStudy();
            else // L'estudi de referència és un previ que encara no s'ha descarregat
                referenceStudy = referenceImageSet->getPreviousStudyToDisplay();

            if( (referenceStudy == 0) || (serie->getParentStudy()->getDate() >= referenceStudy->getDate()) )
                valid = false;
        }
        else if( restriction.selectorAttribute == "MinimumNumberOfImages" )
        {
            if( serie->getNumberOfImages() < restriction.valueRepresentation.toInt() )
                valid = false;
        }
        i++;
    }

    return valid;
}

void HangingProtocolManager::applyDisplayTransformations( Series *serie, int imageNumber, Q2DViewerWidget *viewer, HangingProtocolDisplaySet *displaySet )
{
    viewer->getViewer()->automaticRefresh( false );
    
    QVector<QString> labels = viewer->getViewer()->getCurrentDisplayedImageOrientationLabels();
    applyDesiredDisplayOrientation( labels[2]+"\\"+labels[3], displaySet->getPatientOrientation(), viewer->getViewer() );
    
    //Posem la imatge al mig
    viewer->getViewer()->setAlignPosition( Q2DViewer::AlignCenter );

    QString reconstruction = displaySet->getReconstruction();
    if( !reconstruction.isEmpty() )
    {
        if( reconstruction == "SAGITAL" )
        {
            viewer->getViewer()->resetViewToSagital();
        }
        else if ( reconstruction == "CORONAL" )
        {
            viewer->getViewer()->resetViewToCoronal();
        }
        else if( reconstruction == "AXIAL" )
        {
            viewer->getViewer()->resetViewToAxial();
        }
        else
        {
            DEBUG_LOG( "Field reconstruction in XML hanging protocol has an error" );
        }
    }

    QString phase = displaySet->getPhase();
    if( !phase.isEmpty() )
    {
        viewer->getViewer()->setPhase( phase.toInt() );
    }

    int sliceNumber = displaySet->getSlice();
    if( sliceNumber != -1 )
    {
        viewer->getViewer()->setSlice( sliceNumber );
    }

    disconnect( viewer, SIGNAL( resized() ), viewer->getViewer(), SLOT( alignRight() ) );
    disconnect( viewer, SIGNAL( resized() ), viewer->getViewer(), SLOT( alignLeft() ) );

    QString alignment = displaySet->getAlignment();
    if( !alignment.isEmpty() )
    {
        if( alignment == "right" )
        {
            viewer->getViewer()->alignRight();
            connect( viewer, SIGNAL( resized() ), viewer->getViewer(), SLOT( alignRight() ) );
        }
        else if (alignment == "left" )
        {
            viewer->getViewer()->alignLeft();
            connect( viewer, SIGNAL( resized() ), viewer->getViewer(), SLOT( alignLeft() ) );
        }
    }
    else
    {
        viewer->getViewer()->setAlignPosition( Q2DViewer::AlignCenter );
    }

    viewer->getViewer()->automaticRefresh( true );
    viewer->getViewer()->refresh();
}

void HangingProtocolManager::applyDesiredDisplayOrientation(const QString &currentOrientation, const QString &desiredOrientation, Q2DViewer *viewer)
{
    if( !currentOrientation.isEmpty() && !desiredOrientation.isEmpty() )
    {
        // TODO al tanto, patient orientation podria tenir més d'una lletra per row!
        // per exemple RA\AL en un tall que sigui oblicu
        // per evitar això i no fer una llista enorme de transformacions,
        // agafarem només la primera lletra del row i de la columna
        QStringList rowColumn = currentOrientation.split("\\");
        QString mapIndex = rowColumn.at(0).left(1) + "\\" + rowColumn.at(1).left(1) + "-" + desiredOrientation;
        QString operations = m_operationsMap.value( mapIndex );

        if( !operations.isEmpty() )
        {
            QStringList listOfOperations = operations.split(",");
            // apliquem les transformacions d'imatge necessàries
            // per visualitzar correctament la imatge
            viewer->rotateClockWise( listOfOperations[0].toInt() ); // apliquem el nombre de rotacions
            if( listOfOperations[1].toInt() )
                viewer->verticalFlip(); // apliquem el flip vertical si cal
        }
    }
}

bool HangingProtocolManager::isValidImage( Image *image, HangingProtocolImageSet *imageSet, HangingProtocol * hangingProtocol )
{
    bool valid = true;
    int i = 0;
    QList< HangingProtocolImageSet::Restriction > listOfRestrictions = imageSet->getRestrictions();
    int numberRestrictions = listOfRestrictions.size();
    HangingProtocolImageSet::Restriction restriction;
    DICOMTagReader dicomReader;

    bool ok = dicomReader.setFile( image->getPath() );
    if( ok )
    {
        while ( valid && i < numberRestrictions )
        {
            restriction = listOfRestrictions.value( i );
            if( restriction.selectorAttribute == "ViewPosition" )
            {
                //if( ! dicomReader.getAttributeByName( DICOMViewPosition ).contains( restriction.valueRepresentation) )
                //    valid = false;

                QString viewPosition = dicomReader.getAttributeByName( DICOMViewPosition );
                bool contains = viewPosition.contains( restriction.valueRepresentation, Qt::CaseInsensitive );
                bool match = ( restriction.usageFlag  == HangingProtocolImageSet::NoMatch );
                valid = contains ^ match;
            }
            else if( restriction.selectorAttribute == "ImageLaterality" )
            {
                if( dicomReader.getAttributeByName( DICOMImageLaterality ) != restriction.valueRepresentation )
                    valid = false;
            }
            else if( restriction.selectorAttribute == "Laterality" )
            {
                if( dicomReader.getAttributeByName( DICOMLaterality ) != restriction.valueRepresentation )
                    valid = false;
            }
            else if( restriction.selectorAttribute == "PatientOrientation" )
            {
                if( !image->getPatientOrientation().contains( restriction.valueRepresentation ) )
                    valid = false;
            }
            else if( restriction.selectorAttribute == "CodeMeaning" )
            {
                QStringList tagValue =  dicomReader.getSequenceAttributeByName( DICOMViewCodeSequence, DICOMCodeMeaning );
                bool match = ( restriction.usageFlag  == HangingProtocolImageSet::Match );

                if( tagValue.isEmpty() || !( tagValue.at(0).contains( restriction.valueRepresentation ) ) )
                    valid = false;

                if( !match ) valid = !valid;// just el cas contrari
            }
            else if( restriction.selectorAttribute == "ImageType" )
            {
                QString imageType = dicomReader.getAttributeByName( DICOMImageType );
                bool isLocalyzer = imageType.contains( restriction.valueRepresentation, Qt::CaseInsensitive );
                bool match = ( restriction.usageFlag  == HangingProtocolImageSet::NoMatch );
                valid = isLocalyzer ^ match;
            }
            else if( restriction.selectorAttribute == "Anterior" )
            {
                Series * serie = image->getParentSeries();
                int imageSetNumber = restriction.valueRepresentation.toInt();

                /// S'ha de tenir en compte que a la imatge a què es refereix pot estar pendent de descarrega (prèvia)
                Study * referenceStudy = 0;
                HangingProtocolImageSet * referenceImageSet = hangingProtocol->getImageSet( imageSetNumber );

                if( referenceImageSet->isDownloaded() ) // L'estudi de referència està descarregat
                    if( referenceImageSet->getSeriesToDisplay() != 0 ) // no te sèrie anterior, per tant no és valid
                        referenceStudy = referenceImageSet->getSeriesToDisplay()->getParentStudy();
                else // L'estudi de referència és un previ que encara no s'ha descarregat
                    referenceStudy = referenceImageSet->getPreviousStudyToDisplay();

                if( (referenceStudy == 0) || (serie->getParentStudy()->getDate() >= referenceStudy->getDate()) )
                    valid = false;
            }
            else if( restriction.selectorAttribute == "MinimumNumberOfImages" )
            {
                Series * serie = image->getParentSeries();
                if( serie->getNumberOfImages() < restriction.valueRepresentation.toInt() )
                    valid = false;
            }
            i++;
        }
    }
    return valid;
}

QList<HangingProtocol * > HangingProtocolManager::getHangingProtocolsWidthPreviousSeries( Patient * patient, QList<Study*> previousStudies, QHash<QString, QString> pacs )
{
    HangingProtocol * hangingProtocol;
    int numberOfItems = HangingProtocolsRepository::getRepository()->getNumberOfItems();
    int hangingProtocolNumber;
    QList<HangingProtocol * > previousCandidates;
    QString hangingProtocolNamesLogList; // Noms per mostrar al log
    Identifier id;
    int imageSetNumber;
    HangingProtocolImageSet *imageSet;
    Study * previousStudy;
    Series *serie;
    int numberOfSeriesAssigned;
    double adjustmentOfHanging = 0.0; // Inicialment pensem que no existeix cap hanging

    if ( previousStudies.size() == 0 )
        return previousCandidates;

    QList<Series *> seriesList;    
    QList<Series *> allSeries;
    QList<Study *> allStudies = sortStudiesByDate( patient->getStudies() );

    foreach (Study *study, allStudies)
    {
        foreach( Series *series, study->getViewableSeries() )
        {
            allSeries.push_back(series);
        }
    }

    for( hangingProtocolNumber = 0; hangingProtocolNumber < numberOfItems; hangingProtocolNumber++)
    {
        //Inicialitzacions
        id.setValue( hangingProtocolNumber );
        hangingProtocol = HangingProtocolsRepository::getRepository()->getItem( id );
        
        if( hangingProtocol->isPrevious() )
        {
            imageSetNumber = 1;
            numberOfSeriesAssigned = 0;
            seriesList.clear();
            seriesList += allSeries;// Copia de les series perquè es van eliminant de la llista al ser assignades

            if( isValid( hangingProtocol, patient ) )
            {
                while( imageSetNumber <= hangingProtocol->getNumberOfImageSets() )
                {
                    imageSet = hangingProtocol->getImageSet( imageSetNumber );
                    serie = 0;
                    previousStudy = 0;

                    //Cerca d'una sèrie de la forma habitual
                    serie = searchSerie( seriesList, imageSet, hangingProtocol->getAllDiferent(), hangingProtocol );
                    
                    if( serie != 0 )//S'ha trobat descarregada i carregada a l'estructura pacient
                    {
                        numberOfSeriesAssigned++; 
                        imageSet->setDownloaded( true );
                    }
                    else //Es busca a la llista de pendent de descàrrega
                    {
                        if( imageSet->isPreviousStudy() ) // Si és de tipus prèvi, se li dóna una segona oportunitat buscant a previs
                        {
                            Study * referenceStudy = 0;
                            HangingProtocolImageSet * referenceImageSet = hangingProtocol->getImageSet( imageSet->getPreviousImageSetReference() );

                            if( referenceImageSet->isDownloaded() && referenceImageSet->getSeriesToDisplay() ) // L'estudi de referència està descarregat
                                referenceStudy = referenceImageSet->getSeriesToDisplay()->getParentStudy();
                            else // L'estudi de referència és un previ que encara no s'ha descarregat
                                referenceStudy = referenceImageSet->getPreviousStudyToDisplay();

                            if( referenceStudy != 0 )
                            {
                                previousStudy = searchPreviousStudy( hangingProtocol, referenceStudy, previousStudies );

                                if( previousStudy != 0 ) //S'ha trobat pendent de descarrega
                                {
                                    numberOfSeriesAssigned++;
                                    imageSet->setDownloaded( false );
                                    imageSet->setPreviousStudyToDisplay( previousStudy );
                                    imageSet->setPreviousStudyPacs( pacs[previousStudy->getInstanceUID()] );
                                }
                            }
                        }
                    }
                    imageSetNumber++;
                }
                adjustmentOfHanging = ((double)numberOfSeriesAssigned)/hangingProtocol->getNumberOfImageSets();

                if( hangingProtocol->getStrictness() && adjustmentOfHanging != 1.0 )
                    adjustmentOfHanging = 0.0;

                if( adjustmentOfHanging > 0 )
                {
                    previousCandidates << hangingProtocol;
                    hangingProtocolNamesLogList.append( QString( "%1, " ).arg( hangingProtocol->getName() ) ); // Afegim el hanging a la llista pel log
                }
            }
        }
    }

    return previousCandidates;
}

Study * HangingProtocolManager::searchPreviousStudy( HangingProtocol * protocol , Study * referenceStudy, QList<Study*> previousStudies)
{
    Study * previousStudy = 0;
    bool found = false;
    int studyNumber = 0;
    Study * study;

    previousStudies = sortStudiesByDate( previousStudies );
    while ( !found && (studyNumber < previousStudies.size()) )
    {
        study = previousStudies.at( studyNumber );

        if( study->getDate() < referenceStudy->getDate() )
        {
            QStringList modalities = study->getModalities();
            int i = 0;

            while( !found  && i < modalities.size() )
            {
                if( protocol->getHangingProtocolMask()->getProtocolList().contains( modalities.value(i) ) )
                {
                    previousStudy = study;
                    found = true;
                }
                i++;
            }
        }
        studyNumber++;
    }

    return previousStudy;
}

void HangingProtocolManager::previousStudyDownloaded()
{
    int i;

    // Es busca quins estudis nous hi ha
    foreach( Study * study, m_patient->getStudies() )
    {
        if( m_studiesDownloading->empty() )
            return;

        QList<StructPreviousStudyDownloading*> values = m_studiesDownloading->values( study->getInstanceUID() );

        for (int i = 0; i < values.size(); i++)
        { // Per cada estudi que esperàvem que es descarregués

            // Agafem l'estructura amb les dades que s'havien guardat per poder aplicar-ho
            StructPreviousStudyDownloading * structPreviousStudyDownloading = values.at(i);
            
            /// Busquem la millor serie de l'estudi que ho satisfa
            QList<Series *> studySeries = study->getSeries();
            Series * series = searchSerie( studySeries, structPreviousStudyDownloading->imageSet, false, structPreviousStudyDownloading->hangingProtocol);
            
            Q2DViewerWidget * viewerWidget = structPreviousStudyDownloading->widgetToDisplay;
            ViewersLayout * layout = structPreviousStudyDownloading->layout;
            structPreviousStudyDownloading->imageSet->setDownloaded( true );

            layout->quitDownloadingItem( viewerWidget, structPreviousStudyDownloading->downloadingWidget );

            // S'assigna la serie al visualitzador si es que n'ha trobat alguna
            if( series != 0)
            {
                // cal que la sèrie que escollim sigui vàlida, sinó no posarem pas res
                if( series->isViewable() && series->getFirstVolume() )
                {
                    Volume *volume = VolumeRepository::getRepository()->getVolume( series->getFirstVolume()->getIdentifier() );
                    viewerWidget->setInput( volume );
                    qApp->processEvents( QEventLoop::ExcludeUserInputEvents );

                    if( structPreviousStudyDownloading->imageSet->getTypeOfItem() == "image" )
                    {
                        viewerWidget->getViewer()->setSlice( structPreviousStudyDownloading->imageSet->getImageToDisplay() );
                        applyDisplayTransformations( series, structPreviousStudyDownloading->imageSet->getImageToDisplay(), viewerWidget, structPreviousStudyDownloading->displaySet );
                    }
                    else
                    {
                        applyDisplayTransformations( series, 0, viewerWidget, structPreviousStudyDownloading->displaySet );
                    }
                    if( structPreviousStudyDownloading->displaySet->getToolActivation() != 0 )
                    {
                        if( structPreviousStudyDownloading->displaySet->getToolActivation() == "synchronization" )
                            viewerWidget->setSynchronized( true );
                    }
                    else //Es desactiven les tools necessàries
                    {
                        viewerWidget->setSynchronized( false );
                    }
                }
            }

            m_studiesDownloading->remove( study->getInstanceUID() );

            if( m_studiesDownloading->empty() )
                structPreviousStudyDownloading->hangingProtocol->setHasStudiesToDownload( false );
        }
    }
}

void HangingProtocolManager::errorDowlonadingPreviousStudies(QString studyUID)
{
    if( m_studiesDownloading->contains( studyUID ) )//si és un element que estavem esperant
    {
        StructPreviousStudyDownloading* element = m_studiesDownloading->take( studyUID ); // s'agafa i es treu de la llista
        element->layout->quitDownloadingItem( element->widgetToDisplay, element->downloadingWidget ); // es treu el label de downloading
    }
}

QList<Study*> HangingProtocolManager::sortStudiesByDate( QList<Study*> studies )
{
    QList<Study*> orderedStudiesByDate;
    int i;
    int newest;

    while( studies.size() > 0 )
    {
        newest = 0;
        i = 0;
        while( i < studies.size() )//Es busca el mesgran
        {
            if( studies.at(i)->getDate() > studies.at( newest )->getDate() )
                newest = i;
            i++;
        }
        orderedStudiesByDate << studies.takeAt( newest );
    }
    return orderedStudiesByDate;
}


QWidget * HangingProtocolManager::createDownloadingWidget( ViewersLayout *layout )
{
    QWidget * downloadingWidget = new QWidget( layout );
    downloadingWidget->setStyleSheet( "background-color: black; color: white;" );
    QVBoxLayout * verticalLayout = new QVBoxLayout( downloadingWidget );

    QFlags<Qt::AlignmentFlag> topFlag(Qt::AlignTop);
    QFlags<Qt::AlignmentFlag> hCenterFlag(Qt::AlignHCenter);
    QFlags<Qt::AlignmentFlag> bottomFlag(Qt::AlignBottom);

    QLabel * downloadingLabelText = new QLabel( downloadingWidget );
    downloadingLabelText->setText( tr("Downloading previous study..." ) );
    downloadingLabelText->setAlignment( bottomFlag|hCenterFlag );
    verticalLayout->addWidget(downloadingLabelText);
    QMovie * downloadingMovie = new QMovie();
    QLabel * downloadingLabelMovie  = new QLabel( downloadingWidget );
    downloadingLabelMovie->setMovie(downloadingMovie);
    downloadingMovie->setFileName(QString::fromUtf8(":/images/downloading.gif"));

    downloadingLabelMovie->setAlignment( topFlag|hCenterFlag );
    verticalLayout->addWidget(downloadingLabelMovie);
    downloadingMovie->start();
    
    return downloadingWidget;
}

void HangingProtocolManager::cancelHangingProtocolDowloading()
{
    foreach( QString key, m_studiesDownloading->keys() )
    {
        StructPreviousStudyDownloading* element = m_studiesDownloading->take( key ); // S'agafa i es treu de la llista l'element que s'està esperant
        element->layout->quitDownloadingItem( element->widgetToDisplay, element->downloadingWidget ); // es treu el label de downloading
        delete element;
    }
}

}
