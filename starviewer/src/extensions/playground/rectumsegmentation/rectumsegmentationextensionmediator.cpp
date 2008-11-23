/**************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "rectumsegmentationextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

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

bool RectumSegmentationExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QRectumSegmentationExtension *rectumSegmentationExtension;

    if ( !(rectumSegmentationExtension = qobject_cast<QRectumSegmentationExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        rectumSegmentationExtension->setInput( input );

    return true;
}


}
