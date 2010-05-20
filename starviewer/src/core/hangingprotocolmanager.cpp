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
#include "identifier.h"
#include "logging.h"
#include "volumerepository.h"
// Necessari per poder anar a buscar prèvies
#include "../inputoutput/previousstudiesmanager.h"

#include <QMovie>

namespace udg {

HangingProtocolManager::HangingProtocolManager(QObject *parent)
 : QObject( parent )
{
    m_studiesDownloading = new QMultiHash<QString, StructPreviousStudyDownloading*>();
    m_patient = 0;
}

HangingProtocolManager::~HangingProtocolManager()
{
    cancelHangingProtocolDowloading();
    delete m_studiesDownloading;
}

QList<HangingProtocol *> HangingProtocolManager::searchHangingProtocols(Patient *patient)
{
    int numberOfHangingProtocols = HangingProtocolsRepository::getRepository()->getNumberOfItems();

    QList<HangingProtocol * > outputHangingProtocolList;
    QString hangingProtocolNamesLogList; // Noms per mostrar al log

    QList<Series *> allSeries;

    foreach ( Study *study , sortStudiesByDate( patient->getStudies() ) )
    {
        allSeries += study->getViewableSeries();
    }

    int numberOfSeriesAssigned;

    // Buscar el hangingProtocol que s'ajusta millor a l'estudi del pacient
    // Aprofitem per assignar ja les series, per millorar el rendiment
    for( int hangingProtocolNumber = 0; hangingProtocolNumber < numberOfHangingProtocols ; hangingProtocolNumber++)
    {
        //Inicialitzacions
        HangingProtocol *hangingProtocol = HangingProtocolsRepository::getRepository()->getItem( Identifier(hangingProtocolNumber) );

        if( isModalityCompatible(hangingProtocol, patient) && !hangingProtocol->hasStudiesToDownload() )
        {
            QList<Series *> candidateSeries = allSeries; // Copia de les series perquè es van eliminant de la llista al ser assignades
            
            numberOfSeriesAssigned = 0;
            
            foreach ( HangingProtocolImageSet * imageSet, hangingProtocol->getImageSets() )
            {
                if( searchSerie( candidateSeries, imageSet , hangingProtocol->getAllDiferent(), hangingProtocol ) )
                {
                    numberOfSeriesAssigned++;
                }
            }

            bool isValidHangingProtocol = false;

            if( hangingProtocol->isStrict() )
            {
                if ( numberOfSeriesAssigned == hangingProtocol->getNumberOfImageSets() )
                {
                    isValidHangingProtocol = true;
                }
            }
            else
            {
                if ( numberOfSeriesAssigned > 0 )
                {
                    isValidHangingProtocol = true;
                }
            }

            if( isValidHangingProtocol )
            {
                outputHangingProtocolList << hangingProtocol;
                hangingProtocolNamesLogList.append( QString( "%1, " ).arg( hangingProtocol->getName() ) ); // Afegim el hanging a la llista pel log
            }
        }
    }

    if ( hangingProtocolNamesLogList.size() > 0 )
    {
        INFO_LOG( QString("Hanging protocols carregats: %1").arg( hangingProtocolNamesLogList ) );
    }
    else
    {
        INFO_LOG( QString("No s'ha trobat cap hanging protocol") );
    }

    return outputHangingProtocolList;
}

void HangingProtocolManager::setBestHangingProtocol(Patient *patient, const QList<HangingProtocol*> &hangingProtocolList, ViewersLayout *layout)
{
    HangingProtocol *bestHangingProtocol = NULL;
    foreach (HangingProtocol *hangingProtocol, hangingProtocolList)
    {
        if( hangingProtocol->isBetterThan(bestHangingProtocol) )
        {
            bestHangingProtocol = hangingProtocol;
        }
    }

    if (bestHangingProtocol)
    {
        DEBUG_LOG( QString("Hanging protocol que s'aplica: %1").arg(bestHangingProtocol->getName() ) );
        applyHangingProtocol(bestHangingProtocol, layout, patient);
    }
}

void HangingProtocolManager::applyHangingProtocol( int hangingProtocolNumber, ViewersLayout * layout, Patient * patient )
{
    Identifier id;
    id.setValue( hangingProtocolNumber );

    HangingProtocol *hangingProtocol = HangingProtocolsRepository::getRepository()->getItem( id );

    applyHangingProtocol(hangingProtocol,layout, patient);
}

void HangingProtocolManager::applyHangingProtocol( HangingProtocol *hangingProtocol, ViewersLayout *layout, Patient * patient )
{
    HangingProtocolImageSet *hangingProtocolImageSet;
    Series *serie;
    Q2DViewerWidget *viewerWidget;
    PreviousStudiesManager * previousStudiesManager = new PreviousStudiesManager();

    cancelHangingProtocolDowloading(); // Si hi havia algun estudi descarregant, es treu de la llista d'espera

    // TODO aixo no deixa de ser un HACK perquè quedi seleccionat el primer dels widgets
    // Caldria incoporar algun paràmetre per indicar quin és el visor seleccionat per defecte
    // Es buiden tots els visors per tal que no hi hagi res assignat
    layout->setGrid(1,1);

    foreach ( HangingProtocolDisplaySet *displaySet , hangingProtocol->getDisplaySets() )
    {
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
                    if( !displaySet->getToolActivation().isEmpty() ) // Tenim tools activades per defecte des del hanging protocol
                    {
                        if( displaySet->getToolActivation() == "synchronization" ) // S'activa la tool de sincronització
                            viewerWidget->enableSynchronization(true);
                        else // Es desactiva la tool de sincronització, per si estava activada
                            viewerWidget->enableSynchronization(false);
                    }
                    else // es desactiven totes les tools que puguin estar actives
                    {
                        viewerWidget->enableSynchronization(false);
                    }
                }
            }
        }
    }

    INFO_LOG( QString("Hanging protocol aplicat: %1").arg( hangingProtocol->getName() ) );
}

