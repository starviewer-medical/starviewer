/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "mprcardiac3dextensionmediator.h"

#include "extensionhandler.h"
#include "extensioncontext.h"

namespace udg {

MPRCardiac3DExtensionMediator::MPRCardiac3DExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

MPRCardiac3DExtensionMediator::~MPRCardiac3DExtensionMediator()
{
}

DisplayableID MPRCardiac3DExtensionMediator::getExtensionID() const
{
    return DisplayableID("MPRCardiac3DExtension",tr("3D Cardiac MPR"));
}

bool MPRCardiac3DExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler)
{
    QMPRCardiac3DExtension *mprCardiac3DExtension;

    if ( !(mprCardiac3DExtension = qobject_cast<QMPRCardiac3DExtension*>(extension)) )
    {
        return false;
    }

    mprCardiac3DExtension->setInput( extensionContext.getDefaultVolume() );

    return true;
}

}
