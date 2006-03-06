/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "mutualinformationdirector.h"
#include "mutualinformationparameters.h"

#include "volumerepository.h"

// per la finestra que obrim com a resultat
#include "q2dviewer.h"
// per tenir accés a l'aplication window 
#include "qapplicationmainwindow.h"

// per debug
#include <iostream>

namespace udg{

MutualInformationDirector::MutualInformationDirector( udg::QApplicationMainWindow* app,QObject *parent, const char *name )
 : Director(app, parent, name)
{
    m_applicationWindow = app; // això no és del tot necessari perquè ja ho fa el pare
    
    m_method = new MethodType;    
    m_parameters = 0;
}

MutualInformationDirector::~MutualInformationDirector()
{
    delete m_method;
}

void MutualInformationDirector::setParameters( MutualInformationParameters* parameters )
{
    m_parameters = parameters;
}

void MutualInformationDirector::execute()
{
    bool ok = true; // \TODO Això no es fa servir, cal?
    
    if( !m_parameters )
    {
        // Si no hi ha paràmetres establerts no podem fer res
        // \TODO exception, cerr,... ?
        std::cerr << "MutualInformationDirector::No hi ha parametres establerts" << std::endl;
        ok = false;
    }
    else
    {
        // obtenim el repositori
        udg::VolumeRepository* repository = udg::VolumeRepository::getRepository();
                
        // li indiquem al mètode els paràmetres
        // aquí indiquem que la imatge fixa correspon amb l'actual finestra
        m_parameters->setFixedImageID( m_applicationWindow->getVolumeID() );
        
        m_method->SetInputImages( repository->getVolume( m_parameters->getFixedImageID() )->getItkData(), repository->getVolume( m_parameters->getMovingImageID() )->getItkData() );
        
    
        m_method->SetMetricParameters( m_parameters->getFixedImageStandardDeviation() ,
                                    m_parameters->getMovingImageStandardDeviation(),
                                    m_parameters->getSpatialSamples() );
        
        m_method->SetGaussianFilterParameters( m_parameters->getFixedImageVariance(), 
                                            m_parameters->getMovingImageVariance() );
                                            
        m_method->SetOptimizerParameters( m_parameters->getLearningRate(), 
                                        m_parameters->getNumberOfIterations() ); 
        
        if( m_method->applyMethod() )
        {
            // obrirem les finestres gràcies al coneixement de l'application window
            // Primer es crea el tipus de finestra que volem mostrar
            
            // visió 2D en mode checkerBoard
            Q2DViewer* checkerView = new Q2DViewer();
            // aquest es el que segueix la nova jerarquia de classes
            checkerView->setInput( repository->getVolume( m_parameters->getFixedImageID() ) );
            checkerView->setOverlayInput( this->getRegisteredImage() );
            checkerView->setView( Q2DViewer::Axial );
            checkerView->setDivisions( 10 , 10 , 10 );
            checkerView->setOverlay( Q2DViewer::CheckerBoard );
            checkerView->render();
//            m_applicationWindow->addWorkingAreaWidget( checkerView , tr("Registered Volume [CheckerBoard View]") );
            
            // visió 2D en mode blending
            Q2DViewer* blendView = new Q2DViewer();
            // aquest es el que segueix la nova jerarquia de classes
            blendView->setInput( repository->getVolume( m_parameters->getFixedImageID() ) );
            blendView->setOverlayInput( this->getRegisteredImage() );
            blendView->setView( Q2DViewer::Axial );
            blendView->setOverlay( Q2DViewer::Blend );
            blendView->render();
  //          m_applicationWindow->addWorkingAreaWidget( blendView , tr("Registered Volume [Blend View]") );
            
            // visió 2D en mode RectilinearWipe
            Q2DViewer* wipeView = new Q2DViewer();
            // aquest es el que segueix la nova jerarquia de classes
            wipeView->setInput( repository->getVolume( m_parameters->getFixedImageID() ) );
            wipeView->setOverlayInput( this->getRegisteredImage() );
            wipeView->setView( Q2DViewer::Axial );
            wipeView->setOverlay( Q2DViewer::RectilinearWipe );
            wipeView->render();
    //        m_applicationWindow->addWorkingAreaWidget( wipeView , tr("Registered Volume [Wipe View]") );
        } 
        else
        {
            std::cerr << " No s'ha pogut aplicar el mètode " << std::endl;
        }
        
        
    }
}

udg::Volume* MutualInformationDirector::getRegisteredImage()
{
    udg::Volume* volume = new udg::Volume();
    
    volume->setData( m_method->getRegisteredImage() );
    
    return volume;
}

} // end namespace udg