bool HangingProtocolManager::isModalityCompatible(HangingProtocol *protocol, Patient *patient)
{
    foreach (Study *study, patient->getStudies())
    {
        foreach ( QString modality, study->getModalitiesAsSingleString().split("/") )
        {
            if( protocol->getHangingProtocolMask()->getProtocolList().contains( modality ) )
            {
                return true;
            }
        }
    }

    return false;
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
                listOfImages = serie->getFirstVolume()->getImages(); //Es té en compte només les del primer volum que de moment són les que es col·loquen. HACK
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
            if( serie->getFirstVolume()->getImages().size() < restriction.valueRepresentation.toInt() )
                valid = false;
        }
        i++;
    }

    return valid;
}

void HangingProtocolManager::applyDisplayTransformations( Series *serie, int imageNumber, Q2DViewerWidget *viewer, HangingProtocolDisplaySet *displaySet )
{
    viewer->getViewer()->automaticRefresh( false );
    
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

    // Apliquem la orientació desitjada
    viewer->getViewer()->setImageOrientation(displaySet->getPatientOrientation());
    
    int phase = displaySet->getPhase();
    if( phase > -1 )
    {
        viewer->getViewer()->setPhase( phase );
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

bool HangingProtocolManager::isValidImage( Image *image, HangingProtocolImageSet *imageSet, HangingProtocol * hangingProtocol )
{
    if( !image )
    {
        DEBUG_LOG("La imatge passada és NUL·LA! Retornem fals.");
        return false;
    }

    bool valid = true;
    int i = 0;
    QList< HangingProtocolImageSet::Restriction > listOfRestrictions = imageSet->getRestrictions();
    int numberRestrictions = listOfRestrictions.size();
    HangingProtocolImageSet::Restriction restriction;

    while ( valid && i < numberRestrictions )
    {
        restriction = listOfRestrictions.value( i );
        if( restriction.selectorAttribute == "ViewPosition" )
        {
            bool contains = image->getViewPosition().contains( restriction.valueRepresentation, Qt::CaseInsensitive );
            bool match = ( restriction.usageFlag  == HangingProtocolImageSet::NoMatch );
            valid = contains ^ match;
        }
        else if( restriction.selectorAttribute == "ImageLaterality" )
        {
            if( QString(image->getImageLaterality()) != restriction.valueRepresentation.at(0) )
                valid = false;
        }
        else if( restriction.selectorAttribute == "Laterality" )
        {
            // Atenció! Aquest atribut està definit a nivell de sèries
            if( QString(image->getParentSeries()->getLaterality()) != restriction.valueRepresentation )
                valid = false;
        }
        else if( restriction.selectorAttribute == "PatientOrientation" )
        {
            if( !image->getPatientOrientation().contains( restriction.valueRepresentation ) )
                valid = false;
        }
        else if( restriction.selectorAttribute == "CodeMeaning" ) // TODO es podria canviar el nom, ja que és massa genèric. Seria més adequat ViewCodeMeaning per exemple
        {
            bool match = ( restriction.usageFlag  == HangingProtocolImageSet::Match );

            if( !( image->getViewCodeMeaning().contains( restriction.valueRepresentation ) ) )
                valid = false;

            if( !match ) valid = !valid;// just el cas contrari
        }
        else if( restriction.selectorAttribute == "ImageType" )
        {
            bool isLocalyzer = image->getImageType().contains( restriction.valueRepresentation, Qt::CaseInsensitive );
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
            if( serie->getFirstVolume()->getImages().size() < restriction.valueRepresentation.toInt() )
                valid = false;
        }
        i++;
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

            if( isModalityCompatible(hangingProtocol, patient) )
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

                if( hangingProtocol->isStrict() && adjustmentOfHanging != 1.0 )
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
                            viewerWidget->enableSynchronization(true);
                    }
                    else //Es desactiven les tools necessàries
                    {
                        viewerWidget->enableSynchronization(false);
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

QList<Study*> HangingProtocolManager::sortStudiesByDate( const QList<Study*> & studies )
{
    QMultiMap<long,Study*> sortedStudiesByDate;

    foreach( Study *study, studies )
    {
        // Es posa la data en negatiu per ordenar els estudies de gran a petit
        sortedStudiesByDate.insert( -study->getDateTime().toTime_t() , study );
    }
    return sortedStudiesByDate.values();
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
