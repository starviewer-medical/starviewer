/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "vsireconstructionextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

VSIReconstructionExtensionMediator::VSIReconstructionExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

VSIReconstructionExtensionMediator::~VSIReconstructionExtensionMediator()
{
}

DisplayableID VSIReconstructionExtensionMediator::getExtensionID() const
{
    return DisplayableID("VSIReconstructionExtension",tr("VSI Reconstruction"));
}

bool VSIReconstructionExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QVSIReconstructionExtension *vsiReconstructionExtension;

    if ( !(vsiReconstructionExtension = qobject_cast<QVSIReconstructionExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        vsiReconstructionExtension->setInput( input );

    return true;
}

}
