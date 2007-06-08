/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#include "optimalviewpointextensionmediator.h"


// #include "extensionhandler.h"
#include "volumerepository.h"



namespace udg {



OptimalViewpointExtensionMediator::OptimalViewpointExtensionMediator( QObject * parent )
    : ExtensionMediator( parent )
{
}



OptimalViewpointExtensionMediator::~OptimalViewpointExtensionMediator()
{
}



DisplayableID OptimalViewpointExtensionMediator::getExtensionID() const
{
    return DisplayableID( "OptimalViewpointExtension", tr("Optimal Viewpoint") );
}



bool OptimalViewpointExtensionMediator::initializeExtension(
        QWidget * extension, ExtensionHandler * /*extensionHandler*/, Identifier mainVolumeID )
{
    QOptimalViewpointExtension * optimalViewpointExtension;

    if ( !( optimalViewpointExtension = qobject_cast< QOptimalViewpointExtension * >( extension ) ) )
    {
        return false;
    }

    VolumeRepository * volumeRepository = VolumeRepository::getRepository();
    optimalViewpointExtension->setInput( volumeRepository->getVolume( mainVolumeID ) );

    return true;
}



}
