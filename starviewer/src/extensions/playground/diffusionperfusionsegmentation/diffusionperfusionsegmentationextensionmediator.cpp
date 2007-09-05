/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "diffusionperfusionsegmentationextensionmediator.h"

#include "extensioncontext.h"

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

bool DiffusionPerfusionSegmentationExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QDifuPerfuSegmentationExtension *difuPerfuExtension;

    if ( !(difuPerfuExtension = qobject_cast<QDifuPerfuSegmentationExtension*>(extension)) )
    {
        return false;
    }

    difuPerfuExtension->setDiffusionInput( extensionContext.getDefaultVolume() );
    // TODO per evitar segmentation faults. S'hauria de fer un tracte més intel·ligent a l'hora de
    // donar l'input de difu i perfu, examninar els estudis, etc
    difuPerfuExtension->setPerfusionInput( extensionContext.getDefaultVolume() );

    return true;
}

} //udg namespace
