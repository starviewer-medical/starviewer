/**************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "rectumsegmentationextensionmediator.h"

#include "volumerepository.h"
#include "extensionhandler.h"
#include "extensioncontext.h"

namespace udg {

RectumSegmentationExtensionMediator::RectumSegmentationExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}


RectumSegmentationExtensionMediator::~RectumSegmentationExtensionMediator()
{
}

DisplayableID RectumSegmentationExtensionMediator::getExtensionID() const
{
    return DisplayableID("RectumSegmentationExtension",tr("Rectum Segmentation"));
}

bool RectumSegmentationExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler)
{
    QRectumSegmentationExtension *rectumSegmentationExtension;

    if ( !(rectumSegmentationExtension = qobject_cast<QRectumSegmentationExtension*>(extension)) )
    {
        return false;
    }

    rectumSegmentationExtension->setInput(VolumeRepository::getRepository()->getVolume( extensionContext.getMainVolumeID() ));

    return true;
}


}
