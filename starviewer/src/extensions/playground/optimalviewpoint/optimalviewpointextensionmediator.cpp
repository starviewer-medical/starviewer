/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "optimalviewpointextensionmediator.h"

#include "volumerepository.h"
#include "extensionhandler.h"

namespace udg{

OptimalViewpointExtensionMediator::OptimalViewpointExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}


OptimalViewpointExtensionMediator::~OptimalViewpointExtensionMediator()
{
}

DisplayableID OptimalViewpointExtensionMediator::getExtensionID() const
{
    return DisplayableID("OptimalViewpointExtension",tr("Optimal Viewpoint"));
}

bool OptimalViewpointExtensionMediator::initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID)
{
    QOptimalViewpointExtension *optimalViewpointExtension;

    if ( !(optimalViewpointExtension = qobject_cast<QOptimalViewpointExtension*>(extension)) )
    {
        return false;
    }

    VolumeRepository* volumeRepository = VolumeRepository::getRepository();
    optimalViewpointExtension->setInput(volumeRepository->getVolume( mainVolumeID ));

//     QObject::connect( difuPerfuExtension, SIGNAL( openPerfusionImage() ), extensionHandler, SLOT( openPerfusionImage() ) );
//     QObject::connect( extensionHandler, SIGNAL( perfusionImage(Volume*) ), difuPerfuExtension, SLOT( setPerfusionInput(Volume*) ) );

    return true;
}

} //udg namespace
