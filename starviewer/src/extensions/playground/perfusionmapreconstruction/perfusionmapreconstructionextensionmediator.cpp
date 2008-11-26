/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "perfusionmapreconstructionextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

PerfusionMapReconstructionExtensionMediator::PerfusionMapReconstructionExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

PerfusionMapReconstructionExtensionMediator::~PerfusionMapReconstructionExtensionMediator()
{
}

DisplayableID PerfusionMapReconstructionExtensionMediator::getExtensionID() const
{
    return DisplayableID("PerfusionMapReconstructionExtension",tr("Perfusion Map Reconstruction"));
}

bool PerfusionMapReconstructionExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QPerfusionMapReconstructionExtension *perfusionMapReconstructionExtension;

    if ( !(perfusionMapReconstructionExtension = qobject_cast<QPerfusionMapReconstructionExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        perfusionMapReconstructionExtension->setInput( input );

    return true;
}

}
