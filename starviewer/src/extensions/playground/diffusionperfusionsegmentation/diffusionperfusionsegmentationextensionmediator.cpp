/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "diffusionperfusionsegmentationextensionmediator.h"

#include "volumerepository.h"
#include "extensionhandler.h"

namespace udg{

DiffusionPerfusionSegmentationExtensionMediator::DiffusionPerfusionSegmentationExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}


DiffusionPerfusionSegmentationExtensionMediator::~DiffusionPerfusionSegmentationExtensionMediator()
{
}

DisplayableID DiffusionPerfusionSegmentationExtensionMediator::getExtensionID() const
{
    return DisplayableID("DiffusionPerfusionSegmentationExtension",tr("Diffusion-Perfusion Segmentation"));
}

bool DiffusionPerfusionSegmentationExtensionMediator::initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID)
{
    QDifuPerfuSegmentationExtension *difuPerfuExtension;

    if ( !(difuPerfuExtension = qobject_cast<QDifuPerfuSegmentationExtension*>(extension)) )
    {
        return false;
    }

    VolumeRepository* volumeRepository = VolumeRepository::getRepository();
    difuPerfuExtension->setDiffusionInput(volumeRepository->getVolume( mainVolumeID ));

    QObject::connect( difuPerfuExtension, SIGNAL( openPerfusionImage() ), extensionHandler, SLOT( openSerieToCompare() ) );
    QObject::connect( extensionHandler, SIGNAL( secondInput(Volume*) ), difuPerfuExtension, SLOT( setPerfusionInput(Volume*) ) );

    return true;
}

} //udg namespace
