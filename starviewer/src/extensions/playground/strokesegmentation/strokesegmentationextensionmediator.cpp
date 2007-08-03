/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "strokesegmentationextensionmediator.h"

#include "volumerepository.h"
#include "extensionhandler.h"
#include "extensioncontext.h"

namespace udg {

StrokeSegmentationExtensionMediator::StrokeSegmentationExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

StrokeSegmentationExtensionMediator::~StrokeSegmentationExtensionMediator()
{
}

DisplayableID StrokeSegmentationExtensionMediator::getExtensionID() const
{
    return DisplayableID("StrokeSegmentationExtension",tr("Stroke Segmentation"));
}

bool StrokeSegmentationExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler)
{
    QStrokeSegmentationExtension *strokeSegmentationExtension;

    if ( !(strokeSegmentationExtension = qobject_cast<QStrokeSegmentationExtension*>(extension)) )
    {
        return false;
    }

    strokeSegmentationExtension->setInput(VolumeRepository::getRepository()->getVolume( extensionContext.getMainVolumeID() ));

    return true;
}

}
