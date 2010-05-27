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

namespace udg {

HangingProtocolManager::HangingProtocolManager(QObject *parent)
 : QObject( parent )
{
    m_studiesDownloading = new QMultiHash<QString, StructPreviousStudyDownloading*>();
    m_patient = 0;
    m_previousStudiesManager = new PreviousStudiesManager();

    connect(m_previousStudiesManager, SIGNAL( errorDownloadingPreviousStudy(QString) ), SLOT( errorDowlonadingPreviousStudies(QString) ) );
}

HangingProtocolManager::~HangingProtocolManager()
{
    cancelHangingProtocolDowloading();
    delete m_studiesDownloading;
    delete m_previousStudiesManager;
}

QList<HangingProtocol *> HangingProtocolManager::searchHangingProtocols(Patient *patient)
{
    QList<Study*> previousStudies;
    QHash<QString, QString> originOfPreviousStudies;

    return searchHangingProtocols(patient, previousStudies, originOfPreviousStudies);
}

QList<HangingProtocol *> HangingProtocolManager::searchHangingProtocols(Patient *patient, const QList<Study*> &previousStudies, const QHash<QString, QString> &originOfPreviousStudies)
{
    QList<HangingProtocol * > outputHangingProtocolList;

    QList<Series *> allSeries;

    foreach ( Study *study , sortStudiesByDate( patient->getStudies() ) )
    {
        allSeries += study->getViewableSeries();
    }

    // Buscar el hangingProtocol que s'ajusta millor a l'estudi del pacient
    // Aprofitem per assignar ja les series, per millorar el rendiment
    foreach (HangingProtocol *hangingProtocol, HangingProtocolsRepository::getRepository()->getItems() )
    {
        if( isModalityCompatible(hangingProtocol, patient) )
        {
            QList<Series *> candidateSeries = allSeries; // Copia de les series perquè es van eliminant de la llista al ser assignades
            int numberOfFilledImageSets = 0;

            foreach ( HangingProtocolImageSet * imageSet, hangingProtocol->getImageSets() )
            {
                if( searchSerie( candidateSeries, imageSet , hangingProtocol->getAllDiferent(), hangingProtocol ) )
                {
                    numberOfFilledImageSets++;

                    if ( imageSet->isPreviousStudy() )
                    {
                        imageSet->setDownloaded( true );
                    }
                }
                else
                {
                    if( imageSet->isPreviousStudy() ) // Si és de tipus prèvi, se li dóna una segona oportunitat buscant a previs
                    {
                        Study *referenceStudy = 0, *previousStudy = 0;
                        HangingProtocolImageSet *referenceImageSet = hangingProtocol->getImageSet( imageSet->getPreviousImageSetReference() );

                        if( referenceImageSet->isDownloaded() && referenceImageSet->getSeriesToDisplay() ) // L'estudi de referència està descarregat
                            referenceStudy = referenceImageSet->getSeriesToDisplay()->getParentStudy();
                        else // L'estudi de referència és un previ que encara no s'ha descarregat
                            referenceStudy = referenceImageSet->getPreviousStudyToDisplay();

                        if(referenceStudy)
                        {
                            previousStudy = searchPreviousStudy(hangingProtocol, referenceStudy, previousStudies);

                            if(previousStudy) //S'ha trobat pendent de descarrega
                            {
                                numberOfFilledImageSets++;
                                imageSet->setDownloaded( false );
                                imageSet->setPreviousStudyToDisplay( previousStudy );
                                imageSet->setPreviousStudyPacs( originOfPreviousStudies[previousStudy->getInstanceUID()] );
                            }
                        }
                    }
                }
            }

            bool isValidHangingProtocol = false;

            if( hangingProtocol->isStrict() )
            {
                if ( numberOfFilledImageSets == hangingProtocol->getNumberOfImageSets() )
                {
                    isValidHangingProtocol = true;
                }
            }
            else
            {
                if ( numberOfFilledImageSets > 0 )
                {
                    isValidHangingProtocol = true;
                }
            }

            if( isValidHangingProtocol )
            {
                outputHangingProtocolList << hangingProtocol;
            }
        }
    }

    if (outputHangingProtocolList.size() > 0)
    {
        QString infoLog; // Noms per mostrar al log
        foreach (HangingProtocol *hangingProtocol, outputHangingProtocolList)
        {
            infoLog += QString("%1, ").arg( hangingProtocol->getName() );
        }
        INFO_LOG( QString("Hanging protocols carregats: %1").arg( infoLog ) );
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
    cancelHangingProtocolDowloading(); // Si hi havia algun estudi descarregant, es treu de la llista d'espera

    // TODO aixo no deixa de ser un HACK perquè quedi seleccionat el primer dels widgets
    // Caldria incoporar algun paràmetre per indicar quin és el visor seleccionat per defecte
    // Es buiden tots els visors per tal que no hi hagi res assignat
    layout->setGrid(1,1);

    foreach ( HangingProtocolDisplaySet *displaySet , hangingProtocol->getDisplaySets() )
    {
        HangingProtocolImageSet *hangingProtocolImageSet = hangingProtocol->getImageSet( displaySet->getImageSetNumber() );
        Q2DViewerWidget *viewerWidget = layout->addViewer( displaySet->getPosition() );

        if( hangingProtocolImageSet->isDownloaded() == false )
        {
            viewerWidget->enableDownloadingState();

            StructPreviousStudyDownloading * structPreviousStudyDownloading = new StructPreviousStudyDownloading;
            structPreviousStudyDownloading->widgetToDisplay = viewerWidget;
            structPreviousStudyDownloading->imageSet = hangingProtocolImageSet;
            structPreviousStudyDownloading->hangingProtocol = hangingProtocol;
            structPreviousStudyDownloading->displaySet = displaySet;

            bool isDownloading = m_studiesDownloading->contains( hangingProtocolImageSet->getPreviousStudyToDisplay()->getInstanceUID() );

            m_studiesDownloading->insert( hangingProtocolImageSet->getPreviousStudyToDisplay()->getInstanceUID(), structPreviousStudyDownloading );            
            m_patient = patient;

            connect( m_patient, SIGNAL( patientFused() ), SLOT(previousStudyDownloaded() ) );

            if( !isDownloading )
                m_previousStudiesManager->downloadStudy( hangingProtocolImageSet->getPreviousStudyToDisplay(), hangingProtocolImageSet->getPreviousStudyPacs() );
        }
        else
        {
            setInputToViewer(viewerWidget, hangingProtocolImageSet->getSeriesToDisplay(), hangingProtocolImageSet, displaySet);
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
    viewer->getViewer()->enableRendering( false );
    
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

    QString alignment = displaySet->getAlignment();
    if( !alignment.isEmpty() )
    {
        if( alignment == "right" )
            viewer->getViewer()->alignRight();
        else if (alignment == "left" )
            viewer->getViewer()->alignLeft();
    }
    else
        viewer->getViewer()->setAlignPosition( Q2DViewer::AlignCenter );

    viewer->getViewer()->enableRendering( true );
    viewer->getViewer()->render();
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

Study * HangingProtocolManager::searchPreviousStudy( HangingProtocol * protocol , Study * referenceStudy, const QList<Study*> &previousStudies)
{
    QList<Study*> sortedPreviousStudies = sortStudiesByDate( previousStudies );

    foreach (Study *study, sortedPreviousStudies )
    {
        if( study->getDate() < referenceStudy->getDate() )
        {
            foreach(QString modality, study->getModalities() )
            {
                if( protocol->getHangingProtocolMask()->getProtocolList().contains(modality) )
                {
                    return study;
                }
            }
        }
    }

    return 0;
}

void HangingProtocolManager::previousStudyDownloaded()
{
    if ( m_studiesDownloading->isEmpty() )
    {
        return;
    }

    // Es busca quins estudis nous hi ha
    foreach( Study * study, m_patient->getStudies() )
    {
        for (int i = 0; i < m_studiesDownloading->count( study->getInstanceUID() ) ; i++)
        { // Per cada estudi que esperàvem que es descarregués

            // Agafem l'estructura amb les dades que s'havien guardat per poder aplicar-ho
            StructPreviousStudyDownloading * structPreviousStudyDownloading = m_studiesDownloading->take( study->getInstanceUID() );

            /// Busquem la millor serie de l'estudi que ho satisfa
            QList<Series *> studySeries = study->getSeries();
            Series * series = searchSerie( studySeries, structPreviousStudyDownloading->imageSet, false, structPreviousStudyDownloading->hangingProtocol);
            
            Q2DViewerWidget * viewerWidget = structPreviousStudyDownloading->widgetToDisplay;
            structPreviousStudyDownloading->imageSet->setDownloaded( true );

            viewerWidget->disableDownloadingState();

            setInputToViewer(viewerWidget, series, structPreviousStudyDownloading->imageSet, structPreviousStudyDownloading->displaySet);

            delete structPreviousStudyDownloading;
        }
    }
}

void HangingProtocolManager::errorDowlonadingPreviousStudies(QString studyUID)
{
    if( m_studiesDownloading->contains( studyUID ) )//si és un element que estavem esperant
    {
        int count = m_studiesDownloading->count( studyUID );
        for (int i = 0; i < count ; i++)
        {
            StructPreviousStudyDownloading* element = m_studiesDownloading->take( studyUID ); // s'agafa i es treu de la llista
            element->widgetToDisplay->disableDownloadingState();
            delete element;
        }
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

void HangingProtocolManager::cancelHangingProtocolDowloading()
{
    foreach( QString key, m_studiesDownloading->keys() )
    {
        StructPreviousStudyDownloading* element = m_studiesDownloading->take( key ); // S'agafa i es treu de la llista l'element que s'està esperant
        element->widgetToDisplay->disableDownloadingState(); // es treu el label de downloading
        delete element;
    }
}

void HangingProtocolManager::setInputToViewer(Q2DViewerWidget *viewerWidget, Series *series, HangingProtocolImageSet *imageSet, HangingProtocolDisplaySet *displaySet)
{
    if (series)
    {
        if( series->isViewable() && series->getFirstVolume() )
        {
            viewerWidget->setInput( series->getFirstVolume() );
            qApp->processEvents( QEventLoop::ExcludeUserInputEvents );
            if( imageSet->getTypeOfItem() == "image" )
            {
                viewerWidget->getViewer()->setSlice( imageSet->getImageToDisplay() );
                applyDisplayTransformations(series, imageSet->getImageToDisplay(), viewerWidget, displaySet);
            }
            else
            {
                applyDisplayTransformations(series, 0, viewerWidget, displaySet);
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
