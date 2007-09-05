/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "mprextensionmediator.h"

#include "extensioncontext.h"

namespace udg{

MPRExtensionMediator::MPRExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

MPRExtensionMediator::~MPRExtensionMediator()
{
}

DisplayableID MPRExtensionMediator::getExtensionID() const
{
    return DisplayableID("MPRExtension",tr("MPR 2D"));
}

bool MPRExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QMPRExtension *mprExtension;

    if ( !(mprExtension = qobject_cast<QMPRExtension*>(extension)) )
    {
        return false;
    }

    mprExtension->setInput( extensionContext.getDefaultVolume() );

    return true;
}

} //udg namespace
