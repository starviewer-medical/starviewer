/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "edemasegmentationextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

EdemaSegmentationExtensionMediator::EdemaSegmentationExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

EdemaSegmentationExtensionMediator::~EdemaSegmentationExtensionMediator()
{
}

DisplayableID EdemaSegmentationExtensionMediator::getExtensionID() const
{
    return DisplayableID("EdemaSegmentationExtension",tr("Edema Segmentation"));
}

bool EdemaSegmentationExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QEdemaSegmentationExtension *edemaSegmentationExtension;

    if ( !(edemaSegmentationExtension = qobject_cast<QEdemaSegmentationExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        edemaSegmentationExtension->setInput( input );

    return true;
}


}
